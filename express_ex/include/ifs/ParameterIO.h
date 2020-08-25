#ifndef PARAMETERIO_H
#define PARAMETERIO_H
#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>  

#include "ParameterIfs.h"





class SyncParameter: public ParameterIfs {
public:

    SyncParameter() {

    }

    SyncParameter(std::string name, const TimeInterval &time_interval,
        const std::vector<DataInterval> &interval_list, bool save_fnames=true);

    ~SyncParameter() {

    }

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(Dot * dot_buffer, size_t max_point_number, double from, double to) override { return { -1 }; };

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(
        double * top_buffer_ptr, double * bottom_buffer_ptr, double * time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) override { return { -1 }; };


    void addIntrval(const  DataInterval &interval) {
        interval_list_.push_back(interval);
    }

    bool open(bool open_to_write = false) {
        if (opened_to_read_ | opened_to_write_)
            return false;
        ifs_ = nullptr;

        //seek(0);
        opened_to_read_ = !open_to_write;
        opened_to_write_= open_to_write;
        return opened_to_read_;
    }

    bool close() {
        if (opened_to_read_ | opened_to_write_) {
            if (ifs_) 
                ifs_->close();
            ifs_ = nullptr;
            opened_to_read_  = false;
            opened_to_write_ = false;
            return true;
        }
        return false;
    }

    bool    seek(int64_t point_umber);
    int64_t write(char * data_buffer_ptr, int64_t point_number);
    int64_t read(char * data_buffer_ptr, int64_t point_number);

    PRMTypesEn  getRPMType() { return interval_list_[0].type; }
    int64_t     getVirtualSize() {return frequency_ * (time_interval_.end - time_interval_.bgn);}    

    void setName(const std::string &name) {
        name_=name;
        if (interval_list_.size() == 1)
            interval_list_.front().file_name = name + ".dat";

        int64_t index=0;
        for (auto i : interval_list_)
            i.file_name = name+"_"+std::to_string(index++)+".dat";
    }

    std::stringstream &stream(std::stringstream & OS,  std::string offset="") const override{
        OS << offset << "ParameterInfo{\n";
        OS << offset << "  parameter_name: " << name_ << "\n";
        OS << offset << "  time_interval.end: " << time_interval_.end << "\n";
        OS << offset << "  time_interval.bgn: " << time_interval_.bgn << "\n";
        OS << offset << "  interval_list: [" << "\n";
        for (auto interval : interval_list_)
            ::stream(OS, interval, "    ");
        OS << offset << "  ]\n";
        OS << offset << "}\n";
        return OS;
    }


    SyncParameter *  intersection(SyncParameter  *b, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string &name="");
    SyncParameter *  enlargeFrequency(int64_t arg,PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string &name="");
    SyncParameter *  retyping(PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string &name="");



    friend  void readParametersList(std::string databaseFName, std::vector<SyncParameter>& parameterList);
    friend class ParametersDB;


protected:

    bool calcExtendedInfo() {
        if(interval_list_.size()){
            frequency_=interval_list_.front().frequency;
            for (auto i : interval_list_)
                if (frequency_ != i.frequency) {
                    error_info_ = "calcExtendedInfo - different frequencys";
                    return false;
                }
        }
        return true;
    }

    inline const TimeInterval &getTimeInterval(int64_t nterval_index) {
        return interval_list_[nterval_index].time_interval;
    }

    inline const DataInterval &getCufrrentInterval() {
        return interval_list_[current_interval_index_];
    }

    inline int64_t getDataIntervalIndex(double time) {

        for (int64_t i =current_interval_index_ <0?0:current_interval_index_; i< numer_of_intervals_; i++) {
            DataInterval &a =interval_list_[i];
            if ((a.time_interval.bgn <= time) && (time < a.time_interval.end)) return i;
        }
        return -1;
    }

    inline void openNewInterval(double di_index) {
        if (ifs_) {
            current_interval_index_=di_index;
            ifs_->close();
            delete ifs_;
            ifs_=nullptr;
        }
        if (opened_to_read_) {
            ifs_ = new std::fstream(getCufrrentInterval().file_name, std::ios::in | std::ios::binary);
            bool op =ifs_->is_open();
        }
        else if(opened_to_write_)
            ifs_ =new std::fstream(getCufrrentInterval().file_name, std::ios::out | std::ios::binary);
        // std::ios::app |  std::ios::trunc

    }

    double  frequency_  = -1;
};



