#include <type_traits>
#include <set>
#include "ifs/ParameterIO.h"
#include "parser/common.h"


#include "parser/defWarningIgnore.h"
#include "llvm/Support/JSON.h"
#include "parser/undefWarningIgnore.h"





SyncParameter::SyncParameter(
    std::string name,
    const std::vector<DataInterval>& interval_list,
    bool save_fnames
){
    name_=name;
    //for (auto i : interval_list)
    //    interval_list_.push_back(i);

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
    calcExtendedInfo();
}


SyncParameter::SyncParameter(
    std::string name,
    const TimeInterval& time_interval,
    const std::vector<DataInterval>& interval_list,
    bool save_fnames
):SyncParameter(name, interval_list, save_fnames){
    time_interval_ = time_interval;
    calcExtendedInfo();
}



inline bool SyncParameter::open(bool open_to_write){
    if(opened_to_read_ | opened_to_write_)
        return false;
    ifs_ = nullptr;

    //seek(0);
    opened_to_read_ = !open_to_write;
    opened_to_write_= open_to_write;
    return opened_to_read_;
}

inline bool SyncParameter::close(){
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

bool SyncParameter::seek(int64_t point_umber){

    point_number_ = point_umber;
    const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
    current_interval_index_ = getDataIntervalIndex(current_time);

    if(current_interval_index_ != -1){
        openNewInterval((double)current_interval_index_);
        int64_t local_start_pos =    (int)((current_time - getTimeInterval(current_interval_index_).bgn) * frequency_);
        ifs_->seekg(local_start_pos * sizeof_data_type_);
    }
    return true;
}



uint64_t SyncParameter::write(char* data_buffer_ptr, uint64_t point_number){
    if(!opened_to_read_)
        open();

    uint64_t points_to_write = point_number;
    char* ptr = data_buffer_ptr;
    //std::vector<DataInterval> &interval_list = parameter_info_.interval_list;


    while(points_to_write){
        const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;

        int64_t di_index = getDataIntervalIndex(current_time);
        int64_t local_points_to_write;

        if(di_index == -1){

            double end_time;
            bool   is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);

            if(is_last_interval)
                end_time = time_interval_.end + additional_time_;
            else
                end_time = getTimeInterval(current_interval_index_ + 1).bgn;

            local_points_to_write = (int)((end_time - current_time) * frequency_);

            if(points_to_write <= local_points_to_write)
                local_points_to_write = points_to_write;
            else if(is_last_interval){
                return point_number - points_to_write + local_points_to_write;
            }

        }
        else{
            DataInterval& di = interval_list_[(size_t)di_index];

            int64_t local_start_pos = (int)((current_time - di.time_interval.bgn) * frequency_);
            local_points_to_write = (int)((di.time_interval.end + additional_time_ - current_time) * frequency_);

            bool is_new_interval = (di_index != current_interval_index_) || (ifs_ == nullptr);
            if(is_new_interval){
                openNewInterval((double)di_index);
                //ifs_->seekg(local_start_pos * sizeof_data_type_);
            }

            if(points_to_write <= local_points_to_write)
                local_points_to_write = points_to_write;

            if(calcMinMaxPtr)
                calcMinMaxPtr(ptr, local_points_to_write, di.val_max, di.val_min, is_new_interval);

            ifs_->write(ptr, local_points_to_write * sizeof_data_type_);
        }

        ptr += local_points_to_write * sizeof_data_type_;
        points_to_write -= local_points_to_write;
        point_number_ += local_points_to_write;
    }

    return point_number - points_to_write;
}

