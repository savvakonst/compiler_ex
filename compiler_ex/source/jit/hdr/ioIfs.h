#ifndef IOIFS_H
#define IOIFS_H
#include <string>
#include <algorithm>
#include <vector>


void print_IR_error(const std::string &content);

typedef struct {
    double                       bgn=0.0;
    double                       end=0.0;
    int64_t                      offs=0;
} TimeInterval;

typedef struct {
    uint32_t                    type;
    int64_t                     offs;
    int64_t                     size;
    double                      frequency;
    std::string                 fileName;
    TimeInterval                timeInterval;
    bool                        local;
} DataInterval;


typedef struct {
    double                      frequency;
    TimeInterval                timeInterval;
    int64_t                     virtualSize;
}ExtendedInfo;


typedef struct {
    std::string                 parameterName;
    std::vector<DataInterval>   intervalList;
    void *                      extendedInfo;
} ParameterInfo;


class Parameters
{
public:



    Parameters(ParameterInfo &parameterInfo, std::string code){
        inputParameters.push_back(parameterInfo);
        calcExtendedInfo(inputParameters[0]);
    }

    Parameters(ParameterInfo* parameterInfo, std::string code){
        inputParameters.push_back(*parameterInfo);
        calcExtendedInfo( *parameterInfo);
    }

    Parameters(std::vector<ParameterInfo> &parameterInfoList, std::string code){
        inputParameters=parameterInfoList;
        for (auto &i : inputParameters)
            calcExtendedInfo(i);
    }

    ~Parameters(){
        for (auto &i : inputParameters)  delete i.extendedInfo;
        for (auto &i : outputParameters) delete i.extendedInfo;
    }

    std::vector<ParameterInfo> &getOutputParameters() {

    }

    int64_t read(void *bufferPtr,int64_t pos, int64_t size){
        
    }


private:

    bool calcExtendedInfo(ParameterInfo &arg) {
        auto extendedInfo = new ExtendedInfo;
        arg.extendedInfo=extendedInfo;

        if(arg.intervalList.size()){ 
            extendedInfo->timeInterval.bgn=arg.intervalList.front().timeInterval.bgn;
            extendedInfo->timeInterval.end=arg.intervalList.back().timeInterval.end;
            extendedInfo->frequency=arg.intervalList.front().frequency;
        }

        for (auto i : arg.intervalList)
            if (extendedInfo->frequency != i.frequency)
                print_IR_error("calcExtendedInfo - different frequencys");

        extendedInfo->virtualSize=(int64_t)(extendedInfo->frequency * (extendedInfo->timeInterval.end - extendedInfo->timeInterval.bgn));
    }

    std::vector<ParameterInfo> inputParameters;
    std::vector<ParameterInfo> outputParameters;

};



#endif