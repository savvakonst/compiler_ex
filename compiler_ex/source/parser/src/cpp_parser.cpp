#include <iostream>
#include <strstream>
#include <fstream>


#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"

#include "body.h"
#include "Line.h"





namespace WinNs {
#ifndef PreRelise
    #include <windows.h>
#endif
}


using namespace antlr4;
using namespace  parser_cpp;


size_t errLine = -1;
ParserRuleContext* errContext = NULL;

void print_error(std::string content) {
#ifndef PreRelise
    WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
    WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
    WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);
#endif
    auto startPos = errContext->getStart()->getCharPositionInLine();
    auto stopPos  = errContext->getStop()->getCharPositionInLine();
    std::cout << "line " << errLine << "; pos " << startPos << ":" << stopPos << ". ";

#ifndef PreRelise
    WinNs::SetConsoleTextAttribute(hConsole,(TextAttribute.wAttributes & 0xfff0)| 0x0004);
#endif
    std::cout << "error: ";
#ifndef PreRelise
    WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
#endif
    std::cout << content << ".\n";

#ifndef PreRelise
    WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
#endif
    std::cout << "\t" << errContext->getText() << "\n";
#ifndef PreRelise
    WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
#endif

    throw  errLine;
}

class TreeShapeListener : public EGrammarBaseListener {
public:

    TreeShapeListener() :EGrammarBaseListener() {
        NewBody("main", false);
    }

    ~TreeShapeListener() {
        for (auto g : context) {
            delete g;
        }
    }

    void NewBody( std::string name, bool isPrototype) {
        activBody = new Body( name, isPrototype);
        context.push_back(activBody);
    }

    virtual void exitAssign(EGrammarParser::AssignContext* ctx) override {
        errLine = ctx->getStart()->getLine();errContext = ctx;

        activBody->addLine(ctx->ID()->getText(), activBody->pop());    
    }

    virtual void exitAssignParam(EGrammarParser::AssignParamContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        auto id = ctx->ID();
        auto stl = ctx->STRINGLITERAL();

        if (stl.size() == 0) {
            for (auto i : id) {
                activBody->addParam(i->getText(), TypeEn::Double_jty, DataStructTypeEn::largeArr_dsty, 1000);
            }
        }
        else if (id.size()== stl.size()){
            for (int i = 0; i < id.size(); i++) {
                activBody->addParam(id[i]->getText(), TypeEn::Double_jty, DataStructTypeEn::largeArr_dsty, stoi(stl[i]->getText().substr(1)));
            }
        }
        else
            print_error("there are invalid signature ");
    }

    virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        for (auto k : ctx->expr() ){
            if (activBody->isRetStackFull()) {
                print_error("there are more then one returned value");
            }
            else 
                activBody->addReturn("return", activBody->pop());
        }
    }

    virtual void enterId(EGrammarParser::IdContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        activBody->push(activBody->getLastLineFromName(ctx->ID()->getText()));
    }

    virtual void exitConst(EGrammarParser::ConstContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        TypeEn targetType = TypeEn::DEFAULT_JTY;
        #define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
        switch(ctx->start->getType()) {
            CONV_TY(EGrammarParser::FLOAT   , TypeEn::Float_jty);
            CONV_TY(EGrammarParser::DOUBLE  , TypeEn::Double_jty);
            CONV_TY(EGrammarParser::INT     , TypeEn::Int32_jty);
            CONV_TY(EGrammarParser::INT64   , TypeEn::Int64_jty);
        }
        activBody->push(new Variable(ctx->getText(), targetType));
        #undef CONV_OP
    } 

    virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) override { 
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        TypeEn targetType= TypeEn::DEFAULT_JTY;
        #define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
        switch (ctx->tConvBInFunc()->start->getType()) {
            CONV_TY(EGrammarParser::C_FLOAT,    TypeEn::Float_jty);
            CONV_TY(EGrammarParser::C_DOUBLE,   TypeEn::Double_jty);
            CONV_TY(EGrammarParser::C_INT,      TypeEn::Int32_jty);
            CONV_TY(EGrammarParser::C_INT64,    TypeEn::Int64_jty);
        }
        activBody->addTypeConvOp(targetType);
        #undef CONV_OP
    }

    virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        opCodeEn op = opCodeEn::NONE_op;
        #define CONV_TY(depend,target) case (depend):  op=(target) ;break 
        switch (ctx->unaryBInFunc()->start->getType()) {
            CONV_TY(EGrammarParser::LOG,    opCodeEn::LOG);
            CONV_TY(EGrammarParser::LOG10,  opCodeEn::LOG10);
            CONV_TY(EGrammarParser::COS,    opCodeEn::COS);
            CONV_TY(EGrammarParser::SIN,    opCodeEn::SIN);
            CONV_TY(EGrammarParser::EXP,    opCodeEn::EXP);
        }
        #undef CONV_OP
        activBody->addBuiltInFuncOp(op);
    }

    //arithmetic operations
    virtual void exitMulDiv(EGrammarParser::MulDivContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        opCodeEn op=(EGrammarParser::MUL == ctx->op->getType()) ? opCodeEn::MUL : opCodeEn::SDIV;
        activBody->addArithmeticOp(op);
    }

    virtual void exitAddSub(EGrammarParser::AddSubContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        opCodeEn op = (EGrammarParser::ADD == ctx->op->getType()) ? opCodeEn::ADD : opCodeEn::SUB;
        activBody->addArithmeticOp(op);
    }

    virtual void exitPow(EGrammarParser::PowContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        activBody->addArithmeticOp(opCodeEn::POW);
    }

    //conditional 
    virtual void exitCondExpr(EGrammarParser::CondExprContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        activBody->addSelectOp();
    }

    virtual void exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        activBody->addConvolveOp(opCodeEn::convolve);
    }

    virtual void exitRange(EGrammarParser::RangeContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        activBody->addRangeOp(ctx->expr().size());
    }

    virtual void exitShift(EGrammarParser::ShiftContext * ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;
        activBody->addShiftOp();
    }

    virtual void exitDecimation(EGrammarParser::DecimationContext * ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;
        activBody->addDecimationOp();
    }

    //call function
    virtual void exitCallFunc(EGrammarParser::CallFuncContext* ctx) override {
        errLine = ctx->getStart()->getLine(); errContext = ctx;

        auto exprs = ctx->expr();
        bool b = false;
        std::string targName= ctx->ID()->getText();
        
        for (auto k : context) {
            if (k->getName()==targName ) { 
                b = true; 
                if (k->getArgCount() != exprs.size()) {
                    print_error("there are invalid signature in func: " + targName + " call" );
                }
                activBody->addCall(k); 
                break; 
            }
        }

        if (b) {}else {
            print_error("there are no functin with same name");
            activBody->push(new Variable(ctx->getText(), TypeEn::Float_jty));
        }
        
        //activBody->push(new Variable(ctx->getText(), TypeEn::Float_jty));
        //activBody->addSelectOp();
    }




    virtual void enterFunc(EGrammarParser::FuncContext* ctx) override {
        errLine = ctx->getStart()->getLine();
        NewBody(ctx->ID()->getText(), true);
        for (auto i : ctx->args()->ID()) {
            activBody->addArg(i->getText());
        }
    }


    virtual void exitFunc(EGrammarParser::FuncContext* ctx) override {
        if (activBody->isRetStackEmpty())
            print_error("there are no returned value in func: " + activBody->getName());

        activBody = context[0];
    }


    Body* activBody;
    //tree::ParseTree* lastLineBranch;
    std::vector<Body*> context;

    private:

};


class EErrorListener : public BaseErrorListener
{
    virtual void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override
    {
        //std::ostrstream s;
#ifndef PreRelise
        WinNs::HANDLE  hConsole = WinNs::GetStdHandle(((unsigned long)(-11)));
        WinNs::CONSOLE_SCREEN_BUFFER_INFO TextAttribute;
        WinNs::GetConsoleScreenBufferInfo(hConsole, &TextAttribute);
#endif
        std::cout << "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
#ifndef PreRelise
        WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0004);
#endif
        std::cout << "error: ";
#ifndef PreRelise
        WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
#endif
        std::cout << msg << ".\n";



#ifndef PreRelise
        WinNs::SetConsoleTextAttribute(hConsole, (TextAttribute.wAttributes & 0xfff0) | 0x0102);
#endif
        std::cout <<"\t"<< offendingSymbol->getText() << "\n";
#ifndef PreRelise
        WinNs::SetConsoleTextAttribute(hConsole, TextAttribute.wAttributes);
#endif

    }
};



int main(int argc, const char* argv[]) {
    
#ifdef PreRelise
    std::string content(
        "x, y, z = param() #asfafasdfs\n"
        "a = x + 0\n"
        "func calc_new(i, j) :\n"
        "    a = 0 * 1\n"
        "    v = i + j **i\n"
        "    return v;\n"
        "b = a + 0\n"
        "v = (3.0 ? 100 : b + 90 ** 1.0)\n"
        "b = float(a) * (3 ? 100 : 90 ** 1)\n"
        "g = calc_new(b , 1.0)\n"
        "b = calc_new(b , 2.0)\n"
        "b = float(b)\n\n");

    ANTLRInputStream input(
        content);
    
#else
    std::ifstream ifs("formula.txt");
    std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );

    ANTLRInputStream input(content);

#endif

    //EErrorListener

    EGrammarLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    EGrammarParser parser(&tokens);
    
    parser.removeErrorListeners();
    EErrorListener errorListner;
    parser.addErrorListener(&errorListner);

    tree::ParseTree* tree = parser.start();
    TreeShapeListener listener;


    try
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        listener.activBody->print();
        listener.activBody->symplyfy();
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        listener.activBody->print("", false, true);
    }catch(size_t k)
    {
        
    }



    return 0;
}

