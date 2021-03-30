#include <type_traits>
#include <set>
#include "ifs/ParameterIO.h"
#include "parser/common.h"


#include "parser/defWarningIgnore.h"
#include "llvm/Support/JSON.h"
#include "parser/undefWarningIgnore.h"

//using namespace llvm;



bool         fromJSON(const llvm::json::Value &data_fragment, DataInterval &data_interval);
DataInterval getDataInterval(llvm::json::Value &data_fragment, llvm::json::Array &data_files_list);

calcMinMaxTy g_calcMinMax_select(PRMTypesEn arg) {
    int sub_type = ( ((int)arg) >> 4);
    int code = 0xFF & ((int)arg);

#define CALC_CASE_OP(T, SUB_T)   \
                case( sizeof(T)|(SUB_T<<4) ) :   return calcMinMax<T> 

    switch (code){
        CALC_CASE_OP(int8_t, 1);
        CALC_CASE_OP(int16_t, 1);
        CALC_CASE_OP(int32_t, 1);
        CALC_CASE_OP(int64_t, 1);
        CALC_CASE_OP(float, 2);
        CALC_CASE_OP(double, 2);
    default:
        return nullptr;
    };
}
 


TypeEn PRMType2JITType(PRMTypesEn arg) {

    switch (arg)
    {
    case PRMTypesEn::PRM_TYPE_U08:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U16:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U32:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U64:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_I08:return TypeEn::int8_jty;
    case PRMTypesEn::PRM_TYPE_I16:return TypeEn::int16_jty;
    case PRMTypesEn::PRM_TYPE_I32:return TypeEn::int32_jty;
    case PRMTypesEn::PRM_TYPE_I64:return TypeEn::int64_jty;
    case PRMTypesEn::PRM_TYPE_F32:return TypeEn::float_jty;
    case PRMTypesEn::PRM_TYPE_F64:return TypeEn::double_jty;
    case PRMTypesEn::PRM_TYPE_U08_T:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U16_T:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U32_T:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_U64_T:return TypeEn::unknown_jty;
    case PRMTypesEn::PRM_TYPE_I08_T:return TypeEn::int8_jty;
    case PRMTypesEn::PRM_TYPE_I16_T:return TypeEn::int16_jty;
    case PRMTypesEn::PRM_TYPE_I32_T:return TypeEn::int32_jty;
    case PRMTypesEn::PRM_TYPE_I64_T:return TypeEn::int64_jty;
    case PRMTypesEn::PRM_TYPE_F32_T:return TypeEn::float_jty;
    case PRMTypesEn::PRM_TYPE_F64_T:return TypeEn::double_jty;
    default:return TypeEn::unknown_jty;
    }
}



PRMTypesEn  JITType2PRMType(TypeEn arg) {
    if (isFloating(arg))
        return (PRMTypesEn)(0x20 | sizeOfTy(arg) );
    if (isInteger(arg))
        return (PRMTypesEn)(0x10 | sizeOfTy(arg) );
    return PRMTypesEn::PRM_TYPE_UNKNOWN;
}

std::string toString(PRMTypesEn arg) {
#define CASE_OP(NAME) case PRMTypesEn::NAME:return #NAME;
    switch (arg)
    {
        CASE_OP(PRM_TYPE_U08);
        CASE_OP(PRM_TYPE_U16);
        CASE_OP(PRM_TYPE_U32);
        CASE_OP(PRM_TYPE_U64);
        CASE_OP(PRM_TYPE_I08);
        CASE_OP(PRM_TYPE_I16);
        CASE_OP(PRM_TYPE_I32);
        CASE_OP(PRM_TYPE_I64);
        CASE_OP(PRM_TYPE_F32);
        CASE_OP(PRM_TYPE_F64);
        CASE_OP(PRM_TYPE_U08_T);
        CASE_OP(PRM_TYPE_U16_T);
        CASE_OP(PRM_TYPE_U32_T);
        CASE_OP(PRM_TYPE_U64_T);
        CASE_OP(PRM_TYPE_I08_T);
        CASE_OP(PRM_TYPE_I16_T);
        CASE_OP(PRM_TYPE_I32_T);
        CASE_OP(PRM_TYPE_I64_T);
        CASE_OP(PRM_TYPE_F32_T);
        CASE_OP(PRM_TYPE_F64_T);
    default:return "TypeEn::unknown_jty";
    }
#undef CASE_OP
}

bool isAsync(PRMTypesEn arg){
    if(arg == PRMTypesEn::PRM_TYPE_UNKNOWN)
        return false;
    return (bool)(0x1000 & (uint64_t)arg);
}



