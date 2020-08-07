#ifndef IOIFS_H
#define IOIFS_H
#include <string>
//#include <algorithm>
#include <vector>

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
    PRM_TYPE_F64_T          = 0x1028    // 4136 (4096 + 40)
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


inline int64_t sizeOfTy(RPMTypesEn  arg) {
    return ((int64_t) arg) & 0xf;
}



TypeEn      RPMType2JITType(RPMTypesEn arg);
std::string toString(RPMTypesEn arg);




void                       readParametersList(std::string database_Fame, std::vector<ParameterInfo>& parameter_info_list);
std::vector<ParameterInfo> readParametersList(std::string database_Fame);


class Parameter_IFS {
public:

    Parameter_IFS(const ParameterInfo & parameter_info) {
        parameter_info_     = parameter_info;
        frequency_          = parameter_info_.extended_info->frequency;
        numer_of_intervals_ = parameter_info_.interval_list.size();
        sizeof_data_type_   = sizeOfTy(parameter_info_.interval_list.front().type);
    }

    ~Parameter_IFS() {

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
        const double current_time = parameter_info_.time_interval.bgn + ((double)point_number_) / frequency_;
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


        const double &frequency = parameter_info_.extended_info->frequency; 
        int64_t points_to_read = point_number;
        char*   ptr=data_buffer_ptr;
        std::vector<DataInterval> &interval_list = parameter_info_.interval_list;

        
        while (points_to_read) {
            const double current_time = parameter_info_.time_interval.bgn + ((double)point_number_) / frequency;
            int64_t di_index = getDataIntervalIndex(current_time);
            int64_t local_points_to_read;

            if (di_index == - 1) {
 
                double end_time;
                bool is_last_interval = current_interval_index_ >= (numer_of_intervals_ - 1);
                if (is_last_interval)
                    end_time = parameter_info_.time_interval.end;
                else 
                    end_time = getTimeInterval(current_interval_index_+1).bgn;

                local_points_to_read = (int)((end_time - current_time) * frequency);

                if (points_to_read <= local_points_to_read)
                    local_points_to_read = points_to_read;
                else if (is_last_interval) {
                    memset(ptr, 0, local_points_to_read * sizeof_data_type_);
                    return point_number - points_to_read + local_points_to_read;
                }

                memset(ptr, 0, local_points_to_read * sizeof_data_type_);

            }
            else {
                const DataInterval &di =parameter_info_.interval_list[di_index];

                int64_t local_start_pos =       (int)((current_time - di.time_interval.bgn) * frequency);
                local_points_to_read =  (int)((di.time_interval.end - current_time) * frequency);

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
        return parameter_info_.parameter_name;
    }

    RPMTypesEn getRPMType() {
        return parameter_info_.interval_list[0].type;
    }

    int64_t getVirtualSize() {
        return frequency_ * (parameter_info_.time_interval.end - parameter_info_.time_interval.bgn);
    }


protected:
    inline const TimeInterval &getTimeInterval(int64_t nterval_index) {
        return parameter_info_.interval_list[nterval_index].time_interval;
    }

    inline const DataInterval &getCufrrentInterval() {
        return parameter_info_.interval_list[current_interval_index_];
    }

    inline int64_t getDataIntervalIndex(double time) {
        for (int64_t i =current_interval_index_; i< numer_of_intervals_; i++) {
            DataInterval &a =parameter_info_.interval_list[i];
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


    double         frequency_=0.0;

    int64_t        current_interval_index_=0;
    int64_t        numer_of_intervals_=0;

    ParameterInfo  parameter_info_;

    int64_t        sizeof_data_type_=0;
    int64_t        point_number_=0;


    std::ifstream* ifs_=NULL;

    bool           is_opened_=true;
};


class ParametersDB_IFS
{

public:
    ParametersDB_IFS(ParameterInfo &parameterInfo, std::string code){
        db_parameters_.push_back(parameterInfo);
        calcExtendedInfo(db_parameters_[0]);
    }

    ParametersDB_IFS(ParameterInfo* parameterInfo, std::string code){
        db_parameters_.push_back(*parameterInfo);
        calcExtendedInfo( *parameterInfo);
    }

    ParametersDB_IFS(std::vector<ParameterInfo> &parameterInfoList, std::string code){
        db_parameters_=parameterInfoList;
        for (auto &i : db_parameters_)
            calcExtendedInfo(i);
    }

    ~ParametersDB_IFS(){
        for (auto &i : db_parameters_)  
            delete i.extended_info;
        for (auto &i : output_parameters_) 
            delete i.extended_info ;
    }

    std::vector<ParameterInfo> &getOutputParameters() {

    }

    typedef struct {
        void*       ptr =NULL;
        ex_size_t   length = 0;
        ex_size_t   left_offset = 0;
        ex_size_t   right_offset = 0;
        TypeEn      type = TypeEn::unknown_jty;
        std::string name = std::string();
    }BufferInfo;

    void setInputParameters(std::vector<BufferInfo> args) {
        for (auto i : args) {
            const ParameterInfo &p_info =operator[](i.name);
            input_parameters_ifs_.push_back(Parameter_IFS(p_info));
        }
    }

    int64_t read(void *bufferPtr,int64_t pos, int64_t size){
        return 0;
    }

    const std::vector<std::string> getNamesList() {
        std::vector<std::string> namesList;
        for (auto i : db_parameters_)
            namesList.push_back(i.parameter_name);
        return namesList;
    }


    const std::vector<ParameterInfo>  & getDBParameterList() const  {
        return db_parameters_;
    }
    /*
    llvm::raw_ostream &stream(llvm::raw_ostream &OS,  std::string offset="") {
        for (auto i : db_parameters_)
            ::stream(OS,i);
        return OS;
    }
    */
    inline ParameterInfo operator[] (std::string name) {
        for (auto &i : db_parameters_)
            if (i.parameter_name == name)
                return ParameterInfo(i);
        return ParameterInfo();
    }

private:
    bool calcExtendedInfo(ParameterInfo &arg) {
        auto extended_info = new ExtendedInfo;
        arg.extended_info=extended_info;

        if(arg.interval_list.size()){ 
            extended_info->time_interval.bgn=arg.interval_list.front().time_interval.bgn;
            extended_info->time_interval.end=arg.interval_list.back().time_interval.end;
            extended_info->frequency=arg.interval_list.front().frequency;
            extended_info->jit_type = RPMType2JITType(arg.interval_list.front().type);
        }

        for (auto i : arg.interval_list)
            if (extended_info->frequency != i.frequency) {
                error_info_ = "calcExtendedInfo - different frequencys";
                return false;
            }
                
        extended_info->virtual_size=(int64_t)(extended_info->frequency * (arg.time_interval.end - arg.time_interval.bgn));
        return true;
    }

    void initInputParameters(std::vector<BufferInfo> args) {
        //for (auto &i : inputParameters)
          //  i.
    }



    /// dbParameters contains all parametersInfo from 
    /// all available data sets
    std::vector<ParameterInfo> db_parameters_;

    std::vector<ParameterInfo> output_parameters_;

    std::vector<Parameter_IFS> input_parameters_ifs_;

    std::string error_info_ = "";
};


inline bool isEmpty(ParameterInfo & arg) {
    return arg.interval_list.size() == 0;
}



#endif