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

AsyncParameter::~AsyncParameter(){ 

}

void AsyncParameter::readFromBuffer(char* data_buffer_ptr, uint64_t points_to_read){
    DataInterval& di = interval_list_[current_interval_index_];
    if(calcMinMaxPtr)
        calcMinMaxPtr(data_buffer_ptr, points_to_read, di.val_max, di.val_min, is_new_interval_);
    if(data_size_ == 8)
        copyFromBuffer((uint64_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 4)
        copyFromBuffer((uint32_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 2)
        copyFromBuffer((uint16_t*)data_buffer_ptr, points_to_read);
    else if(data_size_ == 1)
        copyFromBuffer((uint8_t*)data_buffer_ptr, points_to_read);
        
}


// is not supported yet 


// is not supported yet 

inline std::vector<int64_t> AsyncParameter::read_dots(Dot* dot_buffer, size_t max_point_number, double from, double to){ return {-1}; }

inline std::vector<int64_t> AsyncParameter::read_dots(double* top_buffer_ptr, double* bottom_buffer_ptr, double* time_buffer_ptr, double from, double to, size_t max_point_number_to_read){
    return {-1};
}

inline bool AsyncParameter::open(bool open_to_write){
    data_size_ = sizeOfTy(this->getRPMType());
    data_size_factor_ = (data_size_ + PRM_TIME_SIZE_);

    unused_points_in_current_interval_ =
        interval_list_[current_interval_index_].size / data_size_factor_;


    if(opened_to_read_ | opened_to_write_)
        return false;
    
    opened_to_read_ = !open_to_write;
    opened_to_write_= open_to_write;

    openNewInterval();
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
    if(!opened_to_write_)
        return points_to_write;
        //open(true);


    intermediate_buffer_.resize(points_to_write * data_size_factor_);
    intermediate_buffer_.resetPos();

    uint64_t base = points_to_write;
    uint8_t* ptr = (uint8_t*)data_buffer_ptr;

    if(interval_list_.size() <= current_interval_index_)
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
            is_new_interval_ = true;
        }
        else{
            readFromBuffer(data_buffer_ptr, points_to_write);
            unused_points_in_current_interval_ -=points_to_write;
            points_to_write=0;
            is_new_interval_ = false;
        }
    }


    ifs_->write(intermediate_buffer_.base_ptr, (base- points_to_write) * data_size_factor_);

    return base - points_to_write;
}



void AsyncParameter::readRawData(uint64_t points_to_read){
    ifs_->read(intermediate_buffer_.current_ptr, points_to_read * data_size_factor_);
    intermediate_buffer_.current_ptr += points_to_read * data_size_factor_;
}


inline void AsyncParameter::openNewInterval(){
    if(ifs_){
        ifs_->close();
        delete ifs_;
        ifs_=nullptr;
    }

    const DataInterval& current_interval = getCufrrentInterval();
    std::string file_name = (current_interval.local ? work_directory_ + "/" : "") + current_interval.file_name;
    if(opened_to_read_){
        ifs_ = new std::fstream(file_name, std::ios::in | std::ios::binary);
        ifs_->seekg(current_interval.offs);
    }
    else if(opened_to_write_){
        if(current_interval.offs == 0)
            ifs_ =new std::fstream(file_name, std::ios::out | std::ios::binary);
        else
            ifs_ =new std::fstream(file_name, std::ios::out | std::ios::binary | std::ios::app);
    }
    // std::ios::app |  std::ios::trunc
}



uint64_t AsyncParameter::read(char* data_buffer_ptr, uint64_t points_to_read){
    if(!opened_to_read_)
        open();

    intermediate_buffer_.resize(points_to_read * data_size_factor_);
    intermediate_buffer_.resetPos();
    time_buffer_.resize(points_to_read * PRM_TIME_SIZE_);

    uint64_t base = points_to_read;
    uint8_t* ptr = (uint8_t*)data_buffer_ptr;

    if(interval_list_.size() <= current_interval_index_)
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


    time_buffer_.replaceLastData();
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

inline const uint64_t AsyncParameter::getVirtualSize(){
    uint64_t total_size = 0;
    for(auto& interval : interval_list_)
        total_size = total_size + interval.size;
    total_size = total_size / (sizeOfTy(this->getRPMType()) + PRM_TIME_SIZE_);
    return  total_size;
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
    target_ty = PRMTypesEn(0x1000 | (uint64_t)target_ty);
    if(getRPMType() == target_ty)
        return this;

    std::vector<DataInterval> data_interval;
    for(auto a : interval_list_){
        auto interval = createInterval({}, 0, target_ty);
        data_interval.push_back(interval);
    }
    auto ret =new AsyncParameter(name, time_interval_, data_interval);
    return ret;
}

ParameterIfs* AsyncParameter::newParameter(){
    AsyncParameter* p =new AsyncParameter("", this->getMainTimeInterval(), this->getDataIntervalList(), false);
    p->parent_parameter_ = parent_parameter_;
    return p;
}
