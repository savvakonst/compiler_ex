#define _DECL_DLLMAIN



#include "Types.h"
#include "LibInterface.h"

#include <Process.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <windows.h>



#define  RDBUFFSIZE 1024 * 16

LibInterface::LibInterface() {
    Header = NULL;
    GoExpr = NULL;
    Calc = NULL;
    hDLL = LoadLibraryExA("parser.dll", NULL, 0);
    if (NULL != hDLL) {
        Calc = (Calc_t)GetProcAddress(hDLL, "Calc");
        if (NULL == Calc) {
            FreeLibrary(hDLL);
        }

    }
    llvm::outs() << "\n LibInterface init successfull :\n ---------\n";
}

LibInterface::LibInterface(std::string ExprORFilename, bool FileExprSource) {
    Header = NULL;
    GoExpr = NULL;
    Calc = NULL;
    hDLL = LoadLibraryExA("parser.dll", NULL, 0);
    if (NULL != hDLL) {
        Calc = (Calc_t)GetProcAddress(hDLL, "Calc");
        if (NULL == Calc) {
            FreeLibrary(hDLL);
        }
    }
    setEexpr(ExprORFilename, FileExprSource);
    llvm::outs() << "\n LibInterface init successfull :\n ---------\n";
}


void LibInterface::setEexpr(std::string ExprORFilename, bool FileExprSource) {
    if (FileExprSource)
        read_script(ExprORFilename);
    else
        read_expr(ExprORFilename);
}



void LibInterface::read_expr(std::string expr) {

    std::string expr_with_eof;
    expr_with_eof = expr + "\n";
    char* txt = new char[expr_with_eof.length()];
    strcpy(txt, expr_with_eof.c_str());

    char* ch = txt;
    int size = 0;
    while (*ch) {
        ch++;
        size++;
    }
    size++;

    GoExpr = new  GoString{ txt, size };
}
 

void  LibInterface::read_script(std::string Filename) {

    char* ch;
    char* txt = new char[RDBUFFSIZE];
    FILE* formulaFp;

    if ((formulaFp = fopen(Filename.c_str(), "r")) == NULL) {
        return ;
    }

    fread(txt, 1, RDBUFFSIZE, formulaFp);

    if (feof(formulaFp)) {
        printf("Premature end of file.");
    }
    else {
        fclose(formulaFp);
        printf("File read error.");
        return ;
    };

    fclose(formulaFp);

    ch = txt;
    int size = 0;

    while (*ch) {
        ch++;
        size++;
    }
    size++;
    GoExpr = new  GoString{ txt, size };
}



 Header_ptr* LibInterface::CallCalcFunction(uint64_t buffer_len, uint64_t data_len, GoSlice<int64_t>* iTypes) {

    Header_ptr* hrReturnVal = NULL;

    if (GoExpr == NULL) 
        return NULL;

    if (NULL != Calc) {
       Header = Calc(*GoExpr, buffer_len, data_len, *iTypes);
       hrReturnVal = Header;
    }
    if (hrReturnVal!= NULL)
        llvm::outs() << "\n Call Calc Function successfull :\n ---------\n";
    delete[] GoExpr->p;
    delete GoExpr;
    return hrReturnVal;
}

