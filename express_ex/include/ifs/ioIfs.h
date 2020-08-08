#ifndef IOIFS_H
#define IOIFS_H
#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>      
#include "types_jty.h"


enum class RPMTypesEn : uint64_t{
    PRM_TYPE_U08            = 0x0001,   // 1
    PRM_TYPE_U16            = 0x0002,   // 2
    PRM_TYPE_U32            = 0x0004,   // 4
    PRM_TYPE_U64            = 0x0008,   // 8
    PRM_TYPE_I08            = 0x0011,   // 17
    PRM_TYPE_I16            = 0x0012,   // 18
    PRM_TYPE_I32            = 0x0014,   // 20
    PRM_TYPE_I64            = 0x0018,   // 24
    PRM_TYPE_F32            = 0x0024,   // 36
    PRM_TYPE_F64            = 0x0028,   // 40

    PRM_TYPE_U08_T          = 0x1001,   // 4097 (4096 + 1)
    PRM_TYPE_U16_T          = 0x1002,   // 4098 (4096 + 2)
    PRM_TYPE_U32_T          = 0x1004,   // 4100 (4096 + 4)
    PRM_TYPE_U64_T          = 0x1008,   // 4104 (4096 + 8)
    PRM_TYPE_I08_T          = 0x1011,   // 4113 (4096 + 17)
    PRM_TYPE_I16_T          = 0x1012,   // 4114 (4096 + 18)
    PRM_TYPE_I32_T          = 0x1014,   // 4116 (4096 + 20)
    PRM_TYPE_I64_T          = 0x1018,   // 4120 (4096 + 24)
    PRM_TYPE_F32_T          = 0x1024,   // 4132 (4096 + 36)
    PRM_TYPE_F64_T          = 0x1028,   // 4136 (4096 + 40)

    PRM_TYPE_UNKNOWN        = 0xffff    // 4136 (4096 + 40)
};


typedef struct {
    double                       bgn=0.0;
    double                       end=0.0;
    int64_t                      offs=0;
} TimeInterval;

typedef struct {
    union {
        int64_t     int_type_represintation;
        RPMTypesEn  type;
    };
    int64_t                     offs;
    int64_t                     size;
    double                      frequency;
    TimeInterval                time_interval;
    std::string                 file_name;
    bool                        local;
} DataInterval;


inline int64_t sizeOfTy(RPMTypesEn  arg) {
    return ((int64_t)arg) & 0xf;
}
TypeEn      RPMType2JITType(RPMTypesEn arg);
std::string toString(RPMTypesEn arg);



inline std::stringstream &stream(std::stringstream & OS
    , const DataInterval & di, std::string offset) {

    OS << offset << "DataInterval{\n";
    OS << offset << "  type: " << toString(di.type) << "\n";
    OS << offset << "  offs: " << di.offs << "\n";
    OS << offset << "  size: " << di.size << "\n";
    OS << offset << "  frequency: " << di.frequency << "\n";
    OS << offset << "  time_interval.end: " << di.time_interval.end << "\n";
    OS << offset << "  time_interval.bgn: " << di.time_interval.bgn << "\n";
    OS << offset << "  file_name: " << di.file_name << "\n";
    OS << offset << "}\n";
    return OS;
}

inline bool isEmpty(TimeInterval i) {
    return i.bgn <= i.end;
}




inline TimeInterval operator& (DataInterval a, DataInterval b) {
    TimeInterval ret ={ std::max(a.time_interval.bgn, a.time_interval.bgn)  , std::min(a.time_interval.end, a.time_interval.end) };
    return ret;
}

inline TimeInterval operator|| (DataInterval a, DataInterval b) {
    auto c=a & b;
    if (c.bgn < c.end )
        return { std::min(a.time_interval.bgn, a.time_interval.bgn) , std::max(a.time_interval.end, a.time_interval.end) };
    return {0.0};
}



/*
typedef struct {
    int64_t                     virtual_size;
    int64_t                     frequency;
    TimeInterval                time_interval;
    TypeEn                      jit_type=TypeEn::unknown_jty;
}ExtendedInfo;

typedef struct {
    std::string                 parameter_name = "";
    TimeInterval                time_interval;
    std::vector<DataInterval>   interval_list=std::vector<DataInterval>();                
    ExtendedInfo *              extended_info = NULL;
} ParameterInfo;
*/


