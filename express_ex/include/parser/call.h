#ifndef CALL_H_
#define CALL_H_


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

    virtual void printVisitExit(stack<std::string>* exit_stack) override{
        if(body_){
            llvm::outs() << body_->print("    ") << "\n";

            for(auto& i : args_)
                auto x = exit_stack->pop();

            exit_stack->push(body_->getName() + ".ret." + toString(type_));
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


    virtual void genConstRecursiveVisitExit(ConstRecursiveGenContext* context) override {
        context->setUint(this);
        is_visited_ = false;
    }

protected:
    Body* body_ = nullptr;
    stack<Value*>  args_;
};


class CallRecursiveFunction: public CallI_ifs{
public:

    CallRecursiveFunction(Body* body,const stack<Value*> &args ={}):CallI_ifs(){
        body_ = body;
        args_ = args;

        if(body_->getRet().empty())
            return;
        auto ret = body_->getRet().front() ;

        level_ = ret->getLevel();
        type_ = ret->getType();
        data_structure_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_   = ret->getTextValue();
        }
    }

    ~CallRecursiveFunction(){}

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::kCall; }
};


class TailCallDirective: public CallI_ifs{
public:

    TailCallDirective(stack<Value*> args):CallI_ifs(){
        args_ = args;
        type_ = TypeEn::unknown_jty;
    }

    ~TailCallDirective(){}

    virtual NodeTypeEn getNodeType() const override{return   NodeTypeEn::kTailCall; }
};

class Call: public CallI_ifs{
public:

    Call(Body* body, const stack<Value*>& args ={}): CallI_ifs(){
        body_ = body;
        args_ = args;

        if(body_->getRet().empty())
            return;
        auto ret = body_->getRet().front();

        level_ = ret->getLevel();
        type_ = ret->getType();
        data_structure_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_ = ret->getTextValue();
        }
    }

    ~Call(){}

    // safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Value*>* visitor_stack) override{
        commoMmarkUnusedVisitEnter(visitor_stack);

        if(body_->getRet().empty())
            print_error("markUnusedVisitEnter: body_->getRet().empty() == true ");
            
        auto ret = body_->getRet().front();

        visitor_stack->push(ret);
        ret->setBufferLength(this);

        is_unused_ = false;
    }

    virtual void genBlocksVisitExit(TableGenContext* context) override{

        body_->genTable(context);
        unique_name_ = (isLargeArr(this) ? "fb" : "fs") +
            std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited_ = false;
    };


    virtual void setupIR(IRGenerator& builder) override;
    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::kCall; }

};

#endif