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
    Operation(OpCodeEn op, Value* var, TypeEn target_type, int64_t shiftOrDecimation=0):Value() {
        CommonSetup(op, var);
        type_ = target_type;

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
        if(data_structure_type_ == DataStructureTypeEn::kLargeArr)
            length_ = largeArr->getLength();
        else
            length_ = maxInt(largeArr->getLength(), smallArr->getLength());
        
        shift_parameter_ = shift;

        operand_.push_back(largeArr);
        operand_.push_back(smallArr);
    }

    // constructor of arithetic, logic or comparsion operation 
    Operation(OpCodeEn op, Value* var_a, Value* var_b) :Value() {
        CommonSetup(op, maxDSVar(var_a, var_b));

        type_  = maxTypeVar(var_a, var_b)->getType();
        type_  = isComparsion(op) && !isUnknownTy(type_) ? TypeEn::int1_jty : type_;

        level_ = maxLevelVar(var_a, var_b)->getLevel();

        operand_.push_back(var_a);
        operand_.push_back(var_b);

        for (auto i : operand_)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    // constructor of trenary operation 
    Operation(OpCodeEn op, Value* var_a, Value* var_b, Value* var_c, TypeEn target_type, bool rec_call = false) :Value() {
        CommonSetup(op, maxDSVar(var_a, var_b));
        type_ = target_type;
        level_ = maxLevelVar(maxLevelVar(var_a, var_b), var_c)->getLevel();
        contain_rec_call_ = rec_call;

        operand_.push_back(var_a);

        if(rec_call && !isUnknownTy(type_))// it is a dirty hack
            if(var_b->getAssignedVal(true)->getNodeType() == NodeTypeEn::kTailCall)
                type_ = var_c->getType();
            else
                type_ = var_b->getType();
            

        operand_.push_back(var_b);
        operand_.push_back(var_c);

        
        for (auto i : operand_)
            if (i->getLevel() < level_) i->getAssignedVal(true)->setBuffered();
    }

    // constructor of small array definition
    Operation(OpCodeEn op, stack<Value*> &args, TypeEn target_type) :Value() {

        size_t argsSize=args.size();
        op_code_ = op;

        if (argsSize < 1) print_error("range() - invalid signature");

        if (op==OpCodeEn::smallArrayDef) {
            for (auto &i : args)
                operand_.push_back(i);

            type_ = target_type;
            data_structure_type_ = DataStructureTypeEn::kSmallArr;
            length_ = argsSize;

            level_ = 0; 
        }
        else if (op == OpCodeEn::smallArrayRange) {
            for (auto &i : args)
                operand_.push_back(i);

            if (isUnknownTy(target_type))
                return;

            data_structure_type_ = DataStructureTypeEn::kSmallArr;
            smallArray();

            if (argsSize > 3)print_error("range( .. ) -invalid signature");
        }
    }

    void CommonSetup(OpCodeEn op, Value* var) {
        op_code_ = op;
        data_structure_type_ = var->getDSType();
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


    virtual NodeTypeEn getNodeType()const override { return  contain_rec_call_ ? NodeTypeEn::kTailCallSelect :  NodeTypeEn::kOperation; }

    //safe functions .external stack is used
    void         visitEnterSetupBuffer(stack<Value*>* visitorStack);
    void         visitEnterStackUpdate(stack<Value*>* visitorStack);

    virtual void visitEnter(stack<Value*>* visitorStack) override;
    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack) override;


    virtual void genBodyVisitExit(BodyGenContext * context) override;
    virtual void calculateConstRecursive(RecursiveGenContext* context)override;
    virtual void printVisitExit(PrintBodyContext* context) override;
    virtual void genBlocksVisitExit(TableGenContext* context) override;

    virtual void genRecursiveVisitExit(RecursiveGenContext* context) override;


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

    std::string arSym[14]    = { "+", "+.", "-", "-.", "*", "*.", "/", "/", "/.", "%", "%", "%.", "**", "**." };
    std::string arComp[17]   = { "==", "!=", " ugt ", " uge ", " ult ", " ule ", ">", ">=", " less ", "<=", "==", "!=", ">", ">=", " less ", "<=", "_",};
    std::string arTConv[10]   ={ "trunc", "zext", "sext", "fptrunc", "fpext", "fptoi", "fptosi", "uitofp", "sitofp", "common_cast"};
    std::string arBuiltIn[6] = { "log", "log2", "log10", "cos", "sin", "exp" };
    std::string arSlice[2]   = { "decimation", "shift" };


    std::string txtArOp(OpCodeEn opCode)      { return arSym[((int)opCode - (int)TypeOpCodeEn::arithetic)]; }
    std::string txtCompOp(OpCodeEn opCode)    { return arComp[((int)opCode - (int)TypeOpCodeEn::comparsion)]; }
    std::string txtTConOp(OpCodeEn opCode)    { return arTConv[((int)opCode - (int)TypeOpCodeEn::typeConv)]; }
    std::string txtBuiltInOp(OpCodeEn opCode) { return arBuiltIn[((int)opCode - (int)TypeOpCodeEn::builtInFunc)]; }
    std::string txtSliceOp(OpCodeEn opCode)   { return arSlice[((int)opCode - (int)TypeOpCodeEn::slice_op)]; }
};





Value* newInvOperation(GarbageContainer* garbageContainer, Value* arg1);
Value* newBuiltInFuncOperation(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1, OpCodeEn uTypeOp);
Value* newArithmeticOperation(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newComparsionOperation(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newConvolveOperation(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1, Value* arg2, int64_t shift=0, OpCodeEn uTypeOp = OpCodeEn::convolve);
Value* newTypeConvOp(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1);
Value* newSelectOp(GarbageContainer* garbageContainer, TypeEn target_type, Value* arg1, Value* arg2, Value* arg3, bool rec_call = false);
Value* newSliceOp(GarbageContainer* garbageContainer, Value* arg1, Value* arg2, OpCodeEn uTypeOp);
Value* newSliceOp(GarbageContainer* garbageContainer, Value* arg1, int64_t intVal, OpCodeEn uTypeOp);
Value* newSmallArrayDefOp(GarbageContainer* garbageContainer, stack<Value*> &args, OpCodeEn uTypeOp=OpCodeEn::smallArrayDef, bool is_template = false);

#endif