class Parameter_IFS {
public:

    Parameter_IFS() {
    }

    Parameter_IFS(std::string name, TimeInterval time_interval, std::vector<DataInterval> &interval_list) {
        name_=name;
        interval_list_ = interval_list;
        time_interval_ = time_interval;
        calcExtendedInfo();
    }


    ~Parameter_IFS() {

    }

    void addIntrval(const  DataInterval &interval) {
        interval_list_.push_back(interval);
    }


    typedef struct {
        double	val_max;
        double  val_min;
        double  time;
    } Dot;


    std::vector<int64_t>  read_dots( Dot * dot_buffer, size_t max_point_number, double from, double to) {
        //std::ifstream
        return {-1};
    }
    
    std::vector<int64_t>  read_dots(
        double * top_buffer_ptr, double * bottom_buffer_ptr, double * time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) {
        return {-1};
    }

    bool open() {
        if (is_opened_)
            return false;

        seek(0);
        is_opened_=true;
        return is_opened_;
    }


    bool close() {
        if (is_opened_) {
            if (ifs_) ifs_->close();
            is_opened_=false;
            return true;
        }
        return false;
    }

    bool seek(int64_t point_umber) {

        point_number_ = point_umber;
        const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
        current_interval_index_ = getDataIntervalIndex(current_time);
        
        if (current_interval_index_!=-1) {
            openNewInterval(current_interval_index_);
            int64_t local_start_pos =    (int)((current_time - getTimeInterval(current_interval_index_).bgn) * frequency_);
            ifs_->seekg(local_start_pos * sizeof_data_type_);
        }
        return true;
    }

    int64_t write(char * data_buffer_ptr, int64_t point_number) {
        return { 0 };
    }

    int64_t read(char * data_buffer_ptr, int64_t point_number) {
        if (!is_opened_)
            open();

        int64_t points_to_read = point_number;
        char*   ptr=data_buffer_ptr;
        //std::vector<DataInterval> &interval_list = parameter_info_.interval_list;

        
        while (points_to_read) {
            const double current_time = time_interval_.bgn + ((double)point_number_) / frequency_;
            int64_t di_index = getDataIntervalIndex(current_time);
            int64_t local_points_to_read;

            if (di_index == - 1) {
 
                double end_time;
                bool is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);
                if (is_last_interval)
                    end_time = time_interval_.end;
                else 
                    end_time = getTimeInterval(current_interval_index_+1).bgn;

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
                local_points_to_read =  (int)((di.time_interval.end - current_time) * frequency_);

                if (di_index != current_interval_index_) {
                    openNewInterval(di_index);
                    ifs_->seekg(local_start_pos * sizeof_data_type_);
                }

                if (points_to_read <= local_points_to_read)
                    local_points_to_read = points_to_read;

                ifs_->read(ptr, local_points_to_read * sizeof_data_type_);
            }

            ptr +=            local_points_to_read * sizeof_data_type_;
            points_to_read -= local_points_to_read;
        }
        
        return point_number - points_to_read;
    }


    std::string getName(){
        return name_;
    }

    RPMTypesEn getRPMType() {
        return interval_list_[0].type;
    }

    int64_t getVirtualSize() {
        return frequency_ * (time_interval_.end - time_interval_.bgn);
    }    

    friend  void readParametersList(std::string databaseFName, std::vector<Parameter_IFS>& parameterList);

    std::stringstream &stream(std::stringstream & OS,  std::string offset="") const {
        OS << offset << "ParameterInfo{\n";
        OS << offset << "  parameter_name: " << name_ << "\n";
        OS << offset << "  interval_list: [" << "\n";
        for (auto interval : interval_list_)
            ::stream(OS, interval, "    ");
        OS << offset << "  ]\n";
        OS << offset << "}\n";
        return OS;
    }

    void setDirectory(std::string dirname) {
        work_directory = dirname;
    }

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
        for (int64_t i =current_interval_index_; i< numer_of_intervals_; i++) {
            DataInterval &a =interval_list_[i];
            if ((a.time_interval.bgn <= time) && (time < a.time_interval.end)) return i;
        }
        return -1;
    }

    inline void openNewInterval(double di_index) {
        current_interval_index_=di_index;
        ifs_->close();
        delete ifs_;
        ifs_ =new std::ifstream(getCufrrentInterval().file_name, std::ios::binary);

    }



    friend class ParametersDB_IFS;

    std::ifstream*              ifs_  = NULL;

    std::string                 name_ = std::string();
    TimeInterval                time_interval_;
    std::vector<DataInterval>   interval_list_;
    //ExtendedInfo *              extended_info = NULL;


    double         frequency_=-1;

    int64_t        current_interval_index_=0;
    int64_t        numer_of_intervals_=0;

    //ParameterInfo  parameter_info_;

    int64_t        sizeof_data_type_=0;
    int64_t        point_number_=0;

    bool           is_opened_=true;

    

    std::string work_directory = "";
    std::string error_info_ = "";
};


