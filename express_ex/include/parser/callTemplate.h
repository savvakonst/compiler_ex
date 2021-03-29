#ifndef CALL_TEMPLATE_H_
#define CALL_TEMPLATE_H_

#include "bodyTemplate.h"
#include "body.h"

class CallTemplate_ifs: public Value{
public:

    CallTemplate_ifs():Value(){
    }
    ~CallTemplate_ifs(){
    }

    virtual void visitEnter(stack<Value*>* visitor_stack) override{
        visitor_stack->push(this);
        for(int64_t i = ((int64_t)args_.size() - 1);i >= 0; i--){
            visitor_stack->push(args_[(size_t)i]);
        }
        is_visited_ = true;
    };

    virtual void printVisitExit(PrintBodyContext* context) override{
        if(body_template_){

            context->addVoid(body_template_->print(context->tab_ + "  ", context->DST_ena_, context->hide_unused_lines_));

            for(auto& i : args_)
                auto x = context->pop();

            context->push(body_template_->getName() + ".ret." + toString(type_));
            is_visited_ = false;
        }
    };

    virtual string printUint() override{
        return unique_name_ + " = assignCallTemplate(" +
            body_template_->getRet().front()->getAssignedVal(true)->getUniqueName() + ")";
    }

    void genRecursiveVisitExit(RecursiveGenContext* context) override{
        context->setUint(this);
        is_visited_ = false;
    }

protected:
    BodyTemplate* body_template_ = nullptr;
    stack<Value*> args_;
};


class CallRecursiveFunctionTemplate: public CallTemplate_ifs{
public:

    CallRecursiveFunctionTemplate(BodyTemplate* body, const stack<Value*>& args ={}):CallTemplate_ifs(){
        body_template_ = body;
        args_ = args;

        if(body_template_->getRet().empty())
            return;
        auto ret = body_template_->getRet().front();

        level_ = ret->getLevel();
        type_ = ret->getType();
        data_structure_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_   = ret->getTextValue();
        }
    }

    ~CallRecursiveFunctionTemplate(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        bool large_array = false, small_array = false;

        for(auto i : args_){
            Value* var = context->pop();
            large_array |= isLargeArr(var);
            small_array |= isSmallArr(var);
            a.push(var);
        }


        if(large_array && small_array){
            print_error("invalid signature for recursive call method");
        }
        else if(large_array || small_array){
            
            Signature signature;
            for(auto& i : a) 
                signature.push_back(i->getType());

            Body* body = context->getPureFunctionBody(body_template_->getName(), signature);
            
            if(body == nullptr){
                body = body_template_->genBodyByTemplate(context->current_body_, a,true);
                context->setPureFunctionBody(body);
            }
            
            context->push(
                context->getGarbageContainer()->add(
                    new CallRecursiveFunction(body, a)
                ));
            
        }
        else{
            binary_value_ = body_template_->genConstRecusiveByTemplate(a);

            context->push(
                context->getGarbageContainer()->add(
                    new Value(binary_value_, body_template_->getRet().front()->getTempType())
                ));
        }

        is_visited_ = false;
    }

    virtual void calculateConstRecursive(RecursiveGenContext* context) override{
        binary_value_ = body_template_->genConstRecusiveByTemplate(args_); //it might be worth changing the order
        type_ = body_template_->getRet().front()->getTempType();
        is_visited_ = false;
    };

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::kCall; }
};


class TailCallDirectiveTemplate: public CallTemplate_ifs{
public:

    TailCallDirectiveTemplate(stack<Value*> args):CallTemplate_ifs(){
        args_ = args;
        type_ = TypeEn::unknown_jty;
    }

    ~TailCallDirectiveTemplate(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        for(auto& i : args_) a.push(context->pop());

        context->push(
            context->getGarbageContainer()->add(
                new TailCallDirective(a)));
        is_visited_ = false;
    }

    virtual void calculateConstRecursive(RecursiveGenContext* context) override{
        size_t size = context->args_reference_.size();
        for(size_t i = 0; i < size; i++)
            context->args_reference_[i]->binary_value_ = args_[i]->binary_value_;
    }

    virtual NodeTypeEn getNodeType() const override{ return   NodeTypeEn::kTailCall; }
};



class CallTemplate: public CallTemplate_ifs{
public:

    CallTemplate(BodyTemplate * body, const stack<Value*>& args ={}): CallTemplate_ifs(){
        body_template_ = body;
        args_ = args;

        if(body_template_->getRet().empty())
            return;
        auto ret = body_template_->getRet().front();

        level_ = ret->getLevel();
        type_ = ret->getType();
        data_structure_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_ = ret->getTextValue();
        }
    }

    ~CallTemplate(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        for(auto& i : args_)
            a.push(context->pop());

        Body* body = body_template_->genBodyByTemplate(context->current_body_, a, context->is_pure_function_);



        if(context->is_pure_function_){
            context->setPureFunctionBody(body);
            context->push(
                context->getGarbageContainer()->add(new CallRecursiveFunction(body, a)));
        }
        else{
            context->push(
                context->getGarbageContainer()->add(new Call(body, a)));
        }

        is_visited_ = false;
    }

    virtual void calculateConstRecursive(RecursiveGenContext* context) override{
        binary_value_ = body_template_->genConstRecusiveByTemplate(args_); //it might be worth changing the order
        type_ = body_template_->getRet().front()->getTempType();
        is_visited_ = false;
    };

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::kCall; }

};



#endif