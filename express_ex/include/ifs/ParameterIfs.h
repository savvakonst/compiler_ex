#ifndef PARAMETERIFS_H
#define PARAMETERIFS_H
#include <string>
#include <vector>


#include <algorithm >
#include <sstream>
#include <iostream>     
#include <fstream>      
#include "config.h"
 
#pragma warning( push )  
#pragma warning( disable : 4251)

enum class PRMTypesEn : uint64_t {
    PRM_TYPE_U08            = 0x0001, 
    PRM_TYPE_U16            = 0x0002,  
    PRM_TYPE_U32            = 0x0004,  
    PRM_TYPE_U64            = 0x0008,  
    PRM_TYPE_I08            = 0x0011,  
    PRM_TYPE_I16            = 0x0012,  
    PRM_TYPE_I32            = 0x0014,  
    PRM_TYPE_I64            = 0x0018,  
    PRM_TYPE_F32            = 0x0024, 
    PRM_TYPE_F64            = 0x0028, 

    PRM_TYPE_U08_T          = 0x1001, 
    PRM_TYPE_U16_T          = 0x1002,  
    PRM_TYPE_U32_T          = 0x1004,  
    PRM_TYPE_U64_T          = 0x1008,  
    PRM_TYPE_I08_T          = 0x1011,  
    PRM_TYPE_I16_T          = 0x1012,  
    PRM_TYPE_I32_T          = 0x1014,  
    PRM_TYPE_I64_T          = 0x1018,  
    PRM_TYPE_F32_T          = 0x1024,  
    PRM_TYPE_F64_T          = 0x1028,   

    PRM_TYPE_UNKNOWN        = 0xffff    
};


typedef struct {
    double          bgn = 0.0;
    double          end = 0.0;
    int64_t         offs = 0;
} TimeInterval;
 
typedef struct {
    union {
        int64_t     int_type_represintation;
        PRMTypesEn  type;
    };
    int64_t         offs;
    int64_t         size;
    double          frequency;
    double          val_max;
    double          val_min;
    TimeInterval    time_interval;
    std::string     file_name;
    bool            local;
} DataInterval;

 
typedef struct {
    double	        val_max;
    double          val_min;
    double          time;
} Dot;





inline size_t sizeOfTy(PRMTypesEn  arg) {
    return ((uint64_t)arg) & 0xf;
}

//TypeEn      PRMType2JITType(PRMTypesEn arg);
//PRMTypesEn  JITType2PRMType(TypeEn arg);

DLL_EXPORT
std::string toString(PRMTypesEn arg);




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
    return i.bgn >= i.end;
}


inline double stdmax (double a, double b) {
    return a > b ? a : b;
}
inline double stdmin (double a, double b) {
    return a < b ? a : b;
}

inline TimeInterval operator& (DataInterval a, DataInterval b) {
    TimeInterval ret ={ stdmax(a.time_interval.bgn, b.time_interval.bgn)  , stdmin(a.time_interval.end, b.time_interval.end) };
    return ret;
}

inline TimeInterval operator|| (DataInterval a, DataInterval b) {
    auto c=a & b;
    if (c.bgn <= c.end)
        return { stdmin(a.time_interval.bgn, b.time_interval.bgn) , stdmax(a.time_interval.end, b.time_interval.end) };
    return { 0.0 };
}


inline DataInterval createInterval(TimeInterval time_interval, double frequency, PRMTypesEn target_ty, const std::string &filename="", bool local = true) {
    DataInterval interval;

    interval.type=target_ty;
    interval.offs=0;
    interval.size=sizeOfTy(target_ty) * (int64_t)((time_interval.end - time_interval.bgn) * frequency);
    interval.frequency=frequency;
    interval.time_interval=time_interval;

    interval.file_name=filename;
    interval.local=local;

    return interval;
}

typedef void (*calcMinMaxTy)(char* carg, int64_t Number, double & dmax, double & dmin, bool init);



class DLL_EXPORT ParameterIfs {
public:


    const std::string& getName() const{ return name_; }
    const TimeInterval& getMainTimeInterval() const{ return time_interval_; }
    const std::vector<DataInterval>& getDataIntervalList() const{ return interval_list_; }
    virtual const PRMTypesEn getRPMType(){ return type_; }
    virtual const size_t getVirtualSize() = 0;


    virtual bool isAsync() = 0;

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(Dot * dot_buffer, size_t max_point_number, double from, double to) = 0;

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(
        double * top_buffer_ptr, double * bottom_buffer_ptr, double * time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) = 0;


    virtual bool open(bool open_to_write = false) = 0;
    virtual bool close() = 0;

    virtual uint64_t write(char* data_buffer_ptr, uint64_t point_number)= 0;
    virtual uint64_t read(char* data_buffer_ptr, uint64_t point_number)= 0;

    virtual ParameterIfs* intersection(ParameterIfs* b, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="")= 0;
    virtual ParameterIfs* retyping(PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="")= 0;

    virtual void setName(const std::string& name){
        name_=name;
        if(interval_list_.size() == 1)
            interval_list_.front().file_name = name + ".dat";

        int64_t index=0;
        for(auto i : interval_list_)
            i.file_name = name + "_" + std::to_string(index++) + ".dat";
    }


    void setPath(std::string dirname) { work_directory_ = dirname; }
    void setLocal(bool val = true) { for (auto &i : interval_list_) i.local=val; }
    void addIntrval(const  DataInterval& interval){
        interval_list_.push_back(interval);
    }

    virtual ParameterIfs* newParameter()=0;
    virtual std::stringstream& stream(std::stringstream& OS, std::string offset="") const{
        OS << offset << "ParameterInfo{\n";
        OS << offset << "  parameter_name: " << name_ << "\n";
        OS << offset << "  time_interval.end: " << time_interval_.end << "\n";
        OS << offset << "  time_interval.bgn: " << time_interval_.bgn << "\n";
        OS << offset << "  interval_list: [" << "\n";
        for(auto interval : interval_list_)
            ::stream(OS, interval, "    ");
        OS << offset << "  ]\n";
        OS << offset << "}\n";
        return OS;
    }

    friend class ParametersDB;

protected:

    calcMinMaxTy   calcMinMaxPtr = nullptr;

    std::fstream*  ifs_ = nullptr;

    std::string                 name_ = "";
    TimeInterval                time_interval_;
    std::vector<DataInterval>   interval_list_;

    const double additional_time_ = 0.0009765625;
    PRMTypesEn type_    = PRMTypesEn::PRM_TYPE_UNKNOWN;

    int64_t current_interval_index_  = 0;
    int64_t numer_of_intervals_      = 0;

    int64_t sizeof_data_type_        = 0;
    int64_t point_number_            = 0;

    bool    opened_to_read_          = false;
    bool    opened_to_write_         = false;



    std::string work_directory_ = "";
    std::string error_info_     = "";


};
#pragma warning( pop )  
#endif
