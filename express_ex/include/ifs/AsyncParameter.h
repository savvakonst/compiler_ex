#ifndef ASYNC_PARAMETER_H
#define ASYNC_PARAMETER_H

#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>  

#include "ParameterIfs.h"




class DLL_EXPORT AsyncParameter: public ParameterIfs{
public:
    AsyncParameter(){ parent_parameter_ = this; }
    AsyncParameter(std::string name, const std::vector<DataInterval>& interval_list, bool save_fnames);
    AsyncParameter(std::string name, const TimeInterval& time_interval,
                   const std::vector<DataInterval>& interval_list, bool save_fnames=true);



    ~AsyncParameter(){}


    bool isAsync(){ return true; }
    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(Dot* dot_buffer, size_t max_point_number, double from, double to) override{ return {-1}; };

    // is not supported yet 
    virtual std::vector<int64_t> read_dots(
        double* top_buffer_ptr, double* bottom_buffer_ptr, double* time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) override{
        return {-1};
    };

    bool open(bool open_to_write = false) override;
    bool close() override;
    uint64_t write(char* data_buffer_ptr, uint64_t point_number)override;
    uint64_t read(char* data_buffer_ptr, uint64_t point_number)override;

    PRMTypesEn  getRPMType(){ return interval_list_[0].type; }

    const size_t getVirtualSize() override {
        uint64_t total_size = 0;
        for(auto& interval : interval_list_)
            total_size = total_size + interval.size;
        total_size = total_size * ( sizeOfTy(this->getRPMType()) + PRM_TIME_SIZE_) ;
        return  total_size;
    }

    ParameterIfs* intersection(ParameterIfs* b, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="");
    ParameterIfs* enlargeFrequency(int64_t arg, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="");
    ParameterIfs* retyping(PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="")override;

    virtual ParameterIfs* newParameter()override;

    friend void readParametersList(std::string databaseFName, std::vector<ParameterIfs>& parameterList);
    friend class ParametersDB;


protected:
    const size_t PRM_TIME_SIZE_ = 4;




    void readFromBuffer(char* data_buffer_ptr, uint64_t points_to_read);
    void readRawData(uint64_t points_to_read);


    template<typename T>
    void copyToBuffer(T* data_buffer_ptr, uint64_t points_to_read){
        while(points_to_read){
            *((float*)time_buffer_.current_ptr) = *((float*)intermediate_buffer_.current_ptr);
            time_buffer_.current_ptr+=sizeof(float);
            intermediate_buffer_.current_ptr+=sizeof(float);
            *data_buffer_ptr=*((T*)intermediate_buffer_.current_ptr);
            intermediate_buffer_.current_ptr+=sizeof(T);
            points_to_read--;
        }
    }

    template<typename T>
    void copyFromBuffer(T* data_buffer_ptr, uint64_t points_to_read){
        //=parent_parameter_.time_buffer_
        IntermediateBuffer& parent_time_buffer_ = parent_parameter_.time_buffer_;
        while(points_to_read){
            *((float*)intermediate_buffer_.current_ptr)= *((float*)parent_time_buffer_.current_ptr);
            parent_time_buffer_.current_ptr+=sizeof(float);
            intermediate_buffer_.current_ptr+=sizeof(float);
            *((T*)intermediate_buffer_.current_ptr) = *data_buffer_ptr;
            intermediate_buffer_.current_ptr+=sizeof(T);
            points_to_read--;
        }
    }

    inline const TimeInterval& getTimeInterval(int64_t nterval_index){
        return interval_list_[(size_t)nterval_index].time_interval;
    }

    inline const DataInterval& getCufrrentInterval(){
        return interval_list_[(size_t)current_interval_index_];
    }

    inline int64_t getDataIntervalIndex(double time){
        // debug cond => { time == 134.00097656250000 }
        for(int64_t i = (current_interval_index_ < 0 ? 0 : current_interval_index_); i < numer_of_intervals_; i++){
            DataInterval& a =interval_list_[(size_t)i];
            if((a.time_interval.bgn <= time) && (time < (a.time_interval.end + additional_time_))) return i;
        }
        return -1;
    }

    inline void openNewInterval(){
        if(ifs_){
            ifs_->close();
            delete ifs_;
            ifs_=nullptr;
        }
        std::string file_name = (getCufrrentInterval().local ? work_directory_ + "/" : "") + getCufrrentInterval().file_name;
        if(opened_to_read_){
            ifs_ = new std::fstream(file_name, std::ios::in | std::ios::binary);
            bool op =ifs_->is_open();
        }
        else if(opened_to_write_)
            ifs_ =new std::fstream(file_name, std::ios::out | std::ios::binary);
        // std::ios::app |  std::ios::trunc
    }

    struct IntermediateBuffer{
        char* base_ptr;
        char* current_ptr;
        size_t size;
        IntermediateBuffer(): base_ptr(nullptr), current_ptr(nullptr), size(0){}
        ~IntermediateBuffer(){ clear();}

        inline void clear(){
            if(base_ptr)
                delete base_ptr;
            if(current_ptr)
                delete current_ptr;
            base_ptr = nullptr;
            current_ptr = nullptr;
        }

        inline void resize(size_t new_size){
            if(new_size> size){
                clear();
                size=new_size;
                base_ptr=new char[size];
                current_ptr=base_ptr;
            }
            else{
                clear();
            }
        }

        inline void replaceLastData(size_t size_to_replace){
            //char* source_ptr = base_ptr + size - size_to_replace;
            char* source_ptr = current_ptr - size_to_replace;
            std::memcpy(base_ptr, source_ptr, size_to_replace);
            current_ptr = base_ptr + size_to_replace;
        }
        inline void resetPos(){
            current_ptr = base_ptr;
        }
    };

    IntermediateBuffer intermediate_buffer_;
    IntermediateBuffer time_buffer_;

    PRMTypesEn type_    = PRMTypesEn::PRM_TYPE_UNKNOWN;
    size_t unused_points_in_current_interval_ = 0;
    size_t data_size_ = 0;
    size_t data_size_factor_ = 0;
    AsyncParameter *parent_parameter_ = nullptr;
};



#endif