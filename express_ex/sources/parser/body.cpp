#include <string>
#include <list>
#include "parser/bodyTemplate.h"
#include "parser/body.h"
#include "parser/variable.h"
#include "parser/line.h"
#include "parser/operations.h"
#include "parser/call.h"


#include "parser/defWarningIgnore.h"
#include "llvm/Support/raw_ostream.h"
#include "parser/undefWarningIgnore.h"


Body::Body(const std::string& name, const std::list<std::string>& names_of_defined_functions,  Body * parent, bool is_operator)
    :declarated_pure_functions_map_(names_of_defined_functions), 
    parent_body_(parent),
    is_operator_(is_operator),
    name_(name)
{
    garbage_contaiiner_ = new GarbageContainer;
    lines_.reserve(30);
}

Body::~Body(){
    delete garbage_contaiiner_; 
}

//varStack push/pop 
void Body::push(Value* line){
    garbage_contaiiner_->add(line);
    var_stack_.push_back(line);
}

Value* Body::pop(){
    if (var_stack_.size() == 0) 
        print_error("stack is empty");
    return var_stack_.pop();
}

stack<Value*> Body::pop(size_t length){
    return var_stack_.pop(length);
}


void Body::addLine(const std::string& name, Value* var){
    auto line = new Line(name, var);
    garbage_contaiiner_->add(line);
    lines_.push_back(line);
}

void Body::addVariableLine(const std::string& name, Value* var){
    Line* line = new Line(name, var->getType(), DataStructureTypeEn::kVariable, 1);
    garbage_contaiiner_->add(line);
    lines_.push_back(line);
}

void Body::addParam(Line* line){//?delete
    garbage_contaiiner_->add(line);
    arg_count_++;
    lines_.push_back(line);
}

void Body::addReturn(const std::string& name, Value* var){//?remove Value param
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



void Body::setPureFunctionBody(Body* body){
    if(!declarated_pure_functions_map_.setPureFunctionBody(body)){
        if(parent_body_)
            parent_body_->setPureFunctionBody(body);
    }
}

std::map<std::string, std::string> Body::getParameterLinkNames(bool hide_unused) const{

    std::map<std::string, std::string > ret;
    for(auto& value : lines_)
        if(value->isArg())
            if(!(hide_unused && value->isUnused()))
                ret[value->getName(true)]=value->getLinkName();
    return ret;

}

Line* Body::getLastLineFromName(std::string name) const{

    if (lines_.size() < 1)
        return nullptr;

    for (int i = (int)lines_.size() - 1; i >= 0; i--) {
        if (lines_[i]->checkName(name))
            return (lines_[i]);
    }
    print_error("unknown symbol " + name);
    return nullptr;
}

const stack<ParameterIfs*> Body::getOutputParameterList()const{
    stack<ParameterIfs*> p_list;
    for (auto i : return_stack_) {
        p_list.push(i->getAssignedVal(true)->getPatameter());
    }
    return p_list;
}

const Signature Body::getSignature() const{
    Signature ret;
    for(auto& arg : lines_)
        if(arg->isArg())
        ret.push_back(arg->getType());
    return ret;
}

Body* Body::getPureFunctionBody(const std::string& name, const Signature& signature) const{
    Body* body = declarated_pure_functions_map_.getPureFunctionBody(name, signature);
    if(body)
        return body;

    if (parent_body_)
        return parent_body_->getPureFunctionBody(name, signature);
    return nullptr;
}



// tree walker methods
std::string   Body::print(std::string tab, bool DSTEna, bool hideUnusedLines){

    //hideUnusedLines =true;
    stack<Value*> visitor_stack;
    stack<std::string> string_stack;

    const size_t max_line_length=90;
    
    std::string result = " " + getName() + "\n";
    std::string txt_line, txt_skip, txt_shifts;

    for (auto& value : lines_) {
        if (value->isArg()) {
            //std::string postfix = (!hideUnusedLines || !line->isUnused()) ? "" : " \t\t#unused";
            if (!hideUnusedLines || !value->isUnused()) {
                txt_line     = value->getName() + "=arg()" ;
                txt_shifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
                txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
                result     += txt_line + txt_skip + txt_shifts + "\n";
            }
        }
        else {
            visitor_stack.push(value->getAssignedVal());
            do {
                auto var = visitor_stack.pop();
                if (var->isVisited())
                    var->printVisitExit(&string_stack);
                else
                    var->visitEnter(&visitor_stack);
            } while (!visitor_stack.empty());

            auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";
            //std::cout << tab << line->getName()+"."+ line->getTxtDSType()<< "=" << stringStack.pop()  << "\n";
            if (!hideUnusedLines || !value->isUnused()){
                txt_line     = tab + value->getName() + DST_postfix + "=" + string_stack.pop() ;
                txt_shifts   = std::to_string(value->getLeftBufferLen())+" : "+ std::to_string(value->getRightBufferLen()) + " : " + std::to_string(value->getLength());
                txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? max_line_length-2 : txt_line.length()), ' ');
                result     += txt_line + txt_skip + txt_shifts + "\n";
            }
        }
    }
    
    for (auto& value : return_stack_) {
        visitor_stack.push(value->getAssignedVal());
        do {
            auto var = visitor_stack.pop();
            if (var->isVisited())
                var->printVisitExit(&string_stack);
            else
                var->visitEnter(&visitor_stack);
        } while (!visitor_stack.empty());

        auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";

        txt_line = tab + value->getName() + DST_postfix + "  " + string_stack.pop();
        txt_shifts = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
        txt_skip = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
        result += txt_line + txt_skip + txt_shifts + "\n";
    }

    return   result ;
}



void Body::symplyfy(){

    stack<Value*> visitorStack;
    for (auto& value : return_stack_) {
        visitorStack.push(value->getAssignedVal());
        do {
            auto var = visitorStack.pop();
            var->markUnusedVisitEnter(&visitorStack);

        } while (!visitorStack.empty());
    }
}


void Body::genTable(TableGenContext * context){

    stack<Value*>  visitor_stack;

    if (name_ == "main")
        for (auto& value : return_stack_) 
            value->getAssignedVal(true)->setReturned();

    for (auto& value : lines_) {
        if (value->isArg() && (!value->isUnused())) {
            context->setUint(value);
        }
        else if (!value->isUnused()) {

            visitor_stack.push(value->getAssignedVal());
            do {
                auto var = visitor_stack.pop();
                if (var->isVisited())
                    var->genBlocksVisitExit(context);
                else
                    var->visitEnter(&visitor_stack);
            } while (!visitor_stack.empty());

        }
    }
    int64_t maxBufferLength = 0;
    for (auto& value : return_stack_) {

        visitor_stack.push(value->getAssignedVal());

        do {
            auto var = visitor_stack.pop();
            if (var->isVisited())
                var->genBlocksVisitExit(context);
            else
                var->visitEnter(&visitor_stack);
        } while (!visitor_stack.empty());


        auto temp= value->getLength();
        if (maxBufferLength< temp)
            maxBufferLength = temp;
    }

    if (name_ == "main")
        context->setMaxBufferLength(maxBufferLength);
}


Body* DeclaratedBodysMap::getPureFunctionBody(const std::string& name, const Signature & signature) const{
    auto a = find(name);
    if(a == end())
        return nullptr;

    for(auto i : a->second){
        if(signature == i->getSignature())
            return i;
    }
    return nullptr;
}

bool DeclaratedBodysMap::setPureFunctionBody(Body* body){
    
    auto a = find(body->getName());
    if(a == end())
        return false;
    a->second.push_back(body);
    return true;
}
