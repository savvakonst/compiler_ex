#include <type_traits>
#include <set>
#include "ParameterIO.h"
#include "common.h"
#include "llvm/Support/JSON.h"


using namespace llvm;



bool         fromJSON(const json::Value &data_fragment, DataInterval &data_interval);
DataInterval getDataInterval(json::Value &data_fragment, json::Array &data_files_list);

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



bool readParametersList(std::string databaseFName, std::vector<SyncParameter*>& parameterList) {
    std::ifstream ifs(databaseFName);
    if (ifs.bad())
        return false;
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
        ret &=O.map(" Name", name);
        ret &=O.map("sTime.Begin", time_interval.bgn);
        ret &=O.map("sTime.End", time_interval.end);

        SyncParameter * parameter= new SyncParameter(name, time_interval, interval_list);

        parameterList.push_back(parameter);
    }
    return true;
}

std::vector<SyncParameter*>  readParametersList(std::string databaseFName) {
    std::vector<SyncParameter *>  parameterInfoList;
    readParametersList(databaseFName, parameterInfoList);
    return parameterInfoList;
}


SyncParameter::SyncParameter(
    std::string name, 
    const std::vector<DataInterval> &interval_list,
    bool save_fnames
) 
{
    name_=name;
    //for (auto i : interval_list)
    //    interval_list_.push_back(i);

    interval_list_ = interval_list;
    numer_of_intervals_ = interval_list.size();
    if (save_fnames == false) {
        for (auto &i : interval_list_) {
            i.file_name = "";
            i.local = true;
        }
    }

    if (interval_list_.size()) {
        sizeof_data_type_=sizeOfTy(interval_list_.front().type);
        type_ = interval_list_.front().type;
        calcMinMaxPtr = g_calcMinMax_select(type_);
    }
    

    auto bgn=interval_list_.front().time_interval.bgn;
    auto end=interval_list_.back().time_interval.end;

    time_interval_ ={  bgn, end };
    calcExtendedInfo();
}


SyncParameter::SyncParameter(
    std::string name,
    const TimeInterval &time_interval,
    const std::vector<DataInterval> &interval_list,
    bool save_fnames
):SyncParameter(name, interval_list, save_fnames)
{
    time_interval_ = time_interval;
    calcExtendedInfo();
}



bool SyncParameter::seek(int64_t point_umber) {

    point_number_ = point_umber;
    const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
    current_interval_index_ = getDataIntervalIndex(current_time);

    if (current_interval_index_ != -1) {
        openNewInterval(current_interval_index_);
        int64_t local_start_pos =    (int)((current_time - getTimeInterval(current_interval_index_).bgn) * frequency_);
        ifs_->seekg(local_start_pos * sizeof_data_type_);
    }
    return true;
}



int64_t SyncParameter::write(char * data_buffer_ptr, int64_t point_number) {
    if (!opened_to_read_)
        open();

    int64_t points_to_write = point_number;
    char*   ptr = data_buffer_ptr;
    //std::vector<DataInterval> &interval_list = parameter_info_.interval_list;


    while (points_to_write) {
        const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
        
        int64_t di_index = getDataIntervalIndex(current_time);
        int64_t local_points_to_write;

        if (di_index == -1) {

            double end_time;
            bool   is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);

            if (is_last_interval)
                end_time = time_interval_.end + additional_time_;
            else
                end_time = getTimeInterval(current_interval_index_ + 1).bgn;

            local_points_to_write = (int)((end_time - current_time) * frequency_);

            if (points_to_write <= local_points_to_write)
                local_points_to_write = points_to_write;
            else if (is_last_interval) {
                return point_number - points_to_write + local_points_to_write;
            }

        }
        else {
            DataInterval &di = interval_list_[di_index];

            int64_t local_start_pos = (int)((current_time - di.time_interval.bgn) * frequency_);
            local_points_to_write = (int)((di.time_interval.end + additional_time_ - current_time) * frequency_);

            bool is_new_interval = (di_index != current_interval_index_) || (ifs_ == nullptr);
            if (is_new_interval ) {
                openNewInterval(di_index);
                //ifs_->seekg(local_start_pos * sizeof_data_type_);
            }

            if (points_to_write <= local_points_to_write)
                local_points_to_write = points_to_write;
            
            if (calcMinMaxPtr)
                calcMinMaxPtr(ptr, local_points_to_write , di.val_max, di.val_min, is_new_interval);

            ifs_->write(ptr, local_points_to_write * sizeof_data_type_);
        }

        ptr += local_points_to_write * sizeof_data_type_;
        points_to_write -= local_points_to_write;
        point_number_ += local_points_to_write;
    }

    return point_number - points_to_write;
}