void                        readParametersList(std::string database_Fame, std::vector<Parameter_IFS* >& parameter_list);
std::vector<Parameter_IFS*> readParametersList(std::string database_Fame);

class ParametersDB_IFS
{
public:

    ParametersDB_IFS() {

    }

    ParametersDB_IFS(Parameter_IFS* parameterInfo, std::string code) {
        db_parameters_.push_back(parameterInfo);
        calcExtendedInfo(parameterInfo);
    }

    ParametersDB_IFS(std::vector<Parameter_IFS *> &parameterInfoList, std::string code) {
        db_parameters_=parameterInfoList;
        for (auto &i : db_parameters_)
            calcExtendedInfo(i);
    }

    ParametersDB_IFS(const std::vector<std::string> &file_name_list) {
        addParametersSet(file_name_list);
    }

    ~ParametersDB_IFS() {
        for (auto &i : db_parameters_)
            delete i;
        for (auto &i : output_parameters_)
            delete i;
    }

    std::vector<Parameter_IFS *>&   getOutputParameters() {
        std::vector<Parameter_IFS*> readParametersList(std::string database_Fame);
    }

    bool addParametersSet(const std::string &file_name) {
        readParametersList(file_name, db_parameters_);
    }

    bool addParametersSet(const std::vector<std::string> &file_name_list) {
        for(auto file_name : file_name_list)
            readParametersList(file_name, db_parameters_);
    }

    const std::vector<std::string>  getNamesList() {
        std::vector<std::string> namesList;
        for (auto i : db_parameters_)
            namesList.push_back(i->getName());
        return namesList;
    }


    const std::vector<Parameter_IFS*>& getDBParameterList() const  {
        return db_parameters_;
    }

    inline Parameter_IFS* operator[] (std::string name) {
        for (auto &i : db_parameters_)
            if (i->getName() == name)
                return i;
        return NULL;
    }

    bool createSyncParameter(std::string name, std::vector<Parameter_IFS*> prototype_parameter_list, RPMTypesEn target_ty=RPMTypesEn::PRM_TYPE_UNKNOWN) {

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

    DataInterval createInterval(TimeInterval time_interval ,double frequency, RPMTypesEn target_ty ,const std::string &filename="" ,bool local = true){
        DataInterval interval;

        interval.type=target_ty;
        interval.offs=0;
        interval.size=sizeOfTy(target_ty)*(int64_t)((time_interval.end - time_interval.bgn) * frequency);
        interval.frequency=frequency;
        interval.time_interval=time_interval;

        interval.file_name=filename;
        interval.local=local;

        return interval;
    }

    bool intersection(std::vector<DataInterval> &ret,Parameter_IFS* parameter_a, Parameter_IFS* parameter_b, const std::string &name="", RPMTypesEn target_ty = RPMTypesEn::PRM_TYPE_UNKNOWN) {
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


    bool calcExtendedInfo(Parameter_IFS * arg) {
        return true;
    }

    /// dbParameters contains all parametersInfo from 
    /// all available data sets
    /// 
    std::vector<Parameter_IFS*> db_parameters_;

    std::vector<Parameter_IFS*> output_parameters_;
    std::vector<Parameter_IFS*> input_parameters_ifs_;

    std::string error_info_ = "";
};





#endif