#include <type_traits>
#include <set>
#include "ifs/ParameterIO.h"
#include "parser/common.h"


#include "parser/defWarningIgnore.h"
#include "llvm/Support/JSON.h"
#include "parser/undefWarningIgnore.h"




AsyncParameter::AsyncParameter(
    std::string name,
    const std::vector<DataInterval>& interval_list,
    bool save_fnames
){
    name_=name;
    parent_parameter_ = this;
    interval_list_ = interval_list;
    numer_of_intervals_ = interval_list.size();
    if(save_fnames == false){
        for(auto& i : interval_list_){
            i.file_name = "";
            i.local = true;
        }
    }

    if(interval_list_.size()){
        sizeof_data_type_=sizeOfTy(interval_list_.front().type);
        type_ = interval_list_.front().type;
        calcMinMaxPtr = g_calcMinMax_select(type_);
    }


    auto bgn=interval_list_.front().time_interval.bgn;
    auto end=interval_list_.back().time_interval.end;

    time_interval_ ={bgn, end};

}


AsyncParameter::AsyncParameter(
    std::string name,
    const TimeInterval& time_interval,
    const std::vector<DataInterval>& interval_list,
    bool save_fnames
):AsyncParameter(name, interval_list, save_fnames){
    parent_parameter_ = this;
    time_interval_ = time_interval;
}

void AsyncParameter::readFromBuffer(char* data_buffer_ptr, uint64_t points_to_read){
    if(data_size_ == 8)
        copyFromBuffer((uint64_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 4)
        copyFromBuffer((uint32_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 2)
        copyFromBuffer((uint16_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 1)
        copyFromBuffer((uint8_t*)data_buffer_ptr, points_to_read);
}

inline bool AsyncParameter::open(bool open_to_write){
    data_size_ = sizeOfTy(this->getRPMType());
    data_size_factor_ = (data_size_ + PRM_TIME_SIZE_);

    if(opened_to_read_ | opened_to_write_)
        return false;
    ifs_ = nullptr;

    opened_to_read_ = !open_to_write;
    opened_to_write_= open_to_write;
    return opened_to_read_;
}

inline bool AsyncParameter::close(){
    if(opened_to_read_ | opened_to_write_){
        if(ifs_)
            ifs_->close();
        ifs_ = nullptr;
        opened_to_read_  = false;
        opened_to_write_ = false;
        return true;
    }
    return false;
}

uint64_t AsyncParameter::write(char* data_buffer_ptr, uint64_t points_to_write){
    if(!opened_to_read_)
        open();

    intermediate_buffer_.resize(points_to_write * data_size_factor_);
    

    uint64_t base = points_to_write;
    uint8_t* ptr = (uint8_t*)data_buffer_ptr;

    if(interval_list_.size() > current_interval_index_)
        return 0;

    while(points_to_write){
        if(points_to_write >= unused_points_in_current_interval_){
            readFromBuffer(data_buffer_ptr,unused_points_in_current_interval_);
            current_interval_index_++;
            points_to_write-=unused_points_in_current_interval_;
            if(interval_list_.size() > current_interval_index_){
                unused_points_in_current_interval_ =
                    interval_list_[current_interval_index_].size / data_size_factor_;
                openNewInterval();
            }
            else{
                // it is error point (unpossible point)
                break;
            }
        }
        else{
            readFromBuffer(data_buffer_ptr, points_to_write);
            unused_points_in_current_interval_ -=points_to_write;
            points_to_write=0;
        }
    }


    ifs_->write(intermediate_buffer_.base_ptr, base * data_size_factor_);

    return base - points_to_write;
}



void AsyncParameter::readRawData(uint64_t points_to_read){
    ifs_->read(intermediate_buffer_.current_ptr, points_to_read * data_size_factor_);
    intermediate_buffer_.base_ptr += points_to_read * data_size_factor_;
}



uint64_t AsyncParameter::read(char* data_buffer_ptr, uint64_t points_to_read){
    if(!opened_to_read_)
        open();

    intermediate_buffer_.resize(points_to_read * data_size_factor_);
    time_buffer_.resize(points_to_read * PRM_TIME_SIZE_);

    uint64_t base = points_to_read;
    uint8_t* ptr = (uint8_t*)data_buffer_ptr;

    if(interval_list_.size() > current_interval_index_)
        return 0;


    while(points_to_read){
        if(points_to_read >= unused_points_in_current_interval_){
            readRawData(unused_points_in_current_interval_);
            current_interval_index_++;
            points_to_read-=unused_points_in_current_interval_;
            if(interval_list_.size() > current_interval_index_){
                unused_points_in_current_interval_ =
                    interval_list_[current_interval_index_].size / data_size_factor_;
                openNewInterval();

            }
            else{
                //////////////////
                //fill the buffer with zero values
                //////////////////
                break;
            }
        }
        else{
            readRawData(points_to_read);
            unused_points_in_current_interval_ -=points_to_read;
            points_to_read=0;
        }
    }


    time_buffer_.replaceLastData(base);
    intermediate_buffer_.resetPos();


    if(data_size_ == 8)
        copyToBuffer((uint64_t*)data_buffer_ptr, base);
    else if(data_size_ == 4)
        copyToBuffer((uint32_t*)data_buffer_ptr, base);
    else if(data_size_ == 2)
        copyToBuffer((uint16_t*)data_buffer_ptr, base);
    else if(data_size_ == 1)
        copyToBuffer((uint8_t*)data_buffer_ptr, base);

    time_buffer_.resetPos();

    return base - points_to_read;
}

ParameterIfs* AsyncParameter::intersection(ParameterIfs* b, PRMTypesEn target_ty, const std::string& name){
    auto parameter_a = this;
    auto parameter_b = (AsyncParameter*)b;

    if(parameter_a->parent_parameter_ != parameter_b->parent_parameter_)
        return nullptr;

    return this;
    //return newParameter();
}

ParameterIfs* AsyncParameter::enlargeFrequency(int64_t arg, PRMTypesEn target_ty, const std::string& name){
    return nullptr;
}

ParameterIfs* AsyncParameter::retyping(PRMTypesEn target_ty, const std::string& name){
    if(getRPMType() == target_ty)
        return this;

    std::vector<DataInterval> data_interval;
    for(auto a : interval_list_){
        auto interval = createInterval({}, 0, target_ty);
        data_interval.push_back(interval);
    }
    return new AsyncParameter(name, time_interval_, data_interval);
}

ParameterIfs* AsyncParameter::newParameter(){
    AsyncParameter* p =new AsyncParameter("", this->getMainTimeInterval(), this->getDataIntervalList(), false);
    p->parent_parameter_ = parent_parameter_;
    return p;
}
