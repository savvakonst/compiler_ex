#ifndef IOIFS_H
#define IOIFS_H
#include <string>
#include <algorithm>
#include <vector>
#include "types_jty.h"
#include "llvm/Support/JSON.h"



using namespace llvm;
void print_IR_error(const std::string &content);

// Sync
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
    void*       ptr =NULL;
    ex_size_t   length = 0;
    ex_size_t   leftOffset = 0;
    ex_size_t   rightOffset = 0;
    TypeEn      type = TypeEn::unknown_jty;
    std::string name = std::string();
}BufferInfo;

typedef struct {
    double                       bgn=0.0;
    double                       end=0.0;
    int64_t                      offs=0;
} TimeInterval;

typedef struct {
    union {
        int64_t     intRepresintationType;
        RPMTypesEn  type;
    };

    int64_t                     offs;
    int64_t                     size;
    double                      frequency;
    TimeInterval                timeInterval;
    std::string                 fileName;
    bool                        local;
} DataInterval;

typedef struct {
    int64_t                     virtualSize;
    int64_t                     frequency;
    TimeInterval                timeInterval;
    TypeEn                      jitType=TypeEn::unknown_jty;
}ExtendedInfo;

typedef struct {
    std::string                 parameterName = "";
    std::vector<DataInterval>   intervalList=std::vector<DataInterval>();
    ExtendedInfo *              extendedInfo = NULL;
} ParameterInfo;




TypeEn      RPMType2JITType(RPMTypesEn arg);
std::string toString(RPMTypesEn arg);


llvm::raw_ostream &stream(llvm::raw_ostream &OS, const ParameterInfo & di, std::string offset="");
llvm::raw_ostream &stream(llvm::raw_ostream &OS, const DataInterval & dataInterval, std::string offset="");
llvm::raw_ostream &stream(llvm::raw_ostream &OS, const ExtendedInfo & di, std::string offset="");


bool         fromJSON(const json::Value &DataFragment, DataInterval & dataInterval);
DataInterval getDataInterval(json::Value &DataFragment, json::Array &DataFilesList);

void                       readParametersList(std::string databaseFName, std::vector<ParameterInfo>& parameterInfoList);
std::vector<ParameterInfo> readParametersList(std::string databaseFName);

class Parameter_IFS {
    Parameter_IFS() {

    }

    int64_t readSync(void * dataBufferPtr, size_t pointsNumber) {
        return 0;
    }

    int64_t readAsync(void * data_buffer_ptr, void * time_buffer_ptr, size_t size) {
        return 0;
    }

    BufferInfo     bufferInfo;
    ParameterInfo  parameterInfo;
};


class ParametersDB_IFS
{
public:
    ParametersDB_IFS(ParameterInfo &parameterInfo, std::string code){
        dbParameters_.push_back(parameterInfo);
        calcExtendedInfo(dbParameters_[0]);
    }

    ParametersDB_IFS(ParameterInfo* parameterInfo, std::string code){
        dbParameters_.push_back(*parameterInfo);
        calcExtendedInfo( *parameterInfo);
    }

    ParametersDB_IFS(std::vector<ParameterInfo> &parameterInfoList, std::string code){
        dbParameters_=parameterInfoList;
        for (auto &i : dbParameters_)
            calcExtendedInfo(i);
    }

    ~ParametersDB_IFS(){
        for (auto &i : dbParameters_)  
            delete i.extendedInfo;
        for (auto &i : outputParameters_) 
            delete i.extendedInfo;
    }

    std::vector<ParameterInfo> &getOutputParameters() {

    }

    void setInputParameters(std::vector<BufferInfo> args) {
        for (auto i : args)
            inputParameters_.push_back(operator[](i.name));
    }


    int64_t read(void *bufferPtr,int64_t pos, int64_t size){
        return 0;
    }

    const std::vector<std::string> getNamesList() {
        std::vector<std::string> namesList;
        for (auto i : dbParameters_)
            namesList.push_back(i.parameterName);
        return namesList;
    }

