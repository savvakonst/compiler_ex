#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <strstream>
#include <fstream>
#include <list>

#include "defWarningIgnore.h"
#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"
#include "undefWarningIgnore.h"

#include "body.h"
#include "line.h"




using namespace antlr4;
//using namespace  parser_cpp;


extern ParserRuleContext* g_err_context ;

class EErrorListener : public BaseErrorListener
{
    virtual void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override;
};


class TreeShapeListener : public EGrammarBaseListener {
public:

    TreeShapeListener();
    TreeShapeListener(Body * body, const std::vector<Body*> &context={});
    // : EGrammarBaseListener()
    ~TreeShapeListener();


    void NewBody(std::string name, bool is_template);

    virtual void exitAssign(EGrammarParser::AssignContext* ctx) override;
    virtual void exitAssignParam(EGrammarParser::AssignParamContext* ctx) override;
    virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx) override;
    virtual void enterId(EGrammarParser::IdContext* ctx) override;
    virtual void exitConst(EGrammarParser::ConstContext* ctx) override;
    virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) override;
    virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) override;
    //arithmetic operations
    virtual void exitInv(EGrammarParser::InvContext * ctx) override;
    virtual void exitMulDiv(EGrammarParser::MulDivContext* ctx) override;
    virtual void exitAddSub(EGrammarParser::AddSubContext* ctx) override;
    virtual void exitPow(EGrammarParser::PowContext* ctx) override;
    virtual void exitMoreLess(EGrammarParser::MoreLessContext * ctx) override;
    virtual void exitMoreeqLesseq(EGrammarParser::MoreeqLesseqContext * ctx) override;
    virtual void exitEquality(EGrammarParser::EqualityContext * ctx) override;
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

    Body* getMainBody();

    Body* activ_body_;
    std::vector<Body*> context_;

private:
    void setPos(ParserRuleContext* ctx);
};

class KEXParser {
public:

    KEXParser(Body* body,  std::string str, bool is_file_name = true) {
        listener_ = TreeShapeListener(body);
        init(str, is_file_name);
    }


    KEXParser(std::string str, bool is_file_name = true, std::map<std::string, bool/*is_file_name*/> lib_str_map = {}){
        auto body = listener_.activ_body_ ;

        for (auto i : lib_str_map) 
            init(i.first, i.second);

        init(str, is_file_name);
    }


    ~KEXParser() {

    }

    TreeShapeListener* getListener() {
        return &listener_;
    }

    Body*  getActivBody() {
        return listener_.activ_body_;
    }

private:
    std::vector<Body*> & getContext() {
        return listener_.context_;
    }

    void init(std::string str, bool is_file_name = true) {

        std::string content;
        if (is_file_name) {
            std::ifstream ifs(str);
            content = std::string(
                (std::istreambuf_iterator<char>(ifs)),
                (std::istreambuf_iterator<char>()));
        }
        else
            content = str;


        ANTLRInputStream* input_  =new ANTLRInputStream(content + "\n");
        EGrammarLexer* lexer_  =new EGrammarLexer(input_);
        CommonTokenStream* tokens_ =new CommonTokenStream(lexer_);
        EGrammarParser* parser_ =new EGrammarParser(tokens_);

        parser_->removeErrorListeners();
        EErrorListener* error_listner_ = new EErrorListener;
        parser_->addErrorListener(error_listner_);

        tree::ParseTree* tree_ = parser_->start();
        tree::ParseTreeWalker::DEFAULT.walk(&listener_, tree_);


        delete error_listner_;
        delete parser_;
        delete tokens_;
        delete lexer_;
        delete input_;

    }

    
    TreeShapeListener listener_;
};


#endif