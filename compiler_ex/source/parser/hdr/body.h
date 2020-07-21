#ifndef BODY_H
#define BODY_H

#include <iostream>
#include <vector>
#include "variable.h"
#include "line.h"
#include "operations.h"


class Body
{
public:

    Body(std::string name = "main", bool isPrototype = false);
    ~Body();

    Line*       getLastLineFromName(std::string name);
    std::string getName() { return name_; };


    bool    isRetStackFull () {if (name_ != "main") return 0 < returnStack_.size(); else return false;}
    bool    isRetStackEmpty() {return 0 == returnStack_.size();}

    void addLine    (std::string name, Variable* var);
    void addArg     (std::string name); //is necessary to add returned status value with line ,pos end error code and string;
    void addParam   (std::string name, TypeEn ty, DataStructTypeEn dsty, uint64_t len);
    void addReturn  (std::string name, Variable* var,int N=1); //is necessary to add returned status value with line ,pos end error code and string;

    //varStack push/pop 
    void push       (Variable*);
    Variable* pop   ();
private:
    //create operation
    Variable* typeConvOp    (TypeEn   targetType, Variable* arg1);
    Variable* builtInFuncOp (OpCodeEn    uTypeOp, Variable* arg1);
    Variable* arithmeticOp  (OpCodeEn    uTypeOp, Variable* arg1, Variable* arg2);
    Variable* selectOp      (Variable*      arg1, Variable* arg2, Variable* arg3);
    Variable* convolveOp    (OpCodeEn    uTypeOp, Variable* arg1, Variable* arg2, uint32_t shift=0);

public:
    //create operation and push to varStack
    void addTypeConvOp      (TypeEn targetType);
    void addBuiltInFuncOp   (OpCodeEn uTypeOp);
    void addArithmeticOp    (OpCodeEn uTypeOp);
    void addConvolveOp      (OpCodeEn uTypeOp, uint32_t shift = 0);
    void addSelectOp        ();

    void addRangeOp(size_t argCount);
    void addShiftOp         ();
    void addDecimationOp    ();
    void addSmallArrayDefinitionOp(size_t length);
    //create call
    void addCall            (Body* body);

    stack<Line*>  getRet     () { return returnStack_; }
    int           getArgCount() { return argCount_; }

    // tree walker methods
    std::string  print(std::string tab="", bool DSTEna = false, bool hideUnusedLines = false);
    Body* genBodyByPrototype(stack<Variable*> args = {});
    void  symplyfy();
    void  reduce();// this function doesn't work correctly
    void  genTable(TableGenContext * tableGenContext);

private:

    bool isPrototype_=false;
    std::string name_="main";
    std::vector<Line*> lines_;

    stack<Variable*> varStack_;

    stack<Line*> argStack_;
    stack<Line*> returnStack_;

    int   argCount_=0;
    Body* genBody_=NULL;
};



class Call :public Variable
{
public:
    Call(Body* body, stack<Variable*> args = {}) {
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
         uniqueName =(isLargeArr(this) ? "fb" : "fs") + std::to_string(context->getUniqueIndex());
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

    virtual void genBodyVisitExit(stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr = NULL) override {
        stack<Variable*> a;

        for (auto &i : args_) 
            a.push(Stack->pop());

        auto b=body_->genBodyByPrototype(a);
        auto call =new Call(b, a);
        Stack->push(call);
        is_visited_ = false;
    }

    virtual void printVisitExit(stack<std::string>* Stack) override {
        std::cout <<"  call." << body_->getName() <<"\n";
        body_->print("    ");
        for (auto& i : args_) 
            auto x=Stack->pop();

        Stack->push( body_->getName()+".ret."+ typeToStr(type_));
        is_visited_ = false;
    };

    virtual void reduceLinksVisitExit() override { 
        is_visited_ = false; 
    }


    virtual string printUint() { 

        return ""; uniqueName + " = assignCall(" + body_->getRet()[0]->getAssignedVal(true)->getUniqueName() + ")"; 
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
    Body*             body_ = NULL;

};






#endif // !BODY_H
