#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include "variable.h"
#include "types_jty.h"





class Operation :public Variable
{
public:
    Operation():Variable() { opCode = OpCodeEn::none_op;  }

    Operation(OpCodeEn op, Variable* var, TypeEn targetType, int64_t shiftOrDecimation=0):Variable() {
        CommonSetup(op, var);
        type_ = targetType;

        if (op == OpCodeEn::shift){
            shiftParameter=shiftOrDecimation;
            level_ = var->getLevel() + 1;
        }
        else if (op == OpCodeEn::decimation) {
            decimationParameter=shiftOrDecimation;
            level_ = var->getLevel() + 1;
        }
        else {
            level_ = var->getLevel();
        }

        operand.push_back(var); 
    }

    Operation(OpCodeEn op, Variable* largeArr, Variable* smallArr, int64_t shift):Variable(){
        CommonSetup(op, maxDS(largeArr, smallArr));
       
        if (op == OpCodeEn::convolve) {
            shiftParameter  =shift;
            level_ = largeArr->getLevel()+1;
        }else {
            print_error("unknown convolve op");
        }

        
        type_  = largeArr->getType();
        if (dsType_ == DataStructTypeEn::largeArr_dsty)
            length_ = largeArr->getLength();
        else
            length_ = maxInt(largeArr->getLength(), smallArr->getLength());
        
        shiftParameter = shift;

        operand.push_back(largeArr);
        operand.push_back(smallArr);
    }


    Operation(OpCodeEn op, Variable* var1, Variable* var2) :Variable() {
        CommonSetup(op, maxDS(var1, var2));

        type_  = isComparsion(op)? TypeEn::int1_jty : max(var1, var2)->getType();
        level_ = maxLevel(var1, var2)->getLevel();


        operand.push_back(var1);
        operand.push_back(var2);

        for (auto i : operand)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    Operation(OpCodeEn op, Variable* var1, Variable* var2, Variable* var3, TypeEn targetType) :Variable() {
        CommonSetup(op, maxDS(var1, var2));
        type_ = targetType;
        level_ = maxLevel(maxLevel(var1, var2), var3)->getLevel();

        operand.push_back(var1);
        operand.push_back(var2);
        operand.push_back(var3);

        for (auto i : operand)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    Operation(OpCodeEn op, stack<Variable*> &args, TypeEn targetType) :Variable() {

        size_t argsSize=args.size();
        opCode = op;

        if (argsSize < 1) print_error("range() - invalid signature");

        if (op==OpCodeEn::smallArrayDef) {
            for (auto &i : args)
                operand.push_back(i);

            type_ = targetType;
            dsType_ = DataStructTypeEn::smallArr_dsty;
            length_ = argsSize;

            level_ = 0; 
        }
        else if (op == OpCodeEn::smallArrayRange) {
            for (auto &i : args)
                operand.push_back(i);

            if (isUnknownTy(targetType))
                return;

            dsType_ = DataStructTypeEn::smallArr_dsty;
            smallArray();

            if (argsSize > 3)print_error("range( .. ) -invalid signature");
        }
    }

    void CommonSetup(OpCodeEn op, Variable* var) {
        opCode = op;
        dsType_ = var->getDSType();
        length_ = var->getLength();
    }

    int64_t getSliceParameter() {
        if (opCode == OpCodeEn::shift)
            return shiftParameter;
        else if (opCode == OpCodeEn::decimation)
            return decimationParameter;
        else 
            print_error("getSliceParameter");
        return -1;
    }


    //safe functions .external stack is used
    void         visitEnterSetupBuffer(stack<Variable*>* visitorStack);
    void         visitEnterStackUpdate(stack<Variable*>* visitorStack);

    virtual void visitEnter(stack<Variable*>* visitorStack) override;
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override;


    virtual void genBodyVisitExit(BodyGenContext * context) override;
    virtual void printVisitExit( stack<std::string>* Stack)	override;
    virtual void genBlocksVisitExit(TableGenContext* context) override;
    virtual void reduceLinksVisitExit()override;


    virtual Variable* getAssignedVal(bool deep = false)  override { return this; }

    virtual string printUint();
    virtual void setupIR(IRGenerator & builder) override;


    virtual void calculate() override;

private:
    void smallArrayGen();
    void smallArray();
    void smallArray(Variable* arg1, Variable* arg2, Variable* arg3);
    void smallArray(Variable* arg1, Variable* arg2);
    void smallArray(Variable* arg1);


    std::vector<Variable*> operand;
    std::vector<Variable*> simplified_operand;

    OpCodeEn opCode = OpCodeEn::none_op;

    // convolve params
    int64_t shiftParameter = 0; 
    int64_t decimationParameter = 0;


    std::string arSym[14]    = { "+","+.","-","-.","*","*.","/","/","/.","%","%","%.","**","**." };
    std::string arComp[16]   = { "==","!=","/","/","/","/",    ">",">=","<","<=", 
                                  ">",">=","<","<=", "/"};
    std::string arTConv[9]   = { "trunc","zext","sext","fptrunc","fpext","fptoi","fptosi","uitofp","sitofp" };
    std::string arBuiltIn[6] = { "log" ,"log2", "log10" , "cos" ,"sin" , "exp" };
    std::string arSlice[2]   = { "decimation", "shift" };


    std::string txtArOp(OpCodeEn opCode)      { return arSym[((int)opCode - (int)TypeOpCodeEn::arithetic)]; }
    std::string txtCompOp(OpCodeEn opCode)    { return arComp[((int)opCode - (int)TypeOpCodeEn::comparsion)]; }
    std::string txtTConOp(OpCodeEn opCode)    { return arTConv[((int)opCode - (int)TypeOpCodeEn::typeConv)]; }
    std::string txtBuiltInOp(OpCodeEn opCode) { return arBuiltIn[((int)opCode - (int)TypeOpCodeEn::builtInFunc)]; }
    std::string txtSliceOp(OpCodeEn opCode)   { return arSlice[((int)opCode - (int)TypeOpCodeEn::slice_op)]; }
};





Variable* newInvOperation(GarbageContainer* garbageContainer, Variable* arg1);
Variable* newBuiltInFuncOperation(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1, OpCodeEn uTypeOp);
Variable* newArithmeticOperation(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1, Variable* arg2, OpCodeEn uTypeOp);
Variable* newComparsionOperation(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1, Variable* arg2, OpCodeEn uTypeOp);
Variable* newConvolveOperation(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift=0, OpCodeEn uTypeOp = OpCodeEn::convolve);
Variable* newTypeConvOp(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1);
Variable* newSelectOp(GarbageContainer* garbageContainer, TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3);
Variable* newSliceOp(GarbageContainer* garbageContainer, Variable* arg1, Variable* arg2, OpCodeEn uTypeOp);
Variable* newSliceOp(GarbageContainer* garbageContainer, Variable* arg1, int64_t intVal, OpCodeEn uTypeOp);
Variable* newSmallArrayDefOp(GarbageContainer* garbageContainer, stack<Variable*> &args, OpCodeEn uTypeOp=OpCodeEn::smallArrayDef, bool isPrototype=false);

#endif