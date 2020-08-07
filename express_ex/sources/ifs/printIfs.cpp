#include "printIfs.h"

llvm::raw_ostream &stream(llvm::raw_ostream &OS, const ParameterInfo & di, std::string offset) {
    OS << offset << "ParameterInfo{\n";
    OS << offset << "  parameter_name: " << di.parameter_name << "\n";
    OS << offset << "  interval_list: [" << "\n";
    for (auto interval : di.interval_list)
        stream(OS, interval, "    ");
    OS << offset << "  ]\n";
    if (di.extended_info) {
        OS << offset << "  extended_info: " << "\n";
        stream(OS, *(di.extended_info), "    ");
    }
    else
        OS << offset << "  extended_info: " << di.extended_info << "\n";
    OS << offset << "}\n";
    return OS;
}

llvm::raw_ostream &stream(llvm::raw_ostream &OS, const DataInterval & di, std::string offset) {
    OS << offset << "DataInterval{\n";
    OS << offset << "  type: " << di.type << "\n";
    OS << offset << "  offs: " << di.offs << "\n";
    OS << offset << "  size: " << di.size << "\n";
    OS << offset << "  frequency: " << di.frequency << "\n";
    OS << offset << "  time_interval.end: " << di.time_interval.end << "\n";
    OS << offset << "  time_interval.bgn: " << di.time_interval.bgn << "\n";
    OS << offset << "  file_name: " << di.file_name << "\n";
    OS << offset << "}\n";
    return OS;
}

llvm::raw_ostream &stream(llvm::raw_ostream &OS, const ExtendedInfo & di, std::string offset) {
    OS << offset << "ExtendedInfo{\n";
    OS << offset << "  virtual_size: " << di.virtual_size << "\n";
    OS << offset << "  frequency: " << di.frequency << "\n";
    OS << offset << "  time_interval.end: " << di.time_interval.end << "\n";
    OS << offset << "  time_interval.bgn: " << di.time_interval.bgn << "\n";
    OS << offset << "  jit_type: " << toString(di.jit_type) << "\n";
    OS << offset << "}\n";
    return OS;
}