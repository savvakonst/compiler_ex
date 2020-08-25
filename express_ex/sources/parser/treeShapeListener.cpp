#include "treeShapeListener.h"
//#include "parser.h"

ParserRuleContext* g_err_context = nullptr;
PosInText g_pos;

TreeShapeListener:: TreeShapeListener() : EGrammarBaseListener(){
    NewBody("main", true);
}

TreeShapeListener::~TreeShapeListener(){
    for (auto g : context) {
        delete g;
    }
}

void TreeShapeListener::setPos(ParserRuleContext* ctx) {
    g_err_context = ctx;
    g_pos.start_line = ctx->getStart()->getLine();
    g_pos.stop_line = ctx->getStop()->getLine();
    g_pos.start_char_pos = ctx->getStart()->getCharPositionInLine();
    g_pos.stop_char_pos = ctx->getStop()->getCharPositionInLine();
}


void TreeShapeListener::NewBody(std::string name, bool isPrototype) {
    activBody_ = new Body(name, isPrototype);
    context.push_back(activBody_);
}

void TreeShapeListener::exitAssign(EGrammarParser::AssignContext* ctx)  {
    g_err_context = ctx;
    //g_pos.len=g_err_context
    activBody_->addLine(ctx->ID()->getText(), activBody_->pop());
}

void TreeShapeListener::exitAssignParam(EGrammarParser::AssignParamContext* ctx) {
    g_err_context = ctx;
    auto id = ctx->ID();
    auto stl = ctx->STRINGLITERAL();

    if (stl.size() == 0) 
        for (auto i : id) 
            activBody_->addParam(i->getText(), TypeEn::double_jty, DataStructTypeEn::largeArr_dsty, 100000);
    
    else if (id.size() == stl.size()) 
        for (int i = 0; i < id.size(); i++) {
            //activBody_->addParam(id[i]->getText(), TypeEn::double_jty, DataStructTypeEn::largeArr_dsty, stoi(stl[i]->getText().substr(1)));
            std::string s = stl[i]->getText();
            activBody_->addParam(id[i]->getText(), s.substr(1, s.length()-2), DataStructTypeEn::largeArr_dsty);
        }
    
    else
        print_error("there are invalid signature ");
}

void TreeShapeListener::exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx)  {
    setPos(ctx);
    for (auto k : ctx->expr())
        if (activBody_->isRetStackFull())
            print_error("there are more then one returned value");
        else
            activBody_->addReturn("return", activBody_->pop());
}

void TreeShapeListener::enterId(EGrammarParser::IdContext* ctx) {
    setPos(ctx);
    activBody_->push(activBody_->getLastLineFromName(ctx->ID()->getText()));
}

void TreeShapeListener::exitConst(EGrammarParser::ConstContext* ctx)  {
    setPos(ctx);
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
    setPos(ctx);
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
    setPos(ctx);
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
    setPos(ctx);
    OpCodeEn op=(EGrammarParser::MUL == ctx->op->getType()) ? OpCodeEn::mul : OpCodeEn::sdiv;
    activBody_->addArithmeticOp(op);
}

void TreeShapeListener::exitAddSub(EGrammarParser::AddSubContext* ctx) {
    setPos(ctx);
    OpCodeEn op = (EGrammarParser::ADD == ctx->op->getType()) ? OpCodeEn::add : OpCodeEn::sub;
    activBody_->addArithmeticOp(op);
}

void TreeShapeListener::exitPow(EGrammarParser::PowContext* ctx) {
    setPos(ctx);
    activBody_->addArithmeticOp(OpCodeEn::pow);
}

void TreeShapeListener::exitMoreLess(EGrammarParser::MoreLessContext *ctx)  {
    OpCodeEn op = (EGrammarParser::MORE_ == ctx->op->getType()) ? OpCodeEn::sgt : OpCodeEn::slt;
    activBody_->addComarsionOp(op);
}

void TreeShapeListener::exitMoreeqLesseq(EGrammarParser::MoreeqLesseqContext *ctx) {
    OpCodeEn op = (EGrammarParser::MOREEQ == ctx->op->getType()) ? OpCodeEn::sge : OpCodeEn::sle;
    activBody_->addComarsionOp(op);
}

void TreeShapeListener::exitEquality(EGrammarParser::EqualityContext  *ctx)
{
    OpCodeEn op = (EGrammarParser::EQ == ctx->op->getType()) ? OpCodeEn::eq : OpCodeEn::ne;
    activBody_->addComarsionOp(op);
}

//conditional 
void TreeShapeListener::exitCondExpr(EGrammarParser::CondExprContext* ctx) {
    setPos(ctx);
    activBody_->addSelectOp();
}

void TreeShapeListener::exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) {
    setPos(ctx);
    activBody_->addConvolveOp(OpCodeEn::convolve);
}

void TreeShapeListener::exitRange(EGrammarParser::RangeContext* ctx) {
    setPos(ctx);
    activBody_->addRangeOp(ctx->expr().size());
}

void TreeShapeListener::exitShift(EGrammarParser::ShiftContext * ctx) {
    setPos(ctx);
    activBody_->addShiftOp();
}

void TreeShapeListener::exitDecimation(EGrammarParser::DecimationContext * ctx) {
    setPos(ctx);
    activBody_->addDecimationOp();
}

//call function
void TreeShapeListener::exitCallFunc(EGrammarParser::CallFuncContext* ctx) {
    setPos(ctx);

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
    setPos(ctx);
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
    setPos(ctx);
    activBody_->addSmallArrayDefinitionOp(ctx->expr().size());
}


