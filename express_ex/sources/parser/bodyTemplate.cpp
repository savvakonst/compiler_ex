#include <string>
#include "parser/bodyTemplate.h"
#include "parser/body.h"

#include "parser/variable.h"
#include "parser/line.h"
#include "parser/operations.h"
#include "parser/call.h"
#include "parser/callTemplate.h"

BodyTemplate::BodyTemplate(std::string name, BodyTemplate* body )
    :name_(name)
{
    garbage_contaiiner_ = new GarbageContainer;
    lines_.reserve(30);
}

BodyTemplate::~BodyTemplate(){
    delete garbage_contaiiner_;
    /*
    for (auto& line : lines_) {
        delete line;
    }
    */
}

void BodyTemplate::addLine(const std::string& name, Value* var){
    auto line = new Line(name, var);
    garbage_contaiiner_->add(line);
    lines_.push_back(line);
}

void BodyTemplate::addArg(const std::string& name){
    auto line = new Line(name);
    garbage_contaiiner_->add(line);
    arg_count_++;
    lines_.push_back(line);
}

void BodyTemplate::addParam(Line* line){//?delete
    garbage_contaiiner_->add(line);
    arg_count_++;
    lines_.push_back(line);
}

void BodyTemplate::addParam(const std::string& name, TypeEn ty, DataStructureTypeEn dsty, uint64_t len){//?delete
    auto line = new Line(name, ty, dsty, len);
    garbage_contaiiner_->add(line);
    arg_count_++;
    lines_.push_back(line);
}

void BodyTemplate::addParam(const std::string& name, const std::string& linkName, DataStructureTypeEn dsty){
    auto line = new Line(name, linkName, dsty);
    garbage_contaiiner_->add(line);
    arg_count_++;
    lines_.push_back(line);
}

void BodyTemplate::addReturn(const std::string& name, Value* var){//?remove Value param
    auto line = new Line(name, var);

    if(is_tail_callable_){
        var = var->getAssignedVal(true);

        bool valid_recursion = var->getNodeType() == NodeTypeEn::kTailCallSelect;
        if(!valid_recursion)
            print_error("it isn't tail recursion");
    }

    garbage_contaiiner_->add(line);
    return_stack_.push_back(line);
}

//varStack push/pop 
void BodyTemplate::push(Value* line){
    garbage_contaiiner_->add(line);
    var_stack_.push_back(line);
}

Value* BodyTemplate::pop(){
    if(var_stack_.size() == 0)
        print_error("stack is empty");
    return var_stack_.pop();
}

stack<Value*> BodyTemplate::pop(size_t length){
    return var_stack_.pop(length);
}

std::map<std::string, std::string> BodyTemplate::getParameterLinkNames(bool hide_unused) const{

    std::map<std::string, std::string > ret;
    for(auto& value : lines_)
        if(value->isArg())
            if(!(hide_unused && value->isUnused()))
                ret[value->getName(true)]=value->getLinkName();
    return ret;

}

//create operation and push to varStack
void BodyTemplate::addTypeConvOp(TypeEn targetType){
    Value* arg1 = pop();
    push(newTypeConvOp(garbage_contaiiner_, targetType, arg1));
}

void BodyTemplate::addBuiltInFuncOp(OpCodeEn uTypeOp){
    Value* arg = pop();
    TypeEn targetType = TypeEn::DEFAULT_JTY;
    push(newBuiltInFuncOperation(garbage_contaiiner_, targetType, arg, uTypeOp));
}

void BodyTemplate::addInvOp(){
    Value* arg = pop();
    Value* zero = garbage_contaiiner_->add(new  Value("0", TypeEn::int32_jty));
    push(newArithmeticOperation(garbage_contaiiner_, TypeEn::DEFAULT_JTY, zero, arg, OpCodeEn::sub));
}

void BodyTemplate::addArithmeticOp(OpCodeEn uTypeOp){
    Value* arg_b = pop();
    Value* arg_a = pop();
    push(newArithmeticOperation(garbage_contaiiner_, TypeEn::DEFAULT_JTY, arg_a, arg_b, uTypeOp));
}

void BodyTemplate::addComarsionOp(OpCodeEn uTypeOp){
    Value* arg_b = pop();
    Value* arg_a = pop();
    push(newComparsionOperation(garbage_contaiiner_, TypeEn::DEFAULT_JTY, arg_a, arg_b, uTypeOp));
}


void BodyTemplate::addConvolveOp(OpCodeEn uTypeOp, uint32_t shift){//necessary to add type maching
    Value* arg_b = pop();
    Value* arg_a = pop();
    is_operator_ = true;
    push(newConvolveOperation(garbage_contaiiner_, TypeEn::DEFAULT_JTY, arg_a, arg_b, shift, uTypeOp));
}

