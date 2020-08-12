#include <iostream>
#include "treeShapeListener.h"
#include "printer.h"
//#include "llvm/Support/raw_ostream.h"

namespace WinNs {
    //#include <windows.h>
};

void print_error(const std::string &content) {

    auto errLine  = errContext->getStart()->getLine();
    auto startPos = errContext->getStart()->getCharPositionInLine();
    auto stopPos  = errContext->getStop()->getCharPositionInLine();
    llvm::outs() << ExColors::RED << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";

    llvm::outs() << "error: " << ExColors::RESET;
    llvm::outs() << content << ".\n" ;

    llvm::outs() << ExColors::GREEN << "    " << errContext->getText() << "\n" <<ExColors::RESET;

    throw  content.length();
}

void print_IR_error(const std::string &content) {
    llvm::outs() << ExColors::RED << "IR generation error: \n    " << ExColors::RESET << content <<"\n";

    throw  content.length();
}

void print_SA_error(const std::string &content) {
    llvm::outs() << ExColors::RED << "samallarray calc error: \n    " << ExColors::RESET << content << "\n";
    throw  content.length();
}

void EErrorListener::syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e)
{   
    llvm::outs() << ExColors::RED << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
    llvm::outs() << "error: "<< ExColors::RESET;
    llvm::outs() << msg << ".\n"  ;
    llvm::outs() << ExColors::GREEN << "\t" << offendingSymbol->getText() << "\n" << ExColors::RESET;
    throw  size_t(1);
}

