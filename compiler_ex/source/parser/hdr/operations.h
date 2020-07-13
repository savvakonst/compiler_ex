#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include "variable.h"
#include "types_jty.h"





class Operation :public Variable
{
public:


    Operation() { opCode = opCodeEn::NONE_op;  }

    Operation(opCodeEn op, Variable* var, TypeEn targetType, int64_t shiftOrDecimation=0) {
        CommonSetup(op, var);
        type = targetType;

        if (op == opCodeEn::shift){
            shiftParameter=shiftOrDecimation;
            level = var->getLevel() + 1;
        }
        else if (op == opCodeEn::decimation) {
            decimationParameter=shiftOrDecimation;
            level = var->getLevel() + 1;
        }
        else {
            level = var->getLevel();
        }

        operand.push_back(var); 
    }

    Operation(opCodeEn op, Variable* largeArr, Variable* smallArr, int64_t shift) {
        CommonSetup(op, maxDS(largeArr, smallArr));
       
        if (op == opCodeEn::convolve) {
            shiftParameter  =shift;
            level = largeArr->getLevel()+1;
        }else {
            print_error("unknown convolve op");
        }

        type  = largeArr->getType();
        shiftParameter = shift;

        operand.push_back(largeArr);
        operand.push_back(smallArr);
    }


    Operation(opCodeEn op, Variable* var1, Variable* var2) {
        CommonSetup(op, maxDS(var1, var2));
        type = var1->getType();
        
        level = maxLevel(var1, var2)->getLevel();

        operand.push_back(var1);
        operand.push_back(var2);

        for (auto i : operand)
            if (i->getLevel() < level) i->getAssignedVal(true)->setBuffered();
    }

    Operation(opCodeEn op, Variable* var1, Variable* var2, Variable* var3, TypeEn targetType) {
        CommonSetup(op, maxDS(var1, var2));
        type = targetType;

        level = maxLevel(maxLevel(var1, var2), var3)->getLevel();

        operand.push_back(var1);
        operand.push_back(var2);
        operand.push_back(var3);

        for (auto i : operand)
            if (i->getLevel() < level) i->getAssignedVal(true)->setBuffered();
    }

    Operation(opCodeEn op, stack<Variable*> &args, TypeEn targetType) {
        CommonSetup(op, maxDS(var1, var2));
        type = targetType;

        level = maxLevel(maxLevel(var1, var2), var3)->getLevel();

        operand.push_back(var1);
        operand.push_back(var2);
        operand.push_back(var3);

        for (auto i : operand)
            if (i->getLevel() < level) i->getAssignedVal(true)->setBuffered();
    }


    void CommonSetup(opCodeEn op, Variable* var) {
        opCode = op;
        dstype = var->getDSType();
        length = var->getLength();
    }

    int64_t getSliceParameter() {
        if (opCode == opCodeEn::shift)
            return shiftParameter;
        else if (opCode == opCodeEn::decimation)
            return decimationParameter;
        else 
            print_error("getSliceParameter");
    }


    //safe functions .external stack is used
    void         visitEnterSetupBuffer(stack<Variable*>* visitorStack);
    void         visitEnterStackUpdate(stack<Variable*>* visitorStack);

    virtual void visitEnter(stack<Variable*>* visitorStack)										         override;
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack)								     override;
    //virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack)                                  override;

    virtual void genBodyVisitExit( stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr = NULL) override;
    virtual void printVisitExit( stack<std::string>* Stack)										         override;
    virtual void genBlocksVisitExit(TableGenContext*  context)                                           override;
    virtual void reduceLinksVisitExit()                                                                  override;

   

    virtual Variable* getAssignedVal(bool deep = false)  override { return this; }



    virtual string printUint();
    virtual void setupIR(IRGenerator & builder)                                                    override;


    virtual void calculate() override;


private:
    std::vector<Variable*> operand;
    std::vector<Variable*> simplified_operand;

    opCodeEn opCode = opCodeEn::NONE_op;

    // convolve params
    int64_t shiftParameter = 0; 
    int64_t decimationParameter = 0;


    std::string arSym[14]       ={ "+","+.","-","-.","*","*.","/","/","/.","%","%","%.","**","**." };
    std::string arTConv[9]      ={ "trunc","zext","sext","fptrunc","fpext","fptoi","fptosi","uitofp","sitofp" };
    std::string arBuiltIn[6]    ={ "log" ,"log2", "log10" , "cos" ,"sin" , "exp" };
    std::string arSlice[2]      ={ "decimation", "shift" };

    std::string txtArOp(opCodeEn opCode) { return arSym[((int)opCode - (int)typeOpCodeEn::arithetic)]; }
    std::string txtTConOp(opCodeEn opCode) { return arTConv[((int)opCode - (int)typeOpCodeEn::typeConv)]; }
    std::string txtBuiltInOp(opCodeEn opCode) { return arBuiltIn[((int)opCode - (int)typeOpCodeEn::builtInFunc)]; }
    std::string txtSliceOp(opCodeEn opCode) { return arSlice[((int)opCode - (int)typeOpCodeEn::slice_op)]; }
};

Variable* newInvOperation(Variable* arg1);
Variable* newBuiltInFuncOperation(TypeEn targetType, Variable* arg1, opCodeEn uTypeOp);
Variable* newArithmeticOperation(TypeEn targetType, Variable* arg1, Variable* arg2, opCodeEn uTypeOp);
Variable* newConvolveOperation(TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift=0, opCodeEn uTypeOp = opCodeEn::convolve);
Variable* newTypeConvOp(TypeEn targetType, Variable* arg1);
Variable* newSelectOp(TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3);
Variable* newSliceOp(Variable* arg1, Variable* arg2, opCodeEn uTypeOp);
Variable* newSliceOp(Variable* arg1, int64_t intVal, opCodeEn uTypeOp);


#endif