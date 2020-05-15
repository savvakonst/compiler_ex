
#ifndef LibInterface_H
#define LibInterface_H
#include "Types.h"
#include <windows.h>

typedef Header_ptr* (*Calc_t)(GoString, uint64_t, uint64_t, GoSlice<int64_t> );


class LibInterface {
public:
    LibInterface();
    LibInterface(std::string ExprORFilename, bool FileExprSource);

    void setEexpr(std::string ExprORFilename,bool FileExprSource);
    Header_ptr* CallCalcFunction(uint64_t buffer_len, uint64_t data_len, GoSlice<int64_t>* iTypes);
    Header_ptr* Header;
    
private:
    Calc_t Calc;
    void read_script(std::string Filename);
    void read_expr(std::string Filename);
    HINSTANCE hDLL;
    GoString* GoExpr;
    
};


Function* CreateCallFunction(Module* M, LLVMContext& Context);
Function* CreateMainFunction(Module* M, LLVMContext& Context, Header_ptr* header);
#endif