#ifndef SYNC_PARAMETER_H
#define SYNC_PARAMETER_H

#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>  

#include "ParameterIfs.h"


class DLL_EXPORT SyncParameter: public ParameterIfs{
public:

    SyncParameter(){}
    SyncParameter(std::string name, const std::vector<DataInterval>& interval_list, bool save_fnames);
    SyncParameter(std::string name, const TimeInterval& time_interval,
                  const std::vector<DataInterval>& interval_list, bool save_fnames=true);

    ~SyncParameter(){}


    bool isAsync(){return false;}
    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(Dot* dot_buffer, size_t max_point_number, double from, double to) override{ return {-1}; };

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(
        double* top_buffer_ptr, double* bottom_buffer_ptr, double* time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) override{
        return {-1};
    };

    bool open(bool open_to_write = false){
        if(opened_to_read_ | opened_to_write_)
            return false;
        ifs_ = nullptr;

        //seek(0);
        opened_to_read_ = !open_to_write;
        opened_to_write_= open_to_write;
        return opened_to_read_;
    }

    bool close(){
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

    bool    seek(int64_t point_umber);
    int64_t write(char* data_buffer_ptr, int64_t point_number) override;
    int64_t read(char* data_buffer_ptr, int64_t point_number) override;

    PRMTypesEn  getRPMType(){ return interval_list_[0].type; }
    int64_t     getVirtualSize(){ return (int64_t)(frequency_ * (time_interval_.end - time_interval_.bgn + additional_time_)); }

    void setName(const std::string& name){
        name_=name;
        if(interval_list_.size() == 1)
            interval_list_.front().file_name = name + ".dat";

        int64_t index=0;
        for(auto i : interval_list_)
            i.file_name = name + "_" + std::to_string(index++) + ".dat";
    }


    ParameterIfs* intersection(ParameterIfs* b, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="")override;
    SyncParameter* enlargeFrequency(int64_t arg, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="");
    ParameterIfs* retyping(PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="") override;

    virtual ParameterIfs* newParameter()override;

    friend void readParametersList(std::string databaseFName, std::vector<ParameterIfs>& parameterList);
    friend class ParametersDB;


protected:

    bool calcExtendedInfo(){
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

    inline const TimeInterval& getTimeInterval(int64_t nterval_index){
        return interval_list_[(size_t)nterval_index].time_interval;
    }

    inline const DataInterval& getCufrrentInterval(){
        return interval_list_[(size_t)current_interval_index_];
    }

    inline int64_t getDataIntervalIndex(double time){
        // debug cond => { time == 134.00097656250000 }
        for(int64_t i = (current_interval_index_ < 0 ? 0 : current_interval_index_); i < numer_of_intervals_; i++){
            DataInterval& a =interval_list_[(size_t)i];
            if((a.time_interval.bgn <= time) && (time < (a.time_interval.end + additional_time_))) return i;
        }
        return -1;
    }

    inline void openNewInterval(double di_index){
        if(ifs_){
            current_interval_index_ = (int64_t)di_index;
            ifs_->close();
            delete ifs_;
            ifs_=nullptr;
        }
        std::string file_name = (getCufrrentInterval().local ? work_directory_ + "/" : "") + getCufrrentInterval().file_name;
        if(opened_to_read_){
            ifs_ = new std::fstream(file_name, std::ios::in | std::ios::binary);
            bool op =ifs_->is_open();
        }
        else if(opened_to_write_)
            ifs_ =new std::fstream(file_name, std::ios::out | std::ios::binary);
        // std::ios::app |  std::ios::trunc

    }
    const double additional_time_ = 0.0009765625;
    PRMTypesEn type_    = PRMTypesEn::PRM_TYPE_UNKNOWN;
    double  frequency_  = -1;
};



#endif