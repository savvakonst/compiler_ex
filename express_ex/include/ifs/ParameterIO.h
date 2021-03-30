#ifndef PARAMETERIO_H
#define PARAMETERIO_H
#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>  

#include "ParameterIfs.h"

#pragma warning( push )  
#pragma warning( disable : 4251)


#include "AsyncParameter.h"
#include "SyncParameter.h"


DLL_EXPORT bool readParametersList(std::string database_fname, std::vector<ParameterIfs* >& parameter_list);
DLL_EXPORT std::vector<ParameterIfs*> readParametersList(std::string database_Fame);
DLL_EXPORT ParameterIfs* intersection(ParameterIfs* a, ParameterIfs* b, PRMTypesEn target_ty, const std::string& name);
DLL_EXPORT ParameterIfs* intersection(std::vector<ParameterIfs*> arg_list, PRMTypesEn target_ty, const std::string& name);
DLL_EXPORT ParameterIfs* retyping(ParameterIfs* a, PRMTypesEn target_ty, const std::string& name);

calcMinMaxTy g_calcMinMax_select(PRMTypesEn arg);


class DLL_EXPORT ParametersDB
{
public:

    ParametersDB() {

    }

    ParametersDB(ParameterIfs* parameterInfo, std::string code) {
        db_parameters_.push_back(parameterInfo);
        calcExtendedInfo(parameterInfo);
    }

    ParametersDB(std::vector<ParameterIfs*> &parameterInfoList, std::string code) {
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
    /*
    std::vector<SyncParameter *>&   getOutputParameters() {
        return  readParametersList(std::string database_Fame);
    }*/

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

    const std::vector<ParameterIfs*>& getDBParameterList() const  {
        return db_parameters_;
    }

    inline ParameterIfs* operator[] (std::string name) {
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
        return true;
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


    bool calcExtendedInfo(ParameterIfs* arg) {
        return true;
    }

    /// dbParameters contains all parametersInfo from 
    /// all available data sets
    /// 
    std::vector<ParameterIfs*> db_parameters_;

    std::vector<ParameterIfs*> output_parameters_;
    std::vector<ParameterIfs*> input_parameters_ifs_;

    std::string error_info_ = "";

};


#pragma warning( pop )  

#endif