bool fromJSON(const llvm::json::Value &DataFragment, DataInterval & dataInterval) {
    llvm::json::Path::Root root("bare");
    llvm::json::ObjectMapper O(DataFragment, root);
    bool ret= true;

    ret&=O.map("Frequency", dataInterval.frequency);
    ret&=O.map("Offset", dataInterval.offs);
    ret&=O.map("Size", dataInterval.size);
    ret&=O.map("Type", dataInterval.int_type_represintation);

    ret&=O.map("sTime.Begin", dataInterval.time_interval.bgn);
    ret&=O.map("sTime.End", dataInterval.time_interval.end);

    return ret;
}


DataInterval getDataInterval(llvm::json::Value &DataFragment, llvm::json::Array &DataFilesList) {

    DataInterval dataInterval;
    fromJSON(DataFragment, dataInterval);

    int64_t dataIndex;
    llvm::json::Path::Root root("bare");
    llvm::json::ObjectMapper O(DataFragment, root);
    O.map("Index", dataIndex);

    for (auto i : DataFilesList) {
        llvm::json::Path::Root root("bare");
        llvm::json::ObjectMapper O(i, root);
        int64_t fileDataIndex;
        O.map("Index", fileDataIndex);

        if (dataIndex == fileDataIndex) {
            O.map("Name", dataInterval.file_name);
            O.map("Local", dataInterval.local);
        }
    }

    return dataInterval;
}



bool readParametersList(std::string databaseFName, std::vector<ParameterIfs*>& parameterList) {
    std::ifstream ifs(databaseFName);
    if (ifs.bad())
        return false;
    std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );

    auto e =llvm::json::parse(content);
    llvm::json::Value &jValue=e.get();

    llvm::json::Object* jObject          =   jValue.getAsObject();
    llvm::json::Array*  parametersList   = (*jObject)["Parameters.List"].getAsArray();

    llvm::json::Object* parameter        = (*parametersList)[0].getAsObject();

    for (auto i : *parametersList) {
        llvm::json::Object   pObject=*i.getAsObject();

        
        
        if(pObject.find("Data.Fragments.List") != pObject.end()){

            
            llvm::json::Array& DataFilesList     = *(pObject["Data.Files.List"].getAsArray());
            llvm::json::Array& DataFragmentsList = *(pObject["Data.Fragments.List"].getAsArray());

            std::vector<DataInterval> interval_list;
            for(auto k : DataFragmentsList)
                interval_list.push_back(getDataInterval(k, DataFilesList));

            llvm::json::Path::Root root("bare");
            llvm::json::ObjectMapper O(i, root);

            std::string  name;
            TimeInterval time_interval;


            bool ret= true;
            int64_t int_type_represintation=0;
            ret &=O.map(" Name", name);
            ret &=O.map("sTime.Begin", time_interval.bgn);
            ret &=O.map("sTime.End", time_interval.end);
            ret &=O.map("Data.Type", int_type_represintation);

            ParameterIfs* parameter = nullptr;
            if(isAsync((PRMTypesEn)int_type_represintation))
                parameter= new AsyncParameter(name, time_interval, interval_list);
            else
                parameter= new SyncParameter(name, time_interval, interval_list);
            //parameterList.size()==47
            parameterList.push_back(parameter);
        }
    }
    return true;
}

std::vector<ParameterIfs*>  readParametersList(std::string databaseFName) {
    std::vector<ParameterIfs*>  parameterInfoList;
    readParametersList(databaseFName, parameterInfoList);
    return parameterInfoList;
}


ParameterIfs* retyping(ParameterIfs * a, PRMTypesEn target_ty, const std::string& name){
    if(a == nullptr)
        return nullptr;
    return a->retyping(target_ty, name);
}


ParameterIfs* intersection(ParameterIfs* a, ParameterIfs* b, PRMTypesEn target_ty, const std::string& name){
    if(a == nullptr)
        return b;
    if(b == nullptr)
        return a;
    return a->intersection(b, target_ty, name);

}

ParameterIfs* intersection(std::vector<ParameterIfs*> arg_list, PRMTypesEn target_ty, const std::string& name){
    std::set<ParameterIfs*> parameterSet;
    ParameterIfs* p = nullptr;
    for(auto i : arg_list){
        p = intersection(p, i, target_ty, name);
        parameterSet.insert(p);
    }

    for(auto i : arg_list)
        parameterSet.erase(i);

    parameterSet.erase(nullptr);
    //if(p)
    parameterSet.erase(p);

    for(auto i : parameterSet)
        delete i;

    return p;
}