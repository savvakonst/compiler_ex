#ifndef CALL_TEMPLATE_H_
#define CALL_TEMPLATE_H_

#include "bodyTemplate.h"


class CallITemplate: public Value{
public:

    CallITemplate():Value(){
    }
    ~CallITemplate(){
    }

    virtual void visitEnter(stack<Value*>* visitor_stack) override{
        visitor_stack->push(this);
        for(int64_t i = ((int64_t)args_.size() - 1);i >= 0; i--){
            visitor_stack->push(args_[i]);
        }
        is_visited_ = true;
    };

    virtual void printVisitExit(stack<std::string>* exit_stack) override{
        if(body_){
            llvm::outs() << body_->print("    ") << "\n";

            for(auto& i : args_)
                auto x = exit_stack->pop();

            exit_stack->push(body_->getName() + ".ret." + toString(type_));
            is_visited_ = false;
        }
    };

protected:
    BodyTemplate* body_ = nullptr;
    stack<Value*>  args_;
};


#endif