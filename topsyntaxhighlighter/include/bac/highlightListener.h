#ifndef PARSER_H
#define PARSER_H


#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"


#include "highlighter.h"
#include "highlightStyle.h"
#include <QtWidgets>
#include <sstream>
typedef struct {
    int start;
    int count;
    QTextCharFormat *format;
} HighlightUint;




class  Namespace {
public:
    Namespace(std::string name_){name=name_;}
    ~Namespace(){}

    void addId(const std::string &id){
        idList.push_back(id);
    }

    bool checkIdExistens(const std::string &id){
        for (std::string & i:   idList )
            if (i==id) {return true;  }
        return false;
    }

private:
    std::string                 name;
    std::vector<std::string>    idList;
};


class EErrorListener : public antlr4::BaseErrorListener
{
public:
    EErrorListener(HighlightStyle * style){
        highlightStyle=style;
    }
private:
    virtual void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override
    {
        //qDebug()<<"line " << line << "; pos " << charPositionInLine << ":" << "_" << ". \n";
        //errorMessage = "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";

        int start  = (int) charPositionInLine;
        int length = (int)(offendingSymbol->getText().size());
        auto format= &highlightStyle->unknownIdFormat;

        formatMap[line].push_back({start, length, format});

        out << "line " << line << "; pos " << charPositionInLine << ":" << length << ". ";
        out << "\n\terror: ";
        out << msg.c_str() << ".\n";
        //out << "\t" << offendingSymbol->getText().c_str() << "\n";
        error=true;
    }

public:
    QString getErrorMessage(){
        //std::string tmp =out.str();
        //qDebug()<< QString::fromStdString(out.str());
        //return QString::fromUtf8(out.str().data(),out.width());
        return QString::fromStdString(out.str());
    }
    bool hasError(){
        return error;
    }

private:
    bool error=false;
    std::stringstream out;
    HighlightStyle * highlightStyle;
public:
    std::map<int, std::vector<HighlightUint> >    formatMap ;
    //QString errorMessage;

};



class  TreeShapeListener : public EGrammarBaseListener {

public:
    TreeShapeListener(HighlightStyle * style): EGrammarBaseListener() {
        highlightStyle=style;
        activNamespace=new Namespace("main");
        namespaseList.push_back(activNamespace);

    }

    ~TreeShapeListener(){
        for(auto &i : namespaseList){
            delete i ;
        }
    };

    void addFormatUint(antlr4::Token* ctx,QTextCharFormat *format){
        int line    =(int)ctx->getLine();
        int start   =(int)ctx->getCharPositionInLine();
        int length  =(int)ctx->getStopIndex()-(int)ctx->getStartIndex()+1;
        //qDebug()<<line<<", "<<start<<", "<<length<<"\n";
        if(ctx->getLine()==NULL)
            return ;
        formatMap[line].push_back({start,length,format});
    }

    void addFormatUint(antlr4::ParserRuleContext* ctx,int length,QTextCharFormat *format){
        int line    =(int)ctx->getStart()->getLine();
        int start   =(int)ctx->getStart()->getCharPositionInLine();
        formatMap[line].push_back({start,length,format});
    }

    bool checkFunkExistens(const std::string &id){

        for (std::string & i: functionNamespace )
            if (i==id) return true;
        return false;
    }

    virtual void exitAssign(EGrammarParser::AssignContext* ctx) override{
        std::string txt=ctx->ID()->getText();
        activNamespace->addId(txt);
        addFormatUint(ctx->ID()->getSymbol(),&highlightStyle->idFormat);
    }

