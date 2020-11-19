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


    bool    isRetStackFull () {if (name_ != "main") return 0 < return_stack_.size(); else return false;}
    bool    isRetStackEmpty() {return 0 == return_stack_.size();}

    void addLine    (const std::string &name, Variable* var);
    void addArg     (const std::string &name); //is necessary to add returned status_ value with line ,pos end error code and string;
    void addParam   (Line * line);
    void addParam   (const std::string &name, TypeEn ty, DataStructTypeEn dsty, uint64_t len);
    void addParam   (const std::string &name, const std::string & linkName, DataStructTypeEn dsty=DataStructTypeEn::constant_dsty);
    void addReturn  (const std::string &name, Variable* var); //is necessary to add returned status_ value with line ,pos end error code and string;

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
    Variable* comparsionOp  (OpCodeEn uTypeOp, Variable * arg1, Variable * arg2);
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
    void addTailCall        ();


    stack<Line*>               getRet                 () { return return_stack_; }
    int                        getArgCount            () { return arg_count_; }
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

    bool is_tail_callable_ = false;
    bool is_prototype_ = false;
    std::string name_="main";
    std::vector<Line*> lines_;

    stack<Variable*> varStack_;

    stack<Line*> arg_stack_;
    stack<Line*> return_stack_;

    //stack<Body*> context_;

    int arg_count_ = 0;
    Body* gen_body_ = nullptr;
    

    friend BodyGenContext;
};

#include "call.h"

#endif // !BODY_H
