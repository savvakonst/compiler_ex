#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <strstream>
#include <fstream>


#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"

#include "body.h"
#include "line.h"




using namespace antlr4;
//using namespace  parser_cpp;


extern ParserRuleContext* errContext ;

class EErrorListener : public BaseErrorListener
{
    virtual void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override;
};


class TreeShapeListener : public EGrammarBaseListener {
public:

    TreeShapeListener();// : EGrammarBaseListener()
    ~TreeShapeListener();

    void NewBody(std::string name, bool isPrototype);

    virtual void exitAssign(EGrammarParser::AssignContext* ctx) override;
    virtual void exitAssignParam(EGrammarParser::AssignParamContext* ctx) override;
    virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx) override;
    virtual void enterId(EGrammarParser::IdContext* ctx) override;
    virtual void exitConst(EGrammarParser::ConstContext* ctx) override;
    virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) override;
    virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) override;
    //arithmetic operations
    virtual void exitMulDiv(EGrammarParser::MulDivContext* ctx) override;
    virtual void exitAddSub(EGrammarParser::AddSubContext* ctx) override;
    virtual void exitPow(EGrammarParser::PowContext* ctx) override;
    //conditional 
    virtual void exitCondExpr(EGrammarParser::CondExprContext* ctx) override;
    virtual void exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) override;
    virtual void exitRange(EGrammarParser::RangeContext* ctx) override;
    virtual void exitShift(EGrammarParser::ShiftContext * ctx) override;
    virtual void exitDecimation(EGrammarParser::DecimationContext * ctx) override;
    //call function
    virtual void exitCallFunc(EGrammarParser::CallFuncContext* ctx) override;
    virtual void enterFunc(EGrammarParser::FuncContext* ctx) override;
    virtual void exitFunc(EGrammarParser::FuncContext* ctx) override;
    //array definition
    virtual void exitSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext* ctx) override;

    Body* activBody_;
    std::vector<Body*> context;

private:
};

class KEXParser {
public:

    KEXParser(std::string file_name) {
        std::ifstream ifs(file_name);
        std::string content(
            (std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>())
        );

        //ANTLRInputStream    input(content + "\n");

        //EGrammarLexer       lexer(&input);
        //CommonTokenStream   tokens(&lexer);
        //EGrammarParser      parser(&tokens);

        input_  =new ANTLRInputStream(content + "\n");
        lexer_  =new EGrammarLexer(input_);
        tokens_ =new CommonTokenStream(lexer_);
        parser_ =new EGrammarParser(tokens_);

        parser_->removeErrorListeners();
        errorListner_ = new EErrorListener;
        parser_->addErrorListener(errorListner_);

        tree_ = parser_->start();
    }
    ~KEXParser() {
        //delete tree_;
        delete errorListner_;
        delete parser_;
        delete tokens_;
        delete lexer_;
        delete input_;
    }

    TreeShapeListener* getListener() {
        return &listener_;
    }

    tree::ParseTree* getTree() {
        return tree_;
    }

    void walk() {
        tree::ParseTreeWalker::DEFAULT.walk(&listener_, tree_);
    }

    Body*  getActivBody() {
        return listener_.activBody_;
    }

private:
    ANTLRInputStream   *   input_;
    EGrammarLexer      *   lexer_;
    CommonTokenStream  *   tokens_;
    EGrammarParser     *   parser_;
    EErrorListener     *   errorListner_;

    tree::ParseTree*  tree_;
    TreeShapeListener listener_;
};


#endif