void BodyTemplate::addSelectOp(){
    Value* arg_c = pop();
    Value* arg_b = pop();
    Value* arg_a = pop();

    bool valid_recursion = false;

    if(is_tail_callable_){
        const NodeTypeEn p =arg_c->getAssignedVal(true)->getNodeType();
        valid_recursion =  (p == NodeTypeEn::kTailCall);
        valid_recursion = valid_recursion || (arg_b->getAssignedVal(true)->getNodeType() == NodeTypeEn::kTailCall);
    }

    push(newSelectOp(garbage_contaiiner_, TypeEn::DEFAULT_JTY, arg_a, arg_b, arg_c, valid_recursion));
}

void BodyTemplate::addRangeOp(size_t argCount){
    if((argCount < 1) || (argCount > 3))
        print_error("invalid signature of range(..) function");

    stack<Value*> v=pop(argCount);
    push(newSmallArrayDefOp(garbage_contaiiner_, v, OpCodeEn::smallArrayRange));

}

void BodyTemplate::addShiftOp(){
    Value* arg2 = pop();
    Value* arg1 = pop();
    push(newSliceOp(garbage_contaiiner_, arg1, arg2, OpCodeEn::shift));
}

void BodyTemplate::addDecimationOp(){

    Value* arg2 = pop();
    Value* arg1 = pop();
    push(newSliceOp(garbage_contaiiner_, arg1, arg2, OpCodeEn::decimation));
}

void BodyTemplate::addSmallArrayDefinitionOp(size_t size){
    stack<Value* > op;
    is_operator_ = true;
    for(size_t i = 0; i < size; i++)
        op.push(pop());
    std::reverse(op.begin(), op.end());
    push(newSmallArrayDefOp(garbage_contaiiner_, op, OpCodeEn::smallArrayDef, true));
}

void BodyTemplate::addCall(BodyTemplate* body){
    stack<Value*> a;
    a.resize(body->getArgCount());
    for(int i = body->getArgCount() - 1; i >= 0; i--){
        a[i] = pop();
    }

    is_operator_ = is_operator_ || body->is_operator_;

    if(body->is_tail_callable_)
        push(garbage_contaiiner_->add(new CallRecursiveFunctionTemplate(body, a)));
    else
        push(garbage_contaiiner_->add(new CallTemplate(body, a)));
}

void BodyTemplate::addTailCall(){
    stack<Value*> a;
    a.resize(this->getArgCount());
    for(int i = this->getArgCount() - 1; i >= 0; i--){
        a[i] = pop();
    }

    if(is_tail_callable_)
        print_error("second recursive call");

    is_tail_callable_ = true;
    //new TailCallDirective(a);
    push(garbage_contaiiner_->add(new TailCallDirective(a)));
}



Line* BodyTemplate::getLastLineFromName(const std::string& name) const{

    if(lines_.size() < 1)
        return nullptr;
    for(size_t i = lines_.size() - 1; i >= 0; i--){
        if(lines_[i]->checkName(name))
            return (lines_[i]);
    }
    print_error("unknown symbol " + name);
    return nullptr;
}

BodyTemplate* BodyTemplate::getFunctionBody(const std::string& name) const{
    for(auto i : child_body_template_list_)
        if(i->getName() == name)
            return i;

    if (parent_body_template_)
        return parent_body_template_->getFunctionBody(name);
    return nullptr;
}


const stack<ParameterIfs*> BodyTemplate::getOutputParameterList()const{
    stack<ParameterIfs*> pList;
    for(auto i : return_stack_){
        pList.push(i->getAssignedVal(true)->getPatameter());
    }
    return pList;
}



