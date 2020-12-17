#ifndef CALL_H_
#define CALL_H_
#include "variable.h"
#include "line.h"
#include "body.h"
#include "basic.h"


namespace llvm{
    class Function;
}
class IRGenerator;


class CallI_ifs: public Value{
public:
    CallI_ifs ():Value(){
    }
    ~CallI_ifs(){
    }

    virtual void visitEnter(stack<Value*>* visitor_stack) override{
        visitor_stack->push(this);
        for(int64_t i = ((int64_t)args_.size() - 1);i >= 0; i--){
            visitor_stack->push(args_[i]);
        }
        is_visited_ = true;
    };


    virtual void printVisitExit(PrintBodyContext* context) override{
        if(body_){
            context->addVoid(body_->print(context->tab_ + "  ", context->DST_ena_, context->hide_unused_lines_));

            for(auto& i : args_)
                auto x = context->pop();

            context->push(body_->getName() + ".ret." + toString(type_));
            is_visited_ = false;
        }
    };

    virtual Value* getAssignedVal(bool deep = false) override{
        if(body_ == nullptr)
            return nullptr;

        if(is_buffered & deep){
            body_->getRet().front()->getAssignedVal(true)->setBuffered();
        }

        return body_->getRet().front()->getAssignedVal(deep);
    }

    virtual void genBodyVisitExit(BodyGenContext* context) override{
        print_error("genBodyVisitExit is not supported for CallI_ifs");
    }


    virtual void genRecursiveVisitExit(RecursiveGenContext* context) override {
        context->setUint(this);
        is_visited_ = false;
    }

protected:
    Body* body_ = nullptr;
    stack<Value*>  args_;
};


class Call: public CallI_ifs{
public:

    Call(Body* body, const stack<Value*>& args ={});
    ~Call(){}


    virtual void markUnusedVisitEnter(stack<Value*>* visitor_stack) override;
    virtual void genBlocksVisitExit(TableGenContext* context) override;
    virtual void setupIR(IRGenerator& builder) override;


    virtual NodeTypeEn getNodeType()const override{ return NodeTypeEn::kCall; }

};


class CallRecursiveFunction: public CallI_ifs{
public:

    CallRecursiveFunction(Body* body, const stack<Value*>& args ={});
    ~CallRecursiveFunction(){}


    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack);
    virtual void genBlocksVisitExit(TableGenContext* context) override;
    virtual void setupIR(IRGenerator& builder) override;

    virtual Value* getAssignedVal(bool deep = false) override{ return this; }
    virtual NodeTypeEn getNodeType()const override{ return NodeTypeEn::kCall; }
};


class TailCallDirective: public CallI_ifs{
public:

    TailCallDirective(stack<Value*> args)
        :CallI_ifs(){
        type_ = TypeEn::unknown_jty;
        args_ = (args);
    }
    ~TailCallDirective(){}


    virtual void setupIR(IRGenerator& builder) override;

    virtual Value* getAssignedVal(bool deep = false) override{ return this; }
    virtual NodeTypeEn getNodeType() const override{ return NodeTypeEn::kTailCall; }
};


#endif