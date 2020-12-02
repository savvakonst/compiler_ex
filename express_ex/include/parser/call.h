#ifndef CALL_H_
#define CALL_H_


class CallI_ifs: public Value{
public:
    CallI_ifs ():Value(){
    }

    virtual void visitEnter(stack<Value*>* visitorStack) override{
        visitorStack->push(this);
        for(size_t i= (args_.size() - 1); i >= 0; i--){
            visitorStack->push(args_[i]);
        }
        is_visited_ = true;
    };

    virtual void printVisitExit(stack<std::string>* Stack) override{
        if(body_){
            llvm::outs() << body_->print("    ") << "\n";

            for(auto& i : args_)
                auto x = Stack->pop();

            Stack->push(body_->getName() + ".ret." + toString(type_));
            is_visited_ = false;
        }
    };

    virtual Value* getAssignedVal(bool deep = false) override{
        if(body_ == nullptr)
            return nullptr;

        if(is_buffered & deep){
            body_->getRet()[0]->getAssignedVal(true)->setBuffered();
        }

        return body_->getRet()[0]->getAssignedVal(deep);
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
        ds_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_   = ret->getTextValue();
        }
    }

    ~CallRecursiveFunction(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        bool large_array = false, small_array = false;

        for(auto i : args_){
            Value* var = context->pop();
            large_array |= isLargeArr(var);
            small_array |= isSmallArr(var);
            a.push(var);
        }


        Body* b = body_;
        if(large_array && small_array){
            print_error("invalid signature for recursive call method");
        }
        else if(large_array || small_array){
            if(!context->isPrototype())
                b = body_->genBodyByPrototype(a, false);

            context->push(
                context->getGarbageContainer()->add(
                    new CallRecursiveFunction(b, a)
                ));
            print_error("invalid signature for recursive call method");
        }
        else{
            binary_value_ = b->genConstRecusiveByPrototype(a);
            context->push(
                context->getGarbageContainer()->add(
                    new Value(binary_value_, b->getRet().front()->getType())
                ));
        }
        
        is_visited_ = false;
    }

    virtual void calculateConstRecursive(ConstRecursiveGenContext* context) override{
        binary_value_ = body_->genConstRecusiveByPrototype(args_); //it might be worth changing the order
        type_ = body_->getRet().front()->getType();
        is_visited_ = false;
    };


    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::call; }

};


class TailCallDirective: public CallI_ifs{
public:
    TailCallDirective(stack<Value*> args):CallI_ifs(){
        args_ = args;
        type_ = TypeEn::unknown_jty;
    }

    ~TailCallDirective(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        for(auto& i : args_) a.push(context->pop());

        context->push(
            context->getGarbageContainer()->add(
                new TailCallDirective(a)));
        is_visited_ = false;
    }


    virtual void calculateConstRecursive(ConstRecursiveGenContext* context) override{
        size_t size = context->args_reference_.size();
        for(size_t i = 0; i < size; i++)
            args_[i]->binary_value_ = *(context->args_reference_[i]);
    }



    virtual NodeTypeEn getNodeType() const override{
        return   NodeTypeEn::tailCall;
    }
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
        ds_type_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_ = ret->getTextValue();
        }
    }

    ~Call(){}

    // safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack) override{
        commoMmarkUnusedVisitEnter(visitorStack);

        if(body_->getRet().empty())
            print_error("markUnusedVisitEnter: body_->getRet().empty() == true ");
            
        auto ret = body_->getRet().front();

        visitorStack->push(ret);
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

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Value*> a;
        for(auto& i : args_)
            a.push(context->pop());

        Body* b = body_;

        if(!context->isPrototype())
            b = body_->genBodyByPrototype(a, false);

        context->push(
            context->getGarbageContainer()->add(new Call(b, a)));

        is_visited_ = false;
    }

    virtual string printUint() override{
        return unique_name_ + " = assignCall(" +
            body_->getRet()[0]->getAssignedVal(true)->getUniqueName() + ")";
    }

    virtual void setupIR(IRGenerator& builder) override;

    virtual void calculateConstRecursive(ConstRecursiveGenContext* context) override{
        binary_value_ = body_->genConstRecusiveByPrototype(args_); //it might be worth changing the order
        type_ = body_->getRet().front()->getType();
        is_visited_ = false;
    };

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::call; }

};

#endif