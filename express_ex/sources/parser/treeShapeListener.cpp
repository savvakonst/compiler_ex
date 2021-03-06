#include "parser/treeShapeListener.h"
//#include "parser.h"

ParserRuleContext* g_err_context = nullptr;
PosInText g_pos;
std::string g_pos_file;


TreeShapeListener:: TreeShapeListener() : EGrammarBaseListener(){
    activ_body_ = new BodyTemplate("main", activ_body_);
}

TreeShapeListener::TreeShapeListener(BodyTemplate* body, const std::vector<BodyTemplate*> &context) : EGrammarBaseListener() {
    activ_body_ = body;
}

TreeShapeListener:: ~TreeShapeListener(){
}

void TreeShapeListener::setPos(ParserRuleContext* ctx) {
    g_err_context = ctx;
    g_pos.start_line = ctx->getStart()->getLine();
    g_pos.stop_line = ctx->getStop()->getLine();
    g_pos.start_char_pos = ctx->getStart()->getCharPositionInLine();
    g_pos.stop_char_pos = ctx->getStop()->getCharPositionInLine();
}

void TreeShapeListener::exitAssign(EGrammarParser::AssignContext* ctx)  {
    g_err_context = ctx;
    activ_body_->addLine(ctx->ID()->getText(), activ_body_->pop());
}

void TreeShapeListener::exitAssignParam(EGrammarParser::AssignParamContext* ctx) {
    g_err_context = ctx;
    auto id = ctx->ID();
    auto stl = ctx->STRINGLITERAL();

    if (stl.size() == 0) 
        for (auto i : id) 
            activ_body_->addParam(i->getText(),"", DataStructureTypeEn::kLargeArr);
    
    else if (id.size() == stl.size()) 
        for (int i = 0; i < id.size(); i++) {
            //activ_body_->addParam(id[i]->getText(), TypeEn::double_jty, DataStructureTypeEn::kLargeArr, stoi(stl[i]->getText().substr(1)));
            std::string s = stl[i]->getText();
            activ_body_->addParam(id[i]->getText(), s.substr(1, s.length()-2), DataStructureTypeEn::kLargeArr);
        }
    else
        print_error("there are invalid signature ");
}

void TreeShapeListener::exitAssignRetParam(EGrammarParser::AssignRetParamContext* ctx)  {
    setPos(ctx);
    for (auto k : ctx->expr())
        if (activ_body_->isRetStackFull())
            print_error("there are more then one returned value");
        else
            activ_body_->addReturn("return", activ_body_->pop());
}

void TreeShapeListener::enterId(EGrammarParser::IdContext* ctx) {
    setPos(ctx);
    activ_body_->push(activ_body_->getLastLineFromName(ctx->ID()->getText()));
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
    activ_body_->push(new Value(ctx->getText(), targetType));
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
    activ_body_->addTypeConvOp(targetType);
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
    activ_body_->addBuiltInFuncOp(op);
}

void TreeShapeListener::exitInv(EGrammarParser::InvContext * ctx)
{
    activ_body_->addInvOp();
}

//arithmetic operations
void TreeShapeListener::exitMulDiv(EGrammarParser::MulDivContext* ctx) {
    setPos(ctx);
    OpCodeEn op=(EGrammarParser::MUL == ctx->op->getType()) ? OpCodeEn::mul : OpCodeEn::sdiv;
    activ_body_->addArithmeticOp(op);
}

void TreeShapeListener::exitAddSub(EGrammarParser::AddSubContext* ctx) {
    setPos(ctx);
    OpCodeEn op = (EGrammarParser::ADD == ctx->op->getType()) ? OpCodeEn::add : OpCodeEn::sub;
    activ_body_->addArithmeticOp(op);
}

void TreeShapeListener::exitPow(EGrammarParser::PowContext* ctx) {
    setPos(ctx);
    activ_body_->addArithmeticOp(OpCodeEn::pow);
}

void TreeShapeListener::exitMoreLess(EGrammarParser::MoreLessContext *ctx)  {
    OpCodeEn op = (EGrammarParser::MORE_ == ctx->op->getType()) ? OpCodeEn::sgt : OpCodeEn::slt;
    activ_body_->addComarsionOp(op);
}

void TreeShapeListener::exitMoreeqLesseq(EGrammarParser::MoreeqLesseqContext *ctx) {
    OpCodeEn op = (EGrammarParser::MOREEQ == ctx->op->getType()) ? OpCodeEn::sge : OpCodeEn::sle;
    activ_body_->addComarsionOp(op);
}

void TreeShapeListener::exitEquality(EGrammarParser::EqualityContext  *ctx)
{
    OpCodeEn op = (EGrammarParser::EQ == ctx->op->getType()) ? OpCodeEn::eq : OpCodeEn::ne;
    activ_body_->addComarsionOp(op);
}

//conditional 
void TreeShapeListener::exitCondExpr(EGrammarParser::CondExprContext* ctx) {
    setPos(ctx);
    activ_body_->addSelectOp();
}

void TreeShapeListener::exitCallConvolve(EGrammarParser::CallConvolveContext* ctx) {
    setPos(ctx);
    activ_body_->addConvolveOp(OpCodeEn::convolve);
}

void TreeShapeListener::exitRange(EGrammarParser::RangeContext* ctx) {
    setPos(ctx);
    activ_body_->addRangeOp(ctx->expr().size());
}

void TreeShapeListener::exitShift(EGrammarParser::ShiftContext * ctx) {
    setPos(ctx);
    activ_body_->addShiftOp();
}

void TreeShapeListener::exitDecimation(EGrammarParser::DecimationContext * ctx) {
    setPos(ctx);
    activ_body_->addDecimationOp();
}

//call function
void TreeShapeListener::exitCallFunc(EGrammarParser::CallFuncContext* ctx) {
    setPos(ctx);
    auto exprs = ctx->expr();
    const std::string function_name = ctx->ID()->getText();
    BodyTemplate* called_body = activ_body_->getFunctionBody(function_name);
    if(called_body){
        if(called_body->getArgCount() != exprs.size())
            print_error("there are invalid signature call in function: " + function_name + " ");

        if(activ_body_->getName() == function_name)
            activ_body_->addTailCall();  
        else
            activ_body_->addCall(called_body);
    }
    else
        print_error("there are no functin with same name");
}

void TreeShapeListener::enterFunc(EGrammarParser::FuncContext* ctx) {
    setPos(ctx);

    auto body = new BodyTemplate(ctx->ID()->getText(), activ_body_);
    if(activ_body_){
        activ_body_->child_body_template_list_.push_back(body);
    }
    activ_body_ = body;

    for (auto i : ctx->args()->ID()) {
        activ_body_->addArg(i->getText());
    }
}

void TreeShapeListener::exitFunc(EGrammarParser::FuncContext* ctx) {
    if (activ_body_->isRetStackEmpty())
        print_error("there are no returned value in func: " + activ_body_->getName());
    activ_body_ = activ_body_->getParent();
}

void TreeShapeListener::exitSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext * ctx){
    setPos(ctx);
    activ_body_->addSmallArrayDefinitionOp(ctx->expr().size());
}

BodyTemplate* TreeShapeListener::getMainBody()
{
    BodyTemplate* next_parent = activ_body_ ,*parent;
    do{
        parent = next_parent;
        next_parent = next_parent->getParent();
    } while(next_parent);
    return parent;
}


