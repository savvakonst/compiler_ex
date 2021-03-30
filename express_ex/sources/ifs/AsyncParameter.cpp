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
    calcExtendedInfo();
}


AsyncParameter::AsyncParameter(
    std::string name,
    const TimeInterval& time_interval,
    const std::vector<DataInterval>& interval_list,
    bool save_fnames
):AsyncParameter(name, interval_list, save_fnames){
    parent_parameter_ = this;
    time_interval_ = time_interval;
    calcExtendedInfo();
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
        auto interval = createInterval({}, frequency_, target_ty);
        data_interval.push_back(interval);
    }
    return new AsyncParameter(name, time_interval_, data_interval);
}

ParameterIfs* AsyncParameter::newParameter(){
    AsyncParameter* p =new AsyncParameter("", this->getMainTimeInterval(), this->getDataIntervalList(), false);

    p->parent_parameter_ = parent_parameter_;

    return p;
}
