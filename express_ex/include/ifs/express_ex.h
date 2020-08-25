#ifndef EXPRESS_EX_H
#define EXPRESS_EX_H
#include <vector>
#include <map>
#include "parameterIO.h"

class KEXParser;
class Body;
class Table;


DLL_EXPORT void  jit_init();


class DLL_EXPORT  Express_ex {
public :
    ~Express_ex();
    bool                                parseText(std::string str, bool is_file_name = false);

    bool                                setParameters(const std::map<std::string, SyncParameter*> &parameters_map);

    //  return map< name ,link name>
    std::map<std::string,std::string >  getParameterLinkNamesMap(bool hide_unused = false);
    std::vector<ParameterIfs*>          getOutputParameterVector();

    bool                                genJit();
    bool                                run();

private:
    KEXParser * kex_parser_ = nullptr;
    Body*   body_prototype_ = nullptr;
    Body*   body_ = nullptr;
    Table*  table_ = nullptr;

    std::string error_str_ ="";
};




#endif //EXPRESS_EX_H