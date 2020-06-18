#include <iostream>
#include <strstream>
#include <fstream>


#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"

#include "body.h"
#include "Line.h"



#include "parser.h"

ParserRuleContext* errContext = NULL;


TreeShapeListener:: TreeShapeListener() : EGrammarBaseListener(){
    NewBody("main", false);
}

TreeShapeListener::~TreeShapeListener(){
    for (auto g : context) {
        delete g;
    }
}

void TreeShapeListener::NewBody(std::string name, bool isPrototype) {
    activBody = new Body(name, isPrototype);
    context.push_back(activBody);
}

void TreeShapeListener::exitAssign(EGrammarParser::AssignContext* ctx)  {
    errContext = ctx;

    activBody->addLine(ctx->ID()->getText(), activBody->pop());
}

void TreeShapeListener::exitAssignParam(EGrammarParser::AssignParamContext* ctx) {
    errContext = ctx;

    auto id = ctx->ID();
    auto stl = ctx->STRINGLITERAL();

    if (stl.size() == 0) {
        for (auto i : id) {
            activBody->addParam(i->getText(), TypeEn::Double_jty, DataStructTypeEn::largeArr_dsty, 1000);
        }
    }
    else if (id.size() == stl.size()) {
        for (int i = 0; i < id.size(); i++) {
            activBody->addParam(id[i]->getText(), TypeEn::Double_jty, DataStructTypeEn::largeArr_dsty, stoi(stl[i]->getText().substr(1)));
        }
    }
    else
        print_error("there are invalid signature ");
}

void TreeShapeListener::exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx)  {
    errContext = ctx;

    for (auto k : ctx->expr()) {
        if (activBody->isRetStackFull()) {
            print_error("there are more then one returned value");
        }
        else
            activBody->addReturn("return", activBody->pop());
    }
}

void TreeShapeListener::enterId(EGrammarParser::IdContext* ctx) {
    errContext = ctx;

    activBody->push(activBody->getLastLineFromName(ctx->ID()->getText()));
}

void TreeShapeListener::exitConst(EGrammarParser::ConstContext* ctx)  {
    errContext = ctx;

    TypeEn targetType = TypeEn::DEFAULT_JTY;
#define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
    switch (ctx->start->getType()) {
        CONV_TY(EGrammarParser::FLOAT, TypeEn::Float_jty);
        CONV_TY(EGrammarParser::DOUBLE, TypeEn::Double_jty);
        CONV_TY(EGrammarParser::INT, TypeEn::Int32_jty);
        CONV_TY(EGrammarParser::INT64, TypeEn::Int64_jty);
    }
    activBody->push(new Variable(ctx->getText(), targetType));
#undef CONV_TY
}

void TreeShapeListener::exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) {
    errContext = ctx;

    TypeEn targetType= TypeEn::DEFAULT_JTY;
#define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
    switch (ctx->tConvBInFunc()->start->getType()) {
        CONV_TY(EGrammarParser::C_FLOAT, TypeEn::Float_jty);
        CONV_TY(EGrammarParser::C_DOUBLE, TypeEn::Double_jty);
        CONV_TY(EGrammarParser::C_INT, TypeEn::Int32_jty);
        CONV_TY(EGrammarParser::C_INT64, TypeEn::Int64_jty);
    }
    activBody->addTypeConvOp(targetType);
#undef CONV_TY
}

void TreeShapeListener::exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) {
    errContext = ctx;

    opCodeEn op = opCodeEn::NONE_op;
#define CONV_TY(depend,target) case (depend):  op=(target) ;break 
    switch (ctx->unaryBInFunc()->start->getType()) {
        CONV_TY(EGrammarParser::LOG, opCodeEn::LOG);
        CONV_TY(EGrammarParser::LOG10, opCodeEn::LOG10);
        CONV_TY(EGrammarParser::COS, opCodeEn::COS);
        CONV_TY(EGrammarParser::SIN, opCodeEn::SIN);
        CONV_TY(EGrammarParser::EXP, opCodeEn::EXP);
    }
#undef CONV_TY
    activBody->addBuiltInFuncOp(op);
}

//arithmetic operations
void TreeShapeListener::exitMulDiv(EGrammarParser::MulDivContext* ctx) {
    errContext = ctx;

    opCodeEn op=(EGrammarParser::MUL == ctx->op->getType()) ? opCodeEn::MUL : opCodeEn::SDIV;
    activBody->addArithmeticOp(op);
}

void TreeShapeListener::exitAddSub(EGrammarParser::AddSubContext* ctx) {
    errContext = ctx;

    opCodeEn op = (EGrammarParser::ADD == ctx->op->getType()) ? opCodeEn::ADD : opCodeEn::SUB;
    activBody->addArithmeticOp(op);
}

void TreeShapeListener::exitPow(EGrammarParser::PowContext* ctx) {
    errContext = ctx;

    activBody->addArithmeticOp(opCodeEn::POW);
}

//conditional 
void TreeShapeListener::exitCondExpr(EGrammarParser::CondExprContext* ctx) {
    errContext = ctx;

    activBody->addSelectOp();
}

void TreeShapeListener::exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) {
    errContext = ctx;

    activBody->addConvolveOp(opCodeEn::convolve);
}

void TreeShapeListener::exitRange(EGrammarParser::RangeContext* ctx) {
    errContext = ctx;

    activBody->addRangeOp(ctx->expr().size());
}

void TreeShapeListener::exitShift(EGrammarParser::ShiftContext * ctx) {
    errContext = ctx;

    activBody->addShiftOp();
}

void TreeShapeListener::exitDecimation(EGrammarParser::DecimationContext * ctx) {
    errContext = ctx;

    activBody->addDecimationOp();
}

//call function
void TreeShapeListener::exitCallFunc(EGrammarParser::CallFuncContext* ctx) {
    errContext = ctx;

    auto exprs = ctx->expr();
    bool b = true;
    std::string targName= ctx->ID()->getText();

    for (auto k : context) {
        if (k->getName() == targName) {
            b = false;
            if (k->getArgCount() != exprs.size()) {
                print_error("there are invalid signature in func: " + targName + " call");
            }
            activBody->addCall(k);
            break;
        }
    }

    if (b){
        print_error("there are no functin with same name");
        activBody->push(new Variable(ctx->getText(), TypeEn::Float_jty));
    }
}

void TreeShapeListener::enterFunc(EGrammarParser::FuncContext* ctx) {
    errContext = ctx;
    NewBody(ctx->ID()->getText(), true);
    for (auto i : ctx->args()->ID()) {
        activBody->addArg(i->getText());
    }
}

void TreeShapeListener::exitFunc(EGrammarParser::FuncContext* ctx) {
    if (activBody->isRetStackEmpty())
        print_error("there are no returned value in func: " + activBody->getName());
    activBody = context[0];
}