bool                        readParametersList(std::string database_Fame, std::vector<SyncParameter* >& parameter_list);
std::vector<SyncParameter*> readParametersList(std::string database_Fame);
SyncParameter *             intersection(SyncParameter * a, SyncParameter * b, PRMTypesEn target_ty, const std::string & name);
SyncParameter *             intersection(std::vector<SyncParameter*> arg_list, PRMTypesEn target_ty, const std::string &name);
SyncParameter *             retyping(SyncParameter  *a, PRMTypesEn target_ty, const std::string & name);


class ParametersDB
{
public:

    ParametersDB() {

    }

    ParametersDB(SyncParameter* parameterInfo, std::string code) {
        db_parameters_.push_back(parameterInfo);
        calcExtendedInfo(parameterInfo);
    }

    ParametersDB(std::vector<SyncParameter *> &parameterInfoList, std::string code) {
        db_parameters_=parameterInfoList;
        for (auto &i : db_parameters_)
            calcExtendedInfo(i);
    }

    ParametersDB(const std::vector<std::string> &file_name_list) {
        addParametersSet(file_name_list);
    }

    ~ParametersDB() {
        for (auto &i : db_parameters_)
            delete i;
        for (auto &i : output_parameters_)
            delete i;
    }

    std::vector<SyncParameter *>&   getOutputParameters() {
        std::vector<SyncParameter*> readParametersList(std::string database_Fame);
    }

    bool addParametersSet(const std::string &file_name) {
        return readParametersList(file_name, db_parameters_);
    }

    bool addParametersSet(const std::vector<std::string> &file_name_list) {
        auto ret= true;
        for(auto file_name : file_name_list)
            ret&=readParametersList(file_name, db_parameters_);
        return ret;
    }

    const std::vector<std::string>  getNamesList() {
        std::vector<std::string> namesList;
        for (auto i : db_parameters_)
            namesList.push_back(i->getName());
        return namesList;
    }

    const std::vector<SyncParameter*>& getDBParameterList() const  {
        return db_parameters_;
    }

    inline SyncParameter* operator[] (std::string name) {
        for (auto &i : db_parameters_)
            if (i->getName() == name)
                return i;
        return nullptr;
    }

    bool createSyncParameter(std::string name, std::vector<SyncParameter*> prototype_parameter_list, PRMTypesEn target_ty=PRMTypesEn::PRM_TYPE_UNKNOWN) {

        if (prototype_parameter_list.empty())
            return false;

        auto front_parameter=prototype_parameter_list.front();

        auto interval_count = front_parameter->interval_list_.size();
        auto virtual_size   = front_parameter->getVirtualSize();
        for (auto i : prototype_parameter_list) {
            if (virtual_size != front_parameter->getVirtualSize()) {
                error_info_ = "intervals have incompatible sizes";
            }
            //if (interval_count != i->interval_list_.size()) {
            //    error_info_ = "different intervals is not supporded yet";
            //    return false;
            //}
        }
        std::vector<DataInterval> data_interval;

        for (auto i : prototype_parameter_list ) {
            if (!intersection(data_interval, front_parameter, i))
                return false;
        }
    }

private:
    bool intersection(std::vector<DataInterval> &ret,SyncParameter* parameter_a, SyncParameter* parameter_b, const std::string &name="", PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN) {
        if (parameter_a->frequency_ != parameter_b->frequency_) {
            error_info_ = "different frequencys is not supported yet ";
            return false;
        }
        else if (parameter_a->frequency_ <= 0.0) {
            error_info_ = "async parameter is not supported yet";
            return false;
        }
        auto frequency = parameter_a->frequency_;

        for (auto a : parameter_a->interval_list_) {
            for (auto b : parameter_b->interval_list_) {
                auto t =a & b;
                if (!isEmpty(t)) {
                    auto interval = createInterval(t, frequency, target_ty);
                    if (!ret.empty()) {
                        auto t2 = ret.back() || interval;
                        if (!isEmpty(t))
                            ret.back() = createInterval(t2, frequency, target_ty);
                        else
                            ret.push_back(interval);
                    }
                    else
                        ret.push_back(interval);
                }   
            }
        }
        return true;
    }


    bool calcExtendedInfo(SyncParameter * arg) {
        return true;
    }

    /// dbParameters contains all parametersInfo from 
    /// all available data sets
    /// 
    std::vector<SyncParameter*> db_parameters_;

    std::vector<SyncParameter*> output_parameters_;
    std::vector<SyncParameter*> input_parameters_ifs_;

    std::string error_info_ = "";

};

#endif