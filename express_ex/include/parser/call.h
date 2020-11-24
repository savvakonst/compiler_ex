#ifndef CALL_H_
#define CALL_H_

class TailCallDirective:public Variable{
public:
    TailCallDirective(stack<Variable*> args):Variable(){
        args_ = args;
    }

    ~TailCallDirective(){

    }

    virtual void visitEnter(stack<Variable*>* visitorStack) override{
        visitorStack->push(this);
        for(int i= (args_.size() - 1); i >= 0; i--){
            visitorStack->push(args_[i]);
        }
        is_visited_ = true;
    };

    virtual NodeTypeEn getNodeType(){ return   NodeTypeEn::tailCall; }

private:
    stack<Variable*>  args_;
};




class CallRecursiveFunction: public Variable{
public:
    CallRecursiveFunction(Body* body, stack<Variable*> args ={}):Variable(){
        body_ = body;
        args_ = args;

        auto ret = body_->getRet().front();

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

    virtual void visitEnter(stack<Variable*>* visitorStack) override{
        visitorStack->push(this);
        for(int i= (args_.size() - 1); i >= 0; i--){
            visitorStack->push(args_[i]);
        }
        is_visited_ = true;
    };

    virtual NodeTypeEn getNodeType(){ return   NodeTypeEn::call; }
private:
    stack<Variable*>  args_;
    Body* body_ = nullptr;
};


class Call: public Variable{
public:
    Call(Body* body, stack<Variable*> args ={}): Variable(){
        body_ = body;
        args_ = args;

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
        // for (int i = (args.size() - 1); i >= 0; i--) {
        //    visitorStack->push(args[i]);
        //    //args[i]->setBufferLength(this);
        //}
        auto ret = body_->getRet().front();
        visitorStack->push(ret);
        ret->setBufferLength(this);

        is_nused_ = false;
    }

    /*
    virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack) override {
        visitorStack->push(this);
        for (int i= (args.size() - 1); i >= 0; i--) {
            visitorStack->push(args[i]);
        }
        is_visited = true;
    }
    */
    virtual void genBlocksVisitExit(TableGenContext* context) override{

        body_->genTable(context);
        uniqueName_ = (isLargeArr(this) ? "fb" : "fs") +
            std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited_ = false;
    };

    virtual void visitEnter(stack<Variable*>* visitorStack) override{
        visitorStack->push(this);
        for(int i = (args_.size() - 1); i >= 0; i--){
            visitorStack->push(args_[i]);
        }
        is_visited_ = true;
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

    virtual void printVisitExit(stack<std::string>* Stack) override{

        llvm::outs() << body_->print("    ") << "\n";

        for(auto& i : args_)
            auto x = Stack->pop();

        Stack->push(body_->getName() + ".ret." + toString(type_));
        is_visited_ = false;
    };

    

    virtual void reduceLinksVisitExit() override{ is_visited_ = false; }

    virtual string printUint(){

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

    virtual NodeTypeEn getNodeType(){ return   NodeTypeEn::call; }
    // virtual Variable*   getAssignedVal(bool deep = false)  override { return
    // this; }
private:
    stack<Variable*> args_;
    Body* body_ = nullptr;
};




#endif