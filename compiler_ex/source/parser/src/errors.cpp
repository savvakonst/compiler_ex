#include <iostream>
#include "parser.h"
#include "llvm/Support/raw_ostream.h"

namespace WinNs {
    //#include <windows.h>
};

void print_error(const std::string &content) {

    //WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
    //WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
    //WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);

    auto errLine  = errContext->getStart()->getLine();
    auto startPos = errContext->getStart()->getCharPositionInLine();
    auto stopPos  = errContext->getStop()->getCharPositionInLine();
    llvm::outs() << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";


    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0004);
    llvm::outs() << "error: ";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
    llvm::outs() << content << ".\n";


    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
    llvm::outs() << "    " << errContext->getText() << "\n";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);


    throw  errLine;
}

void print_IR_error(const std::string &content) {
    llvm::outs() << "IR generation error: \n    "<< content <<"\n";
    throw  content.length();
}

void print_SA_error(const std::string &content) {
    llvm::outs() << "samallarray calc error: \n    " << content << "\n";
    throw  content.length();
}

void EErrorListener::syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e)
{   //std::ostrstream s;
    //WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
    //WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
    //WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);
    llvm::outs() << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0004);
    llvm::outs() << "error: ";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
    llvm::outs() << msg << ".\n";
    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
    llvm::outs() << "\t" << offendingSymbol->getText() << "\n";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
}

