#ifndef SYNC_PARAMETER_H
#define SYNC_PARAMETER_H

#include <string>
#include <vector>

#include <sstream>
#include <iostream>     
#include <fstream>  

#include "ParameterIfs.h"






class DLL_EXPORT SyncParameter: public ParameterIfs{
public:

    SyncParameter(){}
    SyncParameter(std::string name, const std::vector<DataInterval>& interval_list, bool save_fnames);
    SyncParameter(std::string name, const TimeInterval& time_interval,
                  const std::vector<DataInterval>& interval_list, bool save_fnames=true);

    ~SyncParameter();


    bool isAsync(){return false;}
    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(Dot* dot_buffer, size_t max_point_number, double from, double to) override;;

    // is not supported yet 
    virtual std::vector<int64_t>  read_dots(
        double* top_buffer_ptr, double* bottom_buffer_ptr, double* time_buffer_ptr,
        double from, double to, size_t max_point_number_to_read) override;;


    bool open(bool open_to_write = false) override;
    bool close() override;
    bool seek(int64_t point_umber);
    uint64_t write(char* data_buffer_ptr, uint64_t point_number) override;
    uint64_t read(char* data_buffer_ptr, uint64_t point_number) override;

    const uint64_t getVirtualSize() override;

    ParameterIfs* intersection(ParameterIfs* b, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="")override;
    SyncParameter* enlargeFrequency(int64_t arg, PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="");
    ParameterIfs* retyping(PRMTypesEn target_ty = PRMTypesEn::PRM_TYPE_UNKNOWN, const std::string& name="") override;

    virtual ParameterIfs* newParameter()override;

    friend void readParametersList(std::string databaseFName, std::vector<ParameterIfs>& parameterList);
    friend class ParametersDB;


protected:

    bool calcExtendedInfo();

    inline const TimeInterval& getTimeInterval(int64_t nterval_index){
        return interval_list_[(size_t)nterval_index].time_interval;
    }

    inline const DataInterval& getCufrrentInterval(){
        return interval_list_[(size_t)current_interval_index_];
    }



    void openNewInterval(double di_index);

    BareChunk* chunk_;
    BareChunk* current_chunk_;
    double  frequency_  = -1; 
};


#endif