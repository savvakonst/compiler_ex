#include <iostream>
#include "treeShapeListener.h"
#include "printer.h"

#include "defWarningIgnore.h"
//#include "llvm/Support/raw_ostream.h"
#include "undefWarningIgnore.h"


namespace WinNs {
    //#include <windows.h>
};

std::string g_error_str="";
extern PosInText g_pos;
extern std::string pos_file;


void print_error(const std::string &content) {
    
    auto errLine  = g_pos.start_line;
    auto startPos = g_pos.start_char_pos;
    auto stopPos  = g_pos.stop_char_pos;

#ifdef CONSOLE_APP_
    llvm::outs() << ExColors::RED << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";

    llvm::outs() << "error: " << ExColors::RESET;
    llvm::outs() << content << ".\n" ;

    llvm::outs() << ExColors::GREEN << "    " << g_err_context->getText() << "\n" <<ExColors::RESET;
#else
    std::stringstream s;
    s << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";
    s << "error: " << content << ".\n";
    s << g_err_context->getText() << "\n";
    g_error_str = s.str();
#endif

    throw  content.length();
}

void print_IR_error(const std::string &content) {
#ifdef CONSOLE_APP_
    llvm::outs() << ExColors::RED << "IR generation error: \n    " << ExColors::RESET << content <<"\n";
#else
    std::stringstream s;
    s << "IR generation error: \n    "  << content << "\n";
    g_error_str = s.str();
#endif
    throw  content.length();
}

void print_SA_error(const std::string &content) {
#ifdef CONSOLE_APP_
    llvm::outs() << ExColors::RED << "samallarray calc error: \n    " << ExColors::RESET << content << "\n";
#else
    std::stringstream s;
    s << "samallarray calc error: \n    " << content << "\n";
    g_error_str = s.str();
#endif
    throw  content.length();
}

void EErrorListener::syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e)
{   
#ifdef CONSOLE_APP_
    llvm::outs() << ExColors::RED << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
    llvm::outs() << "error: "<< ExColors::RESET;
    llvm::outs() << msg << ".\n"  ;
    llvm::outs() << ExColors::GREEN << "\t" << offendingSymbol->getText() << "\n" << ExColors::RESET;
#else
    std::stringstream s;
    s << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
    s << "error: ";
    s << msg << ".\n";
    s  << "\t" << offendingSymbol->getText() << "\n" ;
    g_error_str = s.str();
#endif
    throw  size_t(1);
}