    llvm::raw_ostream &stream(llvm::raw_ostream &OS,  std::string offset="") {
        for (auto i : dbParameters_)
            ::stream(OS,i);
        return OS;
    }

    inline ParameterInfo operator[] (std::string name) {
        for (auto &i : dbParameters_)
            if (i.parameterName == name)
                return ParameterInfo(i);
        return ParameterInfo();
    }

private:
    void calcExtendedInfo(ParameterInfo &arg) {
        auto extendedInfo = new ExtendedInfo;
        arg.extendedInfo=extendedInfo;

        if(arg.intervalList.size()){ 
            extendedInfo->timeInterval.bgn=arg.intervalList.front().timeInterval.bgn;
            extendedInfo->timeInterval.end=arg.intervalList.back().timeInterval.end;
            extendedInfo->frequency=arg.intervalList.front().frequency;
            extendedInfo->jitType = RPMType2JITType(arg.intervalList.front().type);
        }

        for (auto i : arg.intervalList)
            if (extendedInfo->frequency != i.frequency)
                print_IR_error("calcExtendedInfo - different frequencys");


        extendedInfo->virtualSize=(int64_t)(extendedInfo->frequency * (extendedInfo->timeInterval.end - extendedInfo->timeInterval.bgn));
    }

    void initInputParameters(std::vector<BufferInfo> args) {
        //for (auto &i : inputParameters)
          //  i.
    }

    /// dbParameters contains all parametersInfo from 
    /// all available data sets
    std::vector<ParameterInfo> dbParameters_;


    std::vector<ParameterInfo> inputParameters_;
    std::vector<ParameterInfo> outputParameters_;
};


inline bool isEmpty(ParameterInfo & arg) {
    return arg.intervalList.size() == 0;
}

template<typename T>
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const std::vector<T> & arg) {
    for (auto i : arg)
        OS << i << " ";
    return OS;
}

template<typename Key_T, typename _T>
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const std::map<Key_T, _T> & arg) {
    for (auto i : arg)
        OS << i.first <<": " <<i.second<<" ";
    return OS;
}

enum class Delimiter {
    BLACK = 0,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    SAVEDCOLOR,


    AnsiBLACK = 32,
    AnsiRED,
    AnsiGREEN,
    AnsiYELLOW,
    AnsiBLUE,
    AnsiMAGENTA,
    AnsiCYAN,
    AnsiWHITE
};

enum class ExColors {
    BLACK = 0,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    SAVEDCOLOR,
    RESET,

    AnsiBLACK = 32,
    AnsiRED,
    AnsiGREEN,
    AnsiYELLOW,
    AnsiBLUE,
    AnsiMAGENTA,
    AnsiCYAN,
    AnsiWHITE,
    AnsiSAVEDCOLOR,
    AnsiRESET
};

extern bool g_ansiEscapeCodes;
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const ExColors & arg) {
    if(((int)arg)<=((int)ExColors::RESET))
        if (g_ansiEscapeCodes) {
            if (arg == ExColors::RESET)
                OS << "\u001b[0m";
            else
                OS << "\u001b[3" + std::to_string((int)arg ) + "m";
        }
        else
            OS << (llvm::raw_ostream::Colors)(arg);
    else {
        if (arg == ExColors::AnsiRESET)
            OS << "\u001b[0m";
        else
            OS << "\u001b[3"+ std::to_string(   (int)arg - (int)ExColors::AnsiBLACK )  +"m";
    }
    return OS;
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Delimiter & arg) {
    OS.SetUnbuffered();
    
    OS <<(ExColors)(arg)<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << (((int)arg <= (int)ExColors::RESET) ? ExColors::RESET: ExColors::AnsiRESET )<<"\n";
    return OS;
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const RPMTypesEn & arg) {
    OS << toString(arg);
    return OS;
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const ParameterInfo & arg) {
    stream(OS, arg);
    return OS;
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const DataInterval & arg) {
    stream(OS, arg);
    return OS;
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, ParametersDB_IFS & arg) {
    arg.stream(OS);
    return OS;
}

#endif