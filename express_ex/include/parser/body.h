#ifndef BODY_H
#define BODY_H

#include <iostream>
#include <vector>
#include "llvm/Support/raw_ostream.h"
#include "variable.h"
#include "line.h"
#include "operations.h"


class Body{
public:

    Body(std::string name = "main", bool isPrototype = false);
    ~Body();


    bool    isRetStackFull () {if (name_ != "main") return 0 < returnStack_.size(); else return false;}
    bool    isRetStackEmpty() {return 0 == returnStack_.size();}

    void addLine    (const std::string &name, Variable* var);
    void addArg     (const std::string &name); //is necessary to add returned status value with line ,pos end error code and string;
    void addParam   (Line * line);
    void addParam   (const std::string &name, TypeEn ty, DataStructTypeEn dsty, uint64_t len);
    void addParam   (const std::string &name, const std::string & linkName, DataStructTypeEn dsty=DataStructTypeEn::constant_dsty);
    void addReturn  (const std::string &name, Variable* var); //is necessary to add returned status value with line ,pos end error code and string;

    //varStack push/pop 
    void             push       (Variable*);
    Variable*        pop   ();
    stack<Variable*> pop(size_t length);



    std::map<std::string /*name*/, std::string /*link name*/> getParameterLinkNames(bool hideUnused = false) {
        std::map<std::string, std::string > ret;
        for (auto& value : lines_)
            if (value->isArg())
                if( ! (hideUnused && value->isUnused()))
                    ret[value->getName(true)]=value->getLinkName();
        return ret;
    }

private:
    //create operation
    Variable* typeConvOp    (TypeEn   targetType, Variable* arg1);
    Variable* builtInFuncOp (OpCodeEn    uTypeOp, Variable* arg1);
    Variable* arithmeticOp  (OpCodeEn    uTypeOp, Variable* arg1, Variable* arg2);
    Variable * comparsionOp(OpCodeEn uTypeOp, Variable * arg1, Variable * arg2);
    Variable* selectOp      (Variable*      arg1, Variable* arg2, Variable* arg3);
    Variable* convolveOp    (OpCodeEn    uTypeOp, Variable* arg1, Variable* arg2, uint32_t shift=0);

public:
    //create operation and push to varStack
    void addTypeConvOp      (TypeEn targetType);
    void addBuiltInFuncOp   (OpCodeEn uTypeOp);
    void addInvOp           ();
    void addArithmeticOp    (OpCodeEn uTypeOp);
    void addComarsionOp     (OpCodeEn uTypeOp);
    void addConvolveOp      (OpCodeEn uTypeOp, uint32_t shift = 0);
    void addSelectOp        ();

    void addRangeOp         (size_t argCount);
    void addShiftOp         ();
    void addDecimationOp    ();
    void addSmallArrayDefinitionOp(size_t length);
    //create call
    void addCall            (Body* body);


    stack<Line*>               getRet                 () { return returnStack_; }
    int                        getArgCount            () { return argCount_; }
    Line*                      getLastLineFromName    (std::string name);
    std::string                getName                () { return name_; };
    const stack<ParameterIfs*> getOutputParameterList ();
    GarbageContainer *         getGarbageContainer    () { return garbage_contaiiner_; }


    // tree walker methods
    std::string  print(std::string tab="", bool DSTEna = false, bool hideUnusedLines = false);
    Body* genBodyByPrototype(stack<Variable*> args ,bool isPrototype);
    void  symplyfy();
    void  reduce();// this function doesn't work correctly
    void  genTable(TableGenContext * tableGenContext);

private:

    GarbageContainer * garbage_contaiiner_;

    bool isPrototype_=false;
    std::string name_="main";
    std::vector<Line*> lines_;

    stack<Variable*> varStack_;

    stack<Line*> argStack_;
    stack<Line*> returnStack_;

    int   argCount_=0;
    Body* genBody_=nullptr;

    friend BodyGenContext;
};



class Call :public Variable
{
public:
    Call(Body* body, stack<Variable*> args = {}) :Variable() {
        body_ = body; 
        args_ = args;

        auto ret = body_->getRet()[0];

        level_       = ret->getLevel();
        type_        = ret->getType();
        dsType_      = ret->getDSType();
        length_      = ret->getLength();

        if (isConst(ret)) {
            binaryValue_ = ret->getBinaryValue();
            textValue_   = ret->getTextValue();
        }
    }
    ~Call() {}

    //safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override {
        commoMmarkUnusedVisitEnter(visitorStack);
        //for (int i = (args.size() - 1); i >= 0; i--) {
        //    visitorStack->push(args[i]);
        //    //args[i]->setBufferLength(this);
        //}
        auto ret = body_->getRet()[0];
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
    virtual void genBlocksVisitExit  (TableGenContext*  context) override {
         
         body_->genTable(context);
         uniqueName_ =(isLargeArr(this) ? "fb" : "fs") + std::to_string(context->getUniqueIndex());
         context->setUint(this);
         is_visited_ = false;
    };

    virtual void visitEnter(stack<Variable*>* visitorStack) override {
        visitorStack->push(this);
        for(int i= (args_.size()-1);i>=0;i--){
            visitorStack->push(args_[i]);
        }
        is_visited_ = true; 
    };

    virtual void genBodyVisitExit(BodyGenContext * context) override {
        

        stack<Variable*> a;
        for (auto &i : args_)
            a.push(context->pop());

        Body * b=body_;
        
        if(!context->isPrototype())
            b=body_->genBodyByPrototype(a, false);
        auto call =new Call(b, a);
        context->getGarbageContainer()->add(call);
        context->push(call);
        is_visited_ = false;
    }



    virtual void printVisitExit(stack<std::string>* Stack) override {

        llvm::outs()<<body_->print("    ")<<"\n";

        for (auto& i : args_) 
            auto x=Stack->pop();

        Stack->push( body_->getName()+".ret."+ toString(type_));
        is_visited_ = false;
    };

    virtual void reduceLinksVisitExit() override { 
        is_visited_ = false; 
    }


    virtual string printUint() { 

        return uniqueName_ + " = assignCall(" + body_->getRet()[0]->getAssignedVal(true)->getUniqueName() + ")"; 
    }
    virtual void setupIR(IRGenerator & builder)override;

    virtual Variable*   getAssignedVal(bool deep = false)  override { 
        if (is_buffered & deep) {
            body_->getRet()[0]->getAssignedVal(true)->setBuffered();
        }

        return body_->getRet()[0]->getAssignedVal(deep);
    }
    //virtual Variable*   getAssignedVal(bool deep = false)  override { return this; }
private:
    stack<Variable*>  args_;
    Body*             body_ = nullptr;

};






#endif // !BODY_H
