#ifndef BODY_H
#define BODY_H

#include <iostream>
#include <vector>

#include "defWarningIgnore.h"
#include "llvm/Support/raw_ostream.h"
#include "undefWarningIgnore.h"
#include "variable.h"
#include "line.h"
#include "operations.h"


class Body{
public:

    Body(std::string name = "main", bool is_template = false);
    ~Body();

    bool isRetStackFull (){ return (name_ != "main") ? 0 < return_stack_.size() : false; }
    bool isRetStackEmpty(){ return 0 == return_stack_.size(); }

    void addLine(const std::string &name, Value* var);
    void addArg(const std::string &name); //is necessary to add returned status_ value with line ,pos end error code and string;
    void addParam(Line * line);
    void addParam(const std::string &name, TypeEn ty, DataStructTypeEn dsty, uint64_t len);
    void addParam(const std::string &name, const std::string & linkName, DataStructTypeEn dsty=DataStructTypeEn::constant_dsty);
    void addReturn(const std::string &name, Value* var); //is necessary to add returned status_ value with line ,pos end error code and string;

    //varStack push/pop 
    void push(Value*);
    Value* pop();
    stack<Value*> pop(size_t length);

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
    Value* typeConvOp(TypeEn   targetType, Value* arg1);
    Value* builtInFuncOp(OpCodeEn    uTypeOp, Value* arg1);
    Value* arithmeticOp(OpCodeEn    uTypeOp, Value* arg1, Value* arg2);
    Value* comparsionOp(OpCodeEn uTypeOp, Value * arg1, Value * arg2);
    Value* selectOp(Value*      arg1, Value* arg2, Value* arg3);
    Value* convolveOp(OpCodeEn    uTypeOp, Value* arg1, Value* arg2, uint32_t shift=0);

public:
    //create operation and push to varStack
    void addTypeConvOp(TypeEn targetType);
    void addBuiltInFuncOp(OpCodeEn uTypeOp);
    void addInvOp();
    void addArithmeticOp(OpCodeEn uTypeOp);
    void addComarsionOp(OpCodeEn uTypeOp);
    void addConvolveOp(OpCodeEn uTypeOp, uint32_t shift = 0);
    void addSelectOp();

    void addRangeOp(size_t argCount);
    void addShiftOp();
    void addDecimationOp();
    void addSmallArrayDefinitionOp(size_t length);
    //create call
    void addCall(Body* body);
    void addTailCall();

    const stack<Line*> &getRet(){ return return_stack_; }
    int getArgCount(){ return arg_count_; }
    Line* getLastLineFromName(std::string name);
    std::string getName(){ return name_; };
    const stack<ParameterIfs*> getOutputParameterList();
    GarbageContainer* getGarbageContainer(){ return garbage_contaiiner_; }

    // tree walker methods
    std::string  print( std::string tab = "", bool DSTEna = false, bool hideUnusedLines = false);
    Body* genBodyByPrototype(stack<Value*> args ,bool is_template);
    untyped_t genConstRecusiveByPrototype(stack<Value*>& args);
    void  symplyfy();

    void  genTable(TableGenContext * tableGenContext);

private:

    GarbageContainer * garbage_contaiiner_;

    bool is_operator_ = false;
    bool is_tail_callable_ = false;
    bool is_template_ = false;

    std::string name_ = "main";
    std::vector<Line*> lines_;

    stack<Value*> var_stack_;

    stack<Line*> arg_stack_;
    stack<Line*> return_stack_;

    int arg_count_ = 0;
    Body* gen_body_ = nullptr;
    
    friend BodyGenContext;
};

#include "call.h"

#endif // !BODY_H
