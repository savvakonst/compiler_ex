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
    time_interval_ = time_interval;
    calcExtendedInfo();
}