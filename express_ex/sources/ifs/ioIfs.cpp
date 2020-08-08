
//#include <strstream>

#include "ioIfs.h"
#include "common.h"
#include "llvm/Support/JSON.h"
using namespace llvm;

bool         fromJSON(const json::Value &data_fragment, DataInterval &data_interval);
DataInterval getDataInterval(json::Value &data_fragment, json::Array &data_files_list);



TypeEn RPMType2JITType(RPMTypesEn arg) {

    switch (arg)
    {
    case RPMTypesEn::PRM_TYPE_U08:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U16:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U32:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U64:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_I08:return TypeEn::int8_jty;
    case RPMTypesEn::PRM_TYPE_I16:return TypeEn::int16_jty;
    case RPMTypesEn::PRM_TYPE_I32:return TypeEn::int32_jty;
    case RPMTypesEn::PRM_TYPE_I64:return TypeEn::int64_jty;
    case RPMTypesEn::PRM_TYPE_F32:return TypeEn::float_jty;
    case RPMTypesEn::PRM_TYPE_F64:return TypeEn::double_jty;
    case RPMTypesEn::PRM_TYPE_U08_T:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U16_T:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U32_T:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_U64_T:return TypeEn::unknown_jty;
    case RPMTypesEn::PRM_TYPE_I08_T:return TypeEn::int8_jty;
    case RPMTypesEn::PRM_TYPE_I16_T:return TypeEn::int16_jty;
    case RPMTypesEn::PRM_TYPE_I32_T:return TypeEn::int32_jty;
    case RPMTypesEn::PRM_TYPE_I64_T:return TypeEn::int64_jty;
    case RPMTypesEn::PRM_TYPE_F32_T:return TypeEn::float_jty;
    case RPMTypesEn::PRM_TYPE_F64_T:return TypeEn::double_jty;
    default:return TypeEn::unknown_jty;
    }
}

std::string toString(RPMTypesEn arg) {
#define CASE_OP(NAME) case RPMTypesEn::NAME:return #NAME;
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




bool fromJSON(const json::Value &DataFragment, DataInterval & dataInterval) {

    json::ObjectMapper O(DataFragment);
    bool ret= true;

    ret&=O.map("Frequency", dataInterval.frequency);
    ret&=O.map("Offset", dataInterval.offs);
    ret&=O.map("Size", dataInterval.size);
    ret&=O.map("Type", dataInterval.int_type_represintation);

    ret&=O.map("sTime.Begin", dataInterval.time_interval.bgn);
    ret&=O.map("sTime.End", dataInterval.time_interval.end);

    return ret;
}


DataInterval getDataInterval(json::Value &DataFragment, json::Array &DataFilesList) {

    DataInterval dataInterval;
    fromJSON(DataFragment, dataInterval);

    int64_t dataIndex;
    json::ObjectMapper O(DataFragment);
    O.map("Index", dataIndex);

    for (auto i : DataFilesList) {
        json::ObjectMapper O(i);
        int64_t fileDataIndex;
        O.map("Index", fileDataIndex);

        if (dataIndex == fileDataIndex) {
            O.map("Name", dataInterval.file_name);
            O.map("Local", dataInterval.local);
        }
    }

    return dataInterval;
}



void readParametersList(std::string databaseFName, std::vector<Parameter_IFS*>& parameterList) {
    std::ifstream ifs(databaseFName);
    std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );

    auto e =json::parse(content);
    json::Value &jValue=e.get();

    json::Object* jObject          =   jValue.getAsObject();
    json::Array*  parametersList   = (*jObject)["Parameters.List"].getAsArray();

    json::Object* parameter        = (*parametersList)[0].getAsObject();

    for (auto i : *parametersList) {
        json::Object   pObject=*i.getAsObject();

        json::Array & DataFilesList     = *(pObject["Data.Files.List"].getAsArray());
        json::Array & DataFragmentsList = *(pObject["Data.Fragments.List"].getAsArray());


        std::vector<DataInterval> interval_list;
        for (auto k : DataFragmentsList)
            interval_list.push_back(getDataInterval(k, DataFilesList));


        json::ObjectMapper O(i);
        std::string  name;
        TimeInterval time_interval;


        bool ret= true;
        ret &=O.map(" name", name);
        ret &=O.map("sTime.Begin", time_interval.bgn);
        ret &=O.map("sTime.End", time_interval.end);

        Parameter_IFS * parameter= new Parameter_IFS(name, time_interval, interval_list);

        parameterList.push_back(parameter);
    }
}



std::vector<Parameter_IFS*>  readParametersList(std::string databaseFName) {
    std::vector<Parameter_IFS *>  parameterInfoList;
    readParametersList(databaseFName, parameterInfoList);
    return parameterInfoList;
}