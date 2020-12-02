#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include "variable.h"
#include "types_jty.h"


//extern bool g_gpu_acceleration_enable;


class Operation :public Value
{
public:
    Operation():Value() {
        op_code_ = OpCodeEn::none_op; 
    }

    // constructor of type conversion operation or shift and decimation 
    Operation(OpCodeEn op, Value* var, TypeEn targetType, int64_t shiftOrDecimation=0):Value() {
        CommonSetup(op, var);
        type_ = targetType;

        if (op == OpCodeEn::shift){
            shift_parameter_=shiftOrDecimation;
            level_ = var->getLevel() + 1;
        }
        else if (op == OpCodeEn::decimation) {
            decimation_parameter_=shiftOrDecimation;
            level_ = var->getLevel() + 1;
        }
        else {
            level_ = var->getLevel();
        }

        operand_.push_back(var); 
    }


    // constructor of convolve operation
    Operation(OpCodeEn op, Value* largeArr, Value* smallArr, int64_t shift):Value(){ 
        CommonSetup(op, maxDSVar(largeArr, smallArr));
      
        if (op == OpCodeEn::convolve) {
            shift_parameter_  =shift;
            level_ = largeArr->getLevel()+1;
        }else {
            print_error("unknown convolve op");
        }

        type_  = largeArr->getType();
        if(ds_type_ == DataStructTypeEn::largeArr_dsty)
            length_ = largeArr->getLength();
        else
            length_ = maxInt(largeArr->getLength(), smallArr->getLength());
        
        shift_parameter_ = shift;

        operand_.push_back(largeArr);
        operand_.push_back(smallArr);
    }

    // constructor of arithetic, logic or comparsion operation 
    Operation(OpCodeEn op, Value* var1, Value* var2) :Value() {
        CommonSetup(op, maxDSVar(var1, var2));

        type_  = maxTypeVar(var1, var2)->getType();
        type_  = isComparsion(op) && !isUnknownTy(type_) ? TypeEn::int1_jty : type_;

        level_ = maxLevelVar(var1, var2)->getLevel();

        operand_.push_back(var1);
        operand_.push_back(var2);

        for (auto i : operand_)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    // constructor of trenary operation 
    Operation(OpCodeEn op, Value* var1, Value* var2, Value* var3, TypeEn targetType, bool rec_call = false) :Value() {
        CommonSetup(op, maxDSVar(var1, var2));
        type_ = targetType;
        level_ = maxLevelVar(maxLevelVar(var1, var2), var3)->getLevel();
        contain_rec_call_ = rec_call;

        operand_.push_back(var1);

        if(rec_call && !isUnknownTy(type_))// it is a dirty hack
            if(var1->getAssignedVal(true)->getNodeType() == NodeTypeEn::tailCall)
                type_ = var2->getType();
            else
                type_ = var1->getType();
            

        operand_.push_back(var2);
        operand_.push_back(var3);

        
        for (auto i : operand_)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    // constructor of small array definition
    Operation(OpCodeEn op, stack<Value*> &args, TypeEn targetType) :Value() {

        size_t argsSize=args.size();
        op_code_ = op;

        if (argsSize < 1) print_error("range() - invalid signature");

        if (op==OpCodeEn::smallArrayDef) {
            for (auto &i : args)
                operand_.push_back(i);

            type_ = targetType;
            ds_type_ = DataStructTypeEn::smallArr_dsty;
            length_ = argsSize;

            level_ = 0; 
        }
        else if (op == OpCodeEn::smallArrayRange) {
            for (auto &i : args)
                operand_.push_back(i);

            if (isUnknownTy(targetType))
                return;

            ds_type_ = DataStructTypeEn::smallArr_dsty;
            smallArray();

            if (argsSize > 3)print_error("range( .. ) -invalid signature");
        }
    }

    void CommonSetup(OpCodeEn op, Value* var) {
        op_code_ = op;
        ds_type_ = var->getDSType();
        length_ = var->getLength();
    }

    int64_t getSliceParameter() {
        if (op_code_ == OpCodeEn::shift)
            return shift_parameter_;
        else if (op_code_ == OpCodeEn::decimation)
            return decimation_parameter_;
        else 
            print_error("getSliceParameter");
        return -1;
    }


    virtual NodeTypeEn getNodeType()const override { return  contain_rec_call_ ? NodeTypeEn::tailCallSelect :  NodeTypeEn::operation; }

    //safe functions .external stack is used
    void         visitEnterSetupBuffer(stack<Value*>* visitorStack);
    void         visitEnterStackUpdate(stack<Value*>* visitorStack);

    virtual void visitEnter(stack<Value*>* visitorStack) override;
    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack) override;


    virtual void genBodyVisitExit(BodyGenContext * context) override;
    virtual void calculateConstRecursive(ConstRecursiveGenContext* context)override;
    virtual void printVisitExit( stack<std::string>* Stack)	override;
    virtual void genBlocksVisitExit(TableGenContext* context) override;

    virtual void genConstRecursiveVisitExit(ConstRecursiveGenContext* context) override;


    virtual Value* getAssignedVal(bool deep = false)  override { return this; }

    virtual string printUint() override;
    virtual void setupIR(IRGenerator & builder) override;


    virtual void calculate() override;

private:
    void smallArrayGen();
    void smallArray();
    void smallArray(Value* arg1, Value* arg2, Value* arg3);
    void smallArray(Value* arg1, Value* arg2);
    void smallArray(Value* arg1);


    std::vector<Value*> operand_;
    std::vector<Value*> simplified_operand_;

    OpCodeEn op_code_ = OpCodeEn::none_op;

    // convolve params
    int64_t shift_parameter_ = 0; 
    int64_t decimation_parameter_ = 0;

    bool contain_rec_call_ = false;

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





Value* newInvOperation(GarbageContainer* garbageContainer, Value* arg1);
Value* newBuiltInFuncOperation(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1, OpCodeEn uTypeOp);
Value* newArithmeticOperation(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newComparsionOperation(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newConvolveOperation(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1, Value* arg2, int64_t shift=0, OpCodeEn uTypeOp = OpCodeEn::convolve);
Value* newTypeConvOp(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1);
Value* newSelectOp(GarbageContainer* garbageContainer, TypeEn targetType, Value* arg1, Value* arg2, Value* arg3, bool rec_call = false);
Value* newSliceOp(GarbageContainer* garbageContainer, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newSliceOp(GarbageContainer* garbageContainer, Value* arg1, int64_t intVal, OpCodeEn uTypeOp);
Value* newSmallArrayDefOp(GarbageContainer* garbageContainer, stack<Value*> &args, OpCodeEn uTypeOp=OpCodeEn::smallArrayDef, bool isPrototype = false);

#endif