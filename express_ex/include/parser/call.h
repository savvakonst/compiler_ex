#ifndef CALL_H_
#define CALL_H_


class CallI_ifs: public Variable{
public:
    CallI_ifs ():Variable(){
    }

    virtual void visitEnter(stack<Variable*>* visitorStack) override{
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

protected:
    Body* body_ = nullptr;
    stack<Variable*>  args_;
};



class TailCallDirective: public CallI_ifs{
public:
    TailCallDirective(stack<Variable*> args):CallI_ifs(){
        args_ = args;
        type_ = TypeEn::unknown_jty;
    }

    ~TailCallDirective(){
    }

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Variable*> a;
        for(auto& i : args_) a.push(context->pop());

        context->push(
            context->getGarbageContainer()->add(
                new TailCallDirective(a)));
        is_visited_ = false;
    }

    virtual NodeTypeEn getNodeType() const override { return   NodeTypeEn::tailCall; }
};



class CallRecursiveFunction: public CallI_ifs{
public:

    CallRecursiveFunction(Body* body,const stack<Variable*> &args ={}):CallI_ifs(){
        body_ = body;
        args_ = args;

        if(body_->getRet().empty())
            return;
        auto ret = body_->getRet().front() ;

        level_ = ret->getLevel();
        type_ = ret->getType();
        dsType_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_   = ret->getTextValue();
        }
    }

    ~CallRecursiveFunction(){}

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Variable*> a;
        for(auto& i : args_)
            a.push(context->pop());

        Body* b = body_;

        if(!context->isPrototype())
            b = body_->genBodyByPrototype(a, false);

        auto call = new CallRecursiveFunction(b, a);
        context->getGarbageContainer()->add(call);
        context->push(call);
        is_visited_ = false;
    }

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::call; }

};


class Call: public CallI_ifs{
public:

    Call(Body* body, const stack<Variable*>& args ={}): CallI_ifs(){
        body_ = body;
        args_ = args;

        if(body_->getRet().empty())
            return;
        auto ret = body_->getRet().front();

        level_ = ret->getLevel();
        type_ = ret->getType();
        dsType_ = ret->getDSType();
        length_ = ret->getLength();

        if(isConst(ret)){
            binary_value_ = ret->getBinaryValue();
            text_value_ = ret->getTextValue();
        }
    }

    ~Call(){}

    // safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override{
        commoMmarkUnusedVisitEnter(visitorStack);

        if(body_->getRet().empty())
            print_error("markUnusedVisitEnter: body_->getRet().empty() == true ");
            
        auto ret = body_->getRet().front();

        visitorStack->push(ret);
        ret->setBufferLength(this);

        is_nused_ = false;
    }

    virtual void genBlocksVisitExit(TableGenContext* context) override{

        body_->genTable(context);
        uniqueName_ = (isLargeArr(this) ? "fb" : "fs") +
            std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited_ = false;
    };

    virtual void genBodyVisitExit(BodyGenContext* context) override{

        stack<Variable*> a;
        for(auto& i : args_)
            a.push(context->pop());

        Body* b = body_;

        if(!context->isPrototype())
            b = body_->genBodyByPrototype(a, false);

        auto call = new Call(b, a);
        context->getGarbageContainer()->add(call);
        context->push(call);
        is_visited_ = false;
    }

    virtual string printUint() override {
 
            return uniqueName_ + " = assignCall(" +
            body_->getRet()[0]->getAssignedVal(true)->getUniqueName() + ")";
    }

    virtual void setupIR(IRGenerator& builder) override;

    virtual Variable* getAssignedVal(bool deep = false) override{

        if(is_buffered & deep){
            body_->getRet()[0]->getAssignedVal(true)->setBuffered();
        }

        return body_->getRet()[0]->getAssignedVal(deep);
    }

    virtual NodeTypeEn getNodeType()const override{ return   NodeTypeEn::call; }

};

#endif