    virtual void exitAssignParam(EGrammarParser::AssignParamContext* ctx) override{
        if ((ctx->getStart()==NULL)|| (ctx->SPEC_SYM==NULL) ) return ;
        for(auto &i : ctx->ID() ){
            if (i!=NULL){
                std::string txt=i->getText();
                if ( activNamespace->checkIdExistens(txt) )
                    addFormatUint(i->getSymbol(),& highlightStyle->unknownIdFormat);
                else
                    addFormatUint(i->getSymbol(),& highlightStyle->idFormat);
                activNamespace->addId(txt);
           }
        }
        addFormatUint(ctx->SPEC_SYM,& highlightStyle->specSymbolFormat);
        for(auto &i : ctx->STRINGLITERAL() ){
            if (i!=NULL)
                addFormatUint(i->getSymbol(),& highlightStyle->stringLiteralFormat);
        }

    }

    virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx) override{
        if (ctx->SPEC_SYM!=NULL)
            addFormatUint(ctx->SPEC_SYM,& highlightStyle->specSymbolFormat);
    }

    virtual void enterId(EGrammarParser::IdContext* ctx) override{
        if ( activNamespace->checkIdExistens( ctx->ID()->getText() ) )
            addFormatUint(ctx->ID()->getSymbol(),& highlightStyle->idFormat);
        else
            addFormatUint(ctx->ID()->getSymbol(),& highlightStyle->unknownIdFormat);
    }

    virtual void exitConst(EGrammarParser::ConstContext* ctx) override {
        addFormatUint(ctx->getStart(),& highlightStyle->constFormat);
    }

    virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) override{
        addFormatUint(ctx->tConvBInFunc()->getStart(),& highlightStyle->builtInFuncFormat);
    }

    virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) override{
        addFormatUint(ctx->unaryBInFunc()->getStart(),& highlightStyle->builtInFuncFormat);
    }

    virtual void exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlightStyle->builtInFuncFormat);
    }

    virtual void exitRange(EGrammarParser::RangeContext* ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlightStyle->builtInFuncFormat);
    }

    virtual void exitShift(EGrammarParser::ShiftContext * ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlightStyle->builtInFuncFormat);
    }

    virtual void exitDecimation(EGrammarParser::DecimationContext * ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlightStyle->builtInFuncFormat);
    }

    virtual void exitCallFunc(EGrammarParser::CallFuncContext* ctx) override{
        if (checkFunkExistens(ctx->ID()->getText()))
            addFormatUint(ctx->ID()->getSymbol(),& highlightStyle->funcFormat);
        else
            addFormatUint(ctx->ID()->getSymbol(),& highlightStyle->unknownFuncFormat);
    }

    virtual void enterFunc(EGrammarParser::FuncContext* ctx) override{
        if ((ctx->ID()==NULL) || (ctx->getStart()==NULL)|| (ctx->args()==NULL) ) {return ;}

        activNamespace=new Namespace(ctx->ID()->getText());
        namespaseList.push_back(activNamespace);

        addFormatUint(ctx->getStart(),& highlightStyle->specSymbolFormat);
        addFormatUint(ctx->ID()->getSymbol(),& highlightStyle->funcFormat);

        for (auto &i : ctx->args()->ID()){
            std::string txt=i->getText();
             if ( activNamespace->checkIdExistens(txt) )
                 addFormatUint(i->getSymbol(),& highlightStyle->unknownIdFormat);
             else
                 addFormatUint(i->getSymbol(),& highlightStyle->idFormat);
             activNamespace->addId(txt);
        }

        if(ctx->SPEC_SYM)
            addFormatUint(ctx->SPEC_SYM,& highlightStyle->specSymbolFormat);
    }

    virtual void exitFunc(EGrammarParser::FuncContext* ctx) override{

        if ((ctx->ID()==NULL) || (ctx->getStart()==NULL)||(ctx->args()==NULL)) return;
        functionNamespace.push_back(ctx->ID()->getText());
        activNamespace=namespaseList[0];
    }

private:
    //namespase
    Namespace*                      activNamespace;
    HighlightStyle *                highlightStyle;
    std::vector<Namespace*>         namespaseList;
    std::vector<std::string>        functionNamespace;

    std::vector<std::string>        idList    ;

public:
    std::map<int, std::vector<HighlightUint>>    formatMap ;

};


#endif