uint64_t SyncParameter::read(char* data_buffer_ptr, uint64_t point_number){
    if(!opened_to_read_)
        open();

    uint64_t points_to_read = point_number;
    uint8_t* ptr = (uint8_t*)data_buffer_ptr;
    //std::vector<DataInterval> &interval_list = parameter_info_.interval_list;
    // (*((ParameterIfs*)this)).point_number_ == 1704067;
    while(points_to_read){
        const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;

        // debug cond =>{ points_to_read == 393088 }
        int64_t di_index = getDataIntervalIndex(current_time);
        int64_t local_points_to_read;

        if(di_index == -1){

            const bool is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);

            const double end_time = is_last_interval ?
                time_interval_.end + additional_time_ :
                getTimeInterval(current_interval_index_ + 1).bgn;

            local_points_to_read = (int)((end_time - current_time) * frequency_);

            if(points_to_read <= local_points_to_read)
                local_points_to_read = points_to_read;
            else if(is_last_interval){
                memset(ptr, 0, (size_t)(local_points_to_read * sizeof_data_type_));
                return point_number - points_to_read + local_points_to_read;
            }

            memset(ptr, 0, (size_t)(local_points_to_read * sizeof_data_type_));

        }
        else{
            const DataInterval& di = interval_list_[(size_t)di_index];

            int64_t local_start_pos = (int)((current_time - di.time_interval.bgn) * frequency_);
            local_points_to_read = (int)((di.time_interval.end + additional_time_ - current_time) * frequency_);

            if(di_index != current_interval_index_ || (ifs_ == nullptr)){
                openNewInterval((double)di_index);
                ifs_->seekp(local_start_pos * sizeof_data_type_);
            }

            if(points_to_read <= local_points_to_read)
                local_points_to_read = points_to_read;

            ifs_->read((char*)ptr, local_points_to_read * sizeof_data_type_);
        }

        ptr += local_points_to_read * sizeof_data_type_;
        points_to_read -= local_points_to_read;
        point_number_ += local_points_to_read;
    }

    return point_number - points_to_read;
}


ParameterIfs* SyncParameter::intersection(ParameterIfs* b, PRMTypesEn target_ty, const std::string& name){
    auto parameter_a = this;
    auto parameter_b = (SyncParameter*)b;

    if(parameter_a == parameter_b)
        return this;

    if(parameter_a->frequency_ != parameter_b->frequency_){
        error_info_ = "different frequencys is not supported yet ";
        return nullptr;
    }

    else if(parameter_a->frequency_ <= 0.0){
        error_info_ = "async parameter is not supported yet";
        return nullptr;
    }

    auto frequency = parameter_a->frequency_;
    std::vector<DataInterval> ret;

    for(auto a : parameter_a->interval_list_){
        for(auto b : parameter_b->interval_list_){
            auto t =a & b;
            if(!isEmpty(t)){
                auto interval = createInterval(t, frequency, target_ty);
                if(!ret.empty()){
                    auto t2 = ret.back() || interval;
                    if(!isEmpty(t))
                        ret.back() = createInterval(t2, frequency, target_ty);
                    else
                        ret.push_back(interval);
                }
                else
                    ret.push_back(interval);
            }
        }
    }
    return new SyncParameter(name, time_interval_, ret);
}




SyncParameter* SyncParameter::enlargeFrequency(int64_t arg, PRMTypesEn target_ty, const std::string& name){
    auto parameter_a=this;
    SyncParameter* ret= this;
    std::vector<DataInterval> data_interval;

    double frequency=.0;

    if(arg > 1)
        frequency = this->frequency_ * arg;
    else if(arg < -1){
        if(((int)this->frequency_) % arg)
            return nullptr;
        frequency = this->frequency_ / arg;
    }
    else
        return this;

    for(auto a : parameter_a->interval_list_){
        auto interval = createInterval(a.time_interval, frequency, target_ty);
        data_interval.push_back(interval);
    }

    return new SyncParameter(name, time_interval_, data_interval);
}

ParameterIfs* SyncParameter::retyping(PRMTypesEn target_ty, const std::string& name) {
    if(getRPMType() == target_ty)
        return this;

    std::vector<DataInterval> data_interval;
    for(auto a : interval_list_){
        auto interval = createInterval({}, frequency_, target_ty);
        data_interval.push_back(interval);
    }
    return new SyncParameter(name, time_interval_, data_interval);
}

ParameterIfs* SyncParameter::newParameter(){
    return new SyncParameter("", this->getMainTimeInterval(), this->getDataIntervalList(), false);
}

inline bool SyncParameter::calcExtendedInfo(){
    if(interval_list_.size()){
        frequency_=interval_list_.front().frequency;
        for(auto i : interval_list_)
            if(frequency_ != i.frequency){
                error_info_ = "calcExtendedInfo - different frequencys";
                return false;
            }
    }
    return true;
}




