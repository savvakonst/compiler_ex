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
    Namespace(std::string name, Namespace* parent=NULL) {
        name_=name; parent_=parent;
    }
    ~Namespace(){
    }

    void addId(const std::string &id){
        variable_list_.push_back(id);
    }

    bool checkIdExistens(const std::string &id){
        for (std::string & i : variable_list_ )
            if (i == id) { return true; }



        return false;
    }
    std::string getName() { return name_; }

private:
    std::string                 name_;
    std::vector<std::string>    variable_list_;
    Namespace*                  parent_=NULL;
};


class EErrorListener : public antlr4::BaseErrorListener
{
public:
    EErrorListener(HighlightStyle * style){
        highlight_style_=style;
    }

private:
    virtual void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override
    {
        //qDebug()<<"line " << line << "; pos " << charPositionInLine << ":" << "_" << ". \n";
        //errorMessage = "line " << line << "; pos " << charPositionInLine << ":" << "_" << ". ";
        
        int start  = (int) charPositionInLine;
        int length = (int)(offendingSymbol->getText().size());
        auto format= &highlight_style_->unknownIdFormat;

        format_map_[line].push_back({start, length, format});

        out_ << "line " << line << "; pos " << charPositionInLine << ":" << length << ". ";
        out_ << "\n\terror: ";
        out_ << msg.c_str() << ".\n";
        //out_ << "\t" << offendingSymbol->getText().c_str() << "\n";
        error_=true;
    }

public:
    QString getErrorMessage(){
        return QString::fromStdString(out_.str());
    }
    bool hasError(){
        return error_;
    }

private:
    bool error_=false;
    std::stringstream out_;
    HighlightStyle * highlight_style_;
public:
    std::map<int, std::vector<HighlightUint> >    format_map_ ;
    //QString errorMessage;

};



class  TreeShapeListener : public EGrammarBaseListener {

public:
    TreeShapeListener(HighlightStyle * style, const std::vector<Namespace*> &lib_namespace ) : EGrammarBaseListener() {
        highlight_style_ = style;

        if (lib_namespace.size()==0) 
            namespase_list_.push_back(new Namespace("main"));
        else
            for (auto i :lib_namespace)
                namespase_list_.push_back(new Namespace(*i));

        activ_namespace_ = namespase_list_[0];
    }

    ~TreeShapeListener(){
        for(auto &i : namespase_list_){
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
        format_map_[line].push_back({start,length,format});
    }

    void addFormatUint(antlr4::ParserRuleContext* ctx,int length,QTextCharFormat *format){
        int line    =(int)ctx->getStart()->getLine();
        int start   =(int)ctx->getStart()->getCharPositionInLine();
        format_map_[line].push_back({start,length,format});
    }

    bool checkFunkExistens(const std::string &id){

        for (auto & i: namespase_list_)
            if (i->getName()==id) 
                return true;
        return false;
    }

    virtual void exitAssign(EGrammarParser::AssignContext* ctx) override{
        std::string txt=ctx->ID()->getText();
        activ_namespace_->addId(txt);
        addFormatUint(ctx->ID()->getSymbol(),&highlight_style_->idFormat);
    }

    virtual void exitAssignParam(EGrammarParser::AssignParamContext* ctx) override{
        if ((ctx->getStart()==NULL)|| (ctx->SPEC_SYM==NULL) ) return ;
        for(auto &i : ctx->ID() ){
            if (i!=NULL){
                std::string txt=i->getText();
                if ( activ_namespace_->checkIdExistens(txt) )
                    addFormatUint(i->getSymbol(),& highlight_style_->unknownIdFormat);
                else
                    addFormatUint(i->getSymbol(),& highlight_style_->idFormat);
                activ_namespace_->addId(txt);
           }
        }
        addFormatUint(ctx->SPEC_SYM,& highlight_style_->specSymbolFormat);
        for(auto &i : ctx->STRINGLITERAL() ){
            if (i!=NULL)
                addFormatUint(i->getSymbol(),& highlight_style_->stringLiteralFormat);
        }

    }

    virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx) override{
        if (ctx->SPEC_SYM!=NULL)
            addFormatUint(ctx->SPEC_SYM,& highlight_style_->specSymbolFormat);
    }

    virtual void enterId(EGrammarParser::IdContext* ctx) override{
        if ( activ_namespace_->checkIdExistens( ctx->ID()->getText() ) )
            addFormatUint(ctx->ID()->getSymbol(),& highlight_style_->idFormat);
        else
            addFormatUint(ctx->ID()->getSymbol(),& highlight_style_->unknownIdFormat);
    }

    virtual void exitConst(EGrammarParser::ConstContext* ctx) override {
        addFormatUint(ctx->getStart(),& highlight_style_->constFormat);
    }

    virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) override{
        addFormatUint(ctx->tConvBInFunc()->getStart(),& highlight_style_->builtInFuncFormat);
    }

    virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) override{
        addFormatUint(ctx->unaryBInFunc()->getStart(),& highlight_style_->builtInFuncFormat);
    }

    virtual void exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlight_style_->builtInFuncFormat);
    }

    virtual void exitRange(EGrammarParser::RangeContext* ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlight_style_->builtInFuncFormat);
    }

    virtual void exitShift(EGrammarParser::ShiftContext * ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlight_style_->builtInFuncFormat);
    }

    virtual void exitDecimation(EGrammarParser::DecimationContext * ctx) override{
        addFormatUint(ctx->SPEC_SYM,& highlight_style_->builtInFuncFormat);
    }

    virtual void exitCallFunc(EGrammarParser::CallFuncContext* ctx) override{
        if (checkFunkExistens(ctx->ID()->getText()))
            addFormatUint(ctx->ID()->getSymbol(),& highlight_style_->funcFormat);
        else
            addFormatUint(ctx->ID()->getSymbol(),& highlight_style_->unknownFuncFormat);
    }

    virtual void enterFunc(EGrammarParser::FuncContext* ctx) override{
        if ((ctx->ID()==NULL) || (ctx->getStart()==NULL)|| (ctx->args()==NULL) ) {return ;}

        activ_namespace_=new Namespace(ctx->ID()->getText());

        namespase_list_.push_back(activ_namespace_);

        addFormatUint(ctx->getStart(),& highlight_style_->specSymbolFormat);
        addFormatUint(ctx->ID()->getSymbol(),& highlight_style_->funcFormat);

        for (auto &i : ctx->args()->ID()){
            std::string txt=i->getText();
             if ( activ_namespace_->checkIdExistens(txt) )
                 addFormatUint(i->getSymbol(),& highlight_style_->unknownIdFormat);
             else
                 addFormatUint(i->getSymbol(),& highlight_style_->idFormat);
             activ_namespace_->addId(txt);
        }

        if(ctx->SPEC_SYM)
            addFormatUint(ctx->SPEC_SYM,& highlight_style_->specSymbolFormat);
    }

    virtual void exitFunc(EGrammarParser::FuncContext* ctx) override{
        if ((ctx->ID()==NULL) || (ctx->getStart()==NULL)||(ctx->args()==NULL)) return;
        activ_namespace_ = namespase_list_[0];
    }
    
    
    std::vector<Namespace*>      getNamespaceList() {
        std::vector<Namespace*>  namespase_list;
        for (auto i : namespase_list_)
            namespase_list.push_back(new Namespace(*i));
        return namespase_list;
    }

private:
    //namespase
    Namespace*              activ_namespace_;
    HighlightStyle*         highlight_style_;
    std::vector<Namespace*> namespase_list_;

public:
    std::map<int, std::vector<HighlightUint>>    format_map_ ;

};


#endif
