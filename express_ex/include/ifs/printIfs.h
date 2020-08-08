#ifndef PRINTIFS_H
#define PRINTIFS_H

#include "ioIfs.h"
#include "llvm/Support/raw_ostream.h"
//#include "llvm/Support/JSON.h"

llvm::raw_ostream &stream(llvm::raw_ostream &OS, const Parameter_IFS &di, std::string offset="");
llvm::raw_ostream &stream(llvm::raw_ostream &OS, const DataInterval &data_interval, std::string offset="");
//llvm::raw_ostream &stream(llvm::raw_ostream &OS, const ExtendedInfo &di, std::string offset="");

template<typename T>
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const std::vector<T> & arg) {
    for (auto i : arg)
        OS << i << " ";
    return OS;
}

template<typename Key_T, typename _T>
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const std::map<Key_T, _T> & arg) {
    for (auto i : arg)
        OS << i.first << ": " << i.second << " ";
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
    if (((int)arg) <= ((int)ExColors::RESET))
        if (g_ansiEscapeCodes) {
            if (arg == ExColors::RESET)
                OS << "\u001b[0m";
            else
                OS << "\u001b[3" + std::to_string((int)arg) + "m";
        }
        else
            OS << (llvm::raw_ostream::Colors)(arg);
    else {
        if (arg == ExColors::AnsiRESET)
            OS << "\u001b[0m";
        else
            OS << "\u001b[3" + std::to_string((int)arg - (int)ExColors::AnsiBLACK) + "m";
    }
    return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Delimiter & arg) {
    OS.SetUnbuffered();

    OS << (ExColors)(arg) << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << (((int)arg <= (int)ExColors::RESET) ? ExColors::RESET : ExColors::AnsiRESET) << "\n";
    return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const RPMTypesEn & arg) {
    OS << toString(arg);
    return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Parameter_IFS & arg) {
    std::stringstream stream;
    arg.stream(stream);
    return OS << stream.str();
}
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const DataInterval & arg) {
    stream(OS, arg);
    return OS;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, ParametersDB_IFS & arg) {
    auto db_parameters_  =arg.getDBParameterList();
    for (auto i : db_parameters_)
        ::stream(OS, *i);
    return OS;
}


#endif //PRINTIFS_H