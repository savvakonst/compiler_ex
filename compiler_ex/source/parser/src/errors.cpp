#include <iostream>
#include "parser.h"


namespace WinNs {
    //#include <windows.h>
};

void print_error(std::string content) {

    //WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
    //WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
    //WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);

    auto errLine  = errContext->getStart()->getLine();
    auto startPos = errContext->getStart()->getCharPositionInLine();
    auto stopPos  = errContext->getStop()->getCharPositionInLine();
    std::cout << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";


    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0004);
    std::cout << "error: ";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
    std::cout << content << ".\n";


    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
    std::cout << "\t" << errContext->getText() << "\n";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);


    throw  errLine;
}


void EErrorListener::syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e)
{ //std::ostrstream s;
    //WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
    //WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
    //WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);
    std::cout << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0004);
    std::cout << "error: ";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
    std::cout << msg << ".\n";
    //WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
    std::cout << "\t" << offendingSymbol->getText() << "\n";
    //WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
}

