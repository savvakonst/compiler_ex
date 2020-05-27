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

    Body(std::string name_ = "main", bool isPrototype_ = false);
    ~Body();

    Line*       getLastLineFromName(std::string name);
    std::string getName() { return name; };


    bool    isRetStackFull () {if (name != "main") return 0 < returnStack.size(); else return false;}
    bool    isRetStackEmpty() {return 0 == returnStack.size();}

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
    Variable* builtInFuncOp (opCodeEn    uTypeOp, Variable* arg1);
    Variable* arithmeticOp  (opCodeEn    uTypeOp, Variable* arg1, Variable* arg2);
    Variable* selectOp      (Variable*      arg1, Variable* arg2, Variable* arg3);
    Variable* convolveOp    (opCodeEn    uTypeOp, Variable* arg1, Variable* arg2, uint32_t shift=0);

public:
    //create operation and push to varStack
    void addTypeConvOp      (TypeEn targetType);
    void addBuiltInFuncOp   (opCodeEn uTypeOp);
    void addArithmeticOp    (opCodeEn uTypeOp);
    void addConvolveOp      (opCodeEn uTypeOp, uint32_t shift = 0);
    void addSelectOp        ();

    void addRangeOp(size_t argCount);
    void addShiftOp         ();
    void addDecimationOp    ();

    //create call
    void addCall            (Body* body_);

    stack<Line*>  getRet     () { return returnStack; }
    int           getArgCount() { return argCount; }

    // tree walker methods
    void  print(std::string tab="", bool DSTEna = false, bool hideUnusedLines = false);
    Body* genBodyByPrototype(stack<Variable*> args = {});
    void  symplyfy();
    void  genTable(TableGenContext * tableGenContext);

private:

    bool isPrototype=false;
    std::string name="main";
    std::vector<Line*> lines;

    stack<Variable*> varStack;

    stack<Line*> argStack;
    stack<Line*> returnStack;

    int argCount=0;
    Body* genBody;
};



class Call :public Variable
{
public:
    Call(Body* body_, stack<Variable*> args_ = {}) {
        body = body_; 

        //for (int i = (args_.size() - 1); i >= 0; i--) {
        //	args.push(args_[i]);
        //}
        args = args_;

        auto ret = body->getRet()[0];

        level       = ret->getLevel();
        type        = ret->getType();
        dstype      = ret->getDSType();
        length      = ret->getLength();

        if (isConst(ret)) {
            binaryValue = ret->getBinaryValue();
            textValue   = ret->getTextValue();
        }

    }
    ~Call() {}

    //safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override {
        commoMmarkUnusedVisitEnter(visitorStack);
        for (int i = (args.size() - 1); i >= 0; i--) {
            visitorStack->push(args[i]);
            args[i]->setBufferLength(this);
        }
        is_unused = false;
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

         uniqueName =(isLargeArr(this) ? "fl" : "fs") + std::to_string(context->getUniqueIndex());
         context->setUint(this);
         is_visited = false;
    };

    virtual void visitEnter(stack<Variable*>* visitorStack) override {
        visitorStack->push(this);
        for(int i= (args.size()-1);i>=0;i--){
            visitorStack->push(args[i]);
        }
        is_visited = true; 
    };

    virtual void genBodyVisitExit(stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr = NULL) override {
        stack<Variable*> a;

        for (auto &i : args) 
            a.push(Stack->pop());

        auto b=body->genBodyByPrototype(a);

        Stack->push(new Call(b,a)); 
        is_visited = false;
    }

    virtual void printVisitExit(stack<std::string>* Stack) override {
        std::cout <<"  call." << body->getName() <<"\n";
        body->print("    ");
        for (auto& i : args) 
            auto x=Stack->pop();

        Stack->push( body->getName()+".ret."+ typeToStr(type));
        is_visited = false;
    };

    //dangerous functions . recursive call is used
    virtual std::string Print() { return textValue; };
    virtual Variable* generate() { return new Variable(textValue, type); };

private:
    stack<Variable*> args;
    Body* body = NULL;

};






#endif