int64_t SyncParameter::read(char * data_buffer_ptr, int64_t point_number) {
    if ( !opened_to_read_ )
        open();

    int64_t   points_to_read = point_number;
    uint8_t*  ptr = (uint8_t * )data_buffer_ptr;
    //std::vector<DataInterval> &interval_list = parameter_info_.interval_list;


    while (points_to_read) {
        const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
        
        int64_t di_index = getDataIntervalIndex(current_time);
        int64_t local_points_to_read;

        if (di_index == -1) {

            double end_time;
            bool   is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);
            if (is_last_interval)
                end_time = time_interval_.end + additional_time_;
            else
                end_time = getTimeInterval(current_interval_index_ + 1).bgn;

            local_points_to_read = (int)((end_time - current_time) * frequency_);

            if (points_to_read <= local_points_to_read)
                local_points_to_read = points_to_read;
            else if (is_last_interval) {
                memset(ptr, 0, local_points_to_read * sizeof_data_type_);
                return point_number - points_to_read + local_points_to_read;
            }

            memset(ptr, 0, local_points_to_read * sizeof_data_type_);

        }
        else {
            const DataInterval &di = interval_list_[di_index];

            int64_t local_start_pos =       (int)((current_time - di.time_interval.bgn) * frequency_);
            local_points_to_read =  (int)((di.time_interval.end + additional_time_ - current_time) * frequency_);

            if (di_index != current_interval_index_ || (ifs_ == nullptr)) {
                openNewInterval(di_index);
                ifs_->seekp(local_start_pos * sizeof_data_type_);
            }

            if (points_to_read <= local_points_to_read)
                local_points_to_read = points_to_read;

            ifs_->read((char*)ptr, local_points_to_read * sizeof_data_type_);
        }

        ptr += local_points_to_read * sizeof_data_type_;
        points_to_read -= local_points_to_read;
        point_number_ += local_points_to_read;
    }
    
    return point_number - points_to_read;
}


SyncParameter *  SyncParameter::intersection(SyncParameter  *b, PRMTypesEn target_ty, const std::string &name) {
    auto parameter_a = this;
    auto parameter_b = b;

    if (parameter_a == parameter_b)
        return this;

    if (parameter_a->frequency_ != parameter_b->frequency_) {
        error_info_ = "different frequencys is not supported yet ";
        return nullptr;
    }

    else if (parameter_a->frequency_ <= 0.0) {
        error_info_ = "async parameter is not supported yet";
        return nullptr;
    }

    auto frequency = parameter_a->frequency_;
    std::vector<DataInterval> ret;

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
    return new SyncParameter(name, time_interval_, ret);
}




SyncParameter *  SyncParameter::enlargeFrequency(int64_t arg, PRMTypesEn target_ty , const std::string &name ) {
    auto parameter_a=this;
    SyncParameter* ret= this;
    std::vector<DataInterval> data_interval;
   
    double frequency=.0;

    if (arg > 1)
        frequency = this->frequency_ * arg;
    else if (arg < -1) {
        if (((int)this->frequency_) % arg)
            return nullptr;
        frequency = this->frequency_ / arg;
    }
    else
        return this;

    for (auto a : parameter_a->interval_list_) {
        auto interval = createInterval(a.time_interval, frequency, target_ty);
        data_interval.push_back(interval);
    }

    return new SyncParameter(name, time_interval_, data_interval);
}

SyncParameter * SyncParameter::retyping(PRMTypesEn target_ty, const std::string & name)
{
    if (getRPMType() == target_ty)
        return this;

    std::vector<DataInterval> data_interval;
    for (auto a : interval_list_) {
        auto interval = createInterval({}, frequency_, target_ty);
        data_interval.push_back(interval);
    }
    return new SyncParameter(name, time_interval_, data_interval);
}


SyncParameter *  intersection(SyncParameter  *a,SyncParameter  *b, PRMTypesEn target_ty, const std::string &name) {
    if (a == nullptr)
        return b;
    if (b == nullptr)
        return a;
    return a->intersection(b, target_ty, name);

}

SyncParameter *  intersection(std::vector<SyncParameter*> arg_list, PRMTypesEn target_ty, const std::string &name) {
    std::set<SyncParameter*> parameterSet;
    SyncParameter* p=nullptr;
    for (auto i : arg_list) {
        p=intersection(p, i, target_ty, name);
        parameterSet.insert(p);
    }

    for (auto i : arg_list)
        parameterSet.erase(i);

    parameterSet.erase(nullptr);
    parameterSet.erase(p);

    for (auto i : parameterSet)
        delete i;

    return p;
}

SyncParameter *retyping(SyncParameter  *a,PRMTypesEn target_ty, const std::string & name)
{
    if (a == nullptr)
        return nullptr;
    return a->retyping(target_ty, name);
}