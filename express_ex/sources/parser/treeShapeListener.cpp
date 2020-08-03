#include "treeShapeListener.h"
//#include "parser.h"

ParserRuleContext* errContext = NULL;


TreeShapeListener:: TreeShapeListener() : EGrammarBaseListener(){
    NewBody("main", true);
}

TreeShapeListener::~TreeShapeListener(){
    for (auto g : context) {
        delete g;
    }
}

void TreeShapeListener::NewBody(std::string name, bool isPrototype) {
    activBody_ = new Body(name, isPrototype);
    context.push_back(activBody_);
}

void TreeShapeListener::exitAssign(EGrammarParser::AssignContext* ctx)  {
    errContext = ctx;
    activBody_->addLine(ctx->ID()->getText(), activBody_->pop());
}

void TreeShapeListener::exitAssignParam(EGrammarParser::AssignParamContext* ctx) {
    errContext = ctx;
    auto id = ctx->ID();
    auto stl = ctx->STRINGLITERAL();

    if (stl.size() == 0) 
        for (auto i : id) 
            activBody_->addParam(i->getText(), TypeEn::double_jty, DataStructTypeEn::largeArr_dsty, 100000);
    
    else if (id.size() == stl.size()) 
        for (int i = 0; i < id.size(); i++) {
            //activBody_->addParam(id[i]->getText(), TypeEn::double_jty, DataStructTypeEn::largeArr_dsty, stoi(stl[i]->getText().substr(1)));
            std::string s=stl[i]->getText();
            activBody_->addParam(id[i]->getText(), s.substr(1, s.length()-2), DataStructTypeEn::largeArr_dsty);
        }
    
    else
        print_error("there are invalid signature ");
}

void TreeShapeListener::exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx)  {
    errContext = ctx;
    for (auto k : ctx->expr())
        if (activBody_->isRetStackFull())
            print_error("there are more then one returned value");
        else
            activBody_->addReturn("return", activBody_->pop());
}

void TreeShapeListener::enterId(EGrammarParser::IdContext* ctx) {
    errContext = ctx;
    activBody_->push(activBody_->getLastLineFromName(ctx->ID()->getText()));
}

void TreeShapeListener::exitConst(EGrammarParser::ConstContext* ctx)  {
    errContext = ctx;
    TypeEn targetType = TypeEn::DEFAULT_JTY;
#define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
    switch (ctx->start->getType()) {
        CONV_TY(EGrammarParser::FLOAT, TypeEn::float_jty);
        CONV_TY(EGrammarParser::DOUBLE, TypeEn::double_jty);
        CONV_TY(EGrammarParser::INT, TypeEn::int32_jty);
        CONV_TY(EGrammarParser::INT64, TypeEn::int64_jty);
    }
    activBody_->push(new Variable(ctx->getText(), targetType));
#undef CONV_TY
}

void TreeShapeListener::exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext  *ctx) {
    errContext = ctx;
    TypeEn targetType= TypeEn::DEFAULT_JTY;
#define CONV_TY(depend,target) case (depend):  targetType=(target) ;break 
    switch (ctx->tConvBInFunc()->start->getType()) {
        CONV_TY(EGrammarParser::C_FLOAT, TypeEn::float_jty);
        CONV_TY(EGrammarParser::C_DOUBLE, TypeEn::double_jty);
        CONV_TY(EGrammarParser::C_INT, TypeEn::int32_jty);
        CONV_TY(EGrammarParser::C_INT64, TypeEn::int64_jty);
    }
    activBody_->addTypeConvOp(targetType);
#undef CONV_TY
}

void TreeShapeListener::exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext* ctx) {
    errContext = ctx;
    OpCodeEn op = OpCodeEn::none_op;
#define CONV_TY(depend,target) case (depend):  op=(target) ;break 
    switch (ctx->unaryBInFunc()->start->getType()) {
        CONV_TY(EGrammarParser::LOG, OpCodeEn::log);
        CONV_TY(EGrammarParser::LOG10, OpCodeEn::log10);
        CONV_TY(EGrammarParser::COS, OpCodeEn::cos);
        CONV_TY(EGrammarParser::SIN, OpCodeEn::sin);
        CONV_TY(EGrammarParser::EXP, OpCodeEn::exp);
    }
#undef CONV_TY
    activBody_->addBuiltInFuncOp(op);
}

//arithmetic operations
void TreeShapeListener::exitMulDiv(EGrammarParser::MulDivContext* ctx) {
    errContext = ctx;
    OpCodeEn op=(EGrammarParser::MUL == ctx->op->getType()) ? OpCodeEn::mul : OpCodeEn::sdiv;
    activBody_->addArithmeticOp(op);
}

void TreeShapeListener::exitAddSub(EGrammarParser::AddSubContext* ctx) {
    errContext = ctx;
    OpCodeEn op = (EGrammarParser::ADD == ctx->op->getType()) ? OpCodeEn::add : OpCodeEn::sub;
    activBody_->addArithmeticOp(op);
}

void TreeShapeListener::exitPow(EGrammarParser::PowContext* ctx) {
    errContext = ctx;
    activBody_->addArithmeticOp(OpCodeEn::pow);
}

//conditional 
void TreeShapeListener::exitCondExpr(EGrammarParser::CondExprContext* ctx) {
    errContext = ctx;
    activBody_->addSelectOp();
}

void TreeShapeListener::exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) {
    errContext = ctx;
    activBody_->addConvolveOp(OpCodeEn::convolve);
}

void TreeShapeListener::exitRange(EGrammarParser::RangeContext* ctx) {
    errContext = ctx;
    activBody_->addRangeOp(ctx->expr().size());
}

void TreeShapeListener::exitShift(EGrammarParser::ShiftContext * ctx) {
    errContext = ctx;
    activBody_->addShiftOp();
}

void TreeShapeListener::exitDecimation(EGrammarParser::DecimationContext * ctx) {
    errContext = ctx;
    activBody_->addDecimationOp();
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
            activBody_->addCall(k);
            break;
        }
    }
    if (b){
        print_error("there are no functin with same name");
        activBody_->push(new Variable(ctx->getText(), TypeEn::float_jty));
    }
}

void TreeShapeListener::enterFunc(EGrammarParser::FuncContext* ctx) {
    errContext = ctx;
    NewBody(ctx->ID()->getText(), true);
    for (auto i : ctx->args()->ID()) {
        activBody_->addArg(i->getText());
    }
}

void TreeShapeListener::exitFunc(EGrammarParser::FuncContext* ctx) {
    if (activBody_->isRetStackEmpty())
        print_error("there are no returned value in func: " + activBody_->getName());
    activBody_ = context[0];
}

void TreeShapeListener::exitSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext * ctx){
    errContext = ctx;
    activBody_->addSmallArrayDefinitionOp(ctx->expr().size());
}