// tree walker methods
std::string   BodyTemplate::print(std::string tab, bool DSTEna, bool hideUnusedLines) const{

    //hideUnusedLines =true;
    stack<Value*> visitorStack;
    stack<std::string> stringStack;

    const size_t max_line_length=90;

    std::string result = " " + getName() + "\n";
    std::string txt_line, txt_skip, txt_shifts;

    for(auto& value : lines_){
        if(value->isArg()){
    
            if(!hideUnusedLines || !value->isUnused()){
                txt_line     = value->getName() + "=arg()";
                txt_shifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
                txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
                result     += txt_line + txt_skip + txt_shifts + "\n";
            }
        }
        else{
            visitorStack.push(value->getAssignedVal());
            do{
                auto var = visitorStack.pop();
                if(var->isVisited())
                    var->printVisitExit(&stringStack);
                else
                    var->visitEnter(&visitorStack);
            } while(!visitorStack.empty());

            auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";

            if(!hideUnusedLines || !value->isUnused()){
                txt_line     = tab + value->getName() + DST_postfix + "=" + stringStack.pop();
                txt_shifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen()) + " : " + std::to_string(value->getLength());
                txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? max_line_length - 2 : txt_line.length()), ' ');
                result     += txt_line + txt_skip + txt_shifts + "\n";
            }
        }
    }

    for(auto& value : return_stack_){
        visitorStack.push(value->getAssignedVal());
        do{
            auto var = visitorStack.pop();
            if(var->isVisited())
                var->printVisitExit(&stringStack);
            else
                var->visitEnter(&visitorStack);
        } while(!visitorStack.empty());

        auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";
        
        txt_line = tab + value->getName() + DST_postfix + "  " + stringStack.pop();
        txt_shifts = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
        txt_skip = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
        result += txt_line + txt_skip + txt_shifts + "\n";
    }

    return result;
}

const std::list<std::string> BodyTemplate::getNamesOfDefinedFunctions()const{
    std::list<std::string> ret;
    for(auto i : child_body_template_list_){
        ret.push_back(i->getName());
    }
    return ret;
}

Body* BodyTemplate::genBodyByTemplate(Body * parent_body, stack<Value*> args, bool is_pure_function)const{

    // try replacing the "var_stack_" member with the "var_stack" local value
    // it might be worth moving this value to BodyGenContext


    Body* body = new Body(name_, getNamesOfDefinedFunctions(), parent_body, is_operator_);
    BodyGenContext* context = new BodyGenContext(body,is_pure_function);

    stack<Value*> visitor_stack;

    auto arg = args.begin();
    for(auto& value : lines_){
        if(value->isArg()){

            if(name_ == "main"){
                body->addParam((Line*)*(arg));
                arg++;
            }
            else{
                if(is_pure_function){
                    body->addVariableLine(value->getName(), *(arg));
                }
                else{
                    body->addLine(value->getName(), *(arg));// in line 
                }
                arg++;
            }
        }
        else{
            visitor_stack.push(value->getAssignedVal());
            do{
                auto var = visitor_stack.pop();
                if(var->isVisited())
                    var->genBodyVisitExit(context);
                else
                    var->visitEnter(&visitor_stack);
            } while(!visitor_stack.empty());
            body->addLine(value->getName(), context->pop());
        }
    }
    for(auto& value : return_stack_){
        visitor_stack.push(value->getAssignedVal());
        do{
            auto var = visitor_stack.pop();
            if(var->isVisited())
                var->genBodyVisitExit(context);
            else
                var->visitEnter(&visitor_stack);
        } while(!visitor_stack.empty());
        body->addReturn(return_stack_[0]->getName(), context->pop());
    }

    delete context;
    return body;
}

untyped_t BodyTemplate::genConstRecusiveByTemplate(stack<Value*>& args)const{

    stack<Value*> visitor_stack;
    stack<std::string> stringStack;


    auto context =new ConstRecursiveGenContext();

    std::vector<Value*> instructions_list;
    auto arg = args.begin();

    for(auto& line : lines_){
        if(line->isArg()){
            line->setTempTypeAndBinaryValue(*arg++);
            context->addArg(line->getBinaryValuePtr());

        }
        else{
            visitor_stack.push(line->getAssignedVal());
            do{
                auto var = visitor_stack.pop();
                if(var->isVisited())
                    var->genConstRecursiveVisitExit(context);
                else
                    var->visitEnter(&visitor_stack);
            } while(!visitor_stack.empty());
            line->genConstRecursiveVisitExit(context);
        }
    }

    for(auto& value : return_stack_){
        visitor_stack.push(value->getAssignedVal());
        do{
            auto var = visitor_stack.pop();
            if(var->isVisited())
                var->genConstRecursiveVisitExit(context);
            else
                var->visitEnter(&visitor_stack);
            value->genConstRecursiveVisitExit(context);
        } while(!visitor_stack.empty());
    }

    int32_t iteration_cnt = 0;

    for(int32_t index = 0; context->exitFromLoop(); iteration_cnt++){
        for(auto& instruction : context->instructions_list_)
            instruction->calculateConstRecursive(context);

        if(iteration_cnt == -1)
            print_error("recursion too deep");
    }

    // maybe it is not necessary
    untyped_t return_val = context->instructions_list_.back()->getBinaryValue();
    delete context;
    return return_val;
}



GarbageContainer::~GarbageContainer(){
    for(auto i : value_set_)
        delete i;
}