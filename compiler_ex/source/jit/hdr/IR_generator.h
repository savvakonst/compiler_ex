#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <iostream>
#include <vector>
#include "llvm/IR/IRBuilder.h"
#include "types_jty.h"



class Table;



class IRGenerator : public llvm::IRBuilder <>
{
public:
    IRGenerator(llvm::LLVMContext & context, Table * table_=NULL);

    ~IRGenerator();

    llvm::Value * CreateFPow(llvm::Value *AOperand, llvm::Value *BOperand, std::string name="");
    llvm::Value * CreateConst(uint64_t & binaryValue, TypeEn targetTy, std::string name="");
    llvm::Value * CreateArithmetic(llvm::Value *AOperand, llvm::Value *BOperand, opCodeEn opCode, std::string name="");
    llvm::Value * CreateInv(llvm::Value * AOperand, opCodeEn opCode, std::string name="");
    llvm::Value * CreateTypeConv(llvm::Value *AOperand,  opCodeEn opCode, TypeEn targetTy, std::string name="");
    llvm::Value * CreateBuiltInFunc(llvm::Value *AOperand, opCodeEn opCode, std::string name="");
    llvm::Value * CreatePositionalAlloca(llvm::Type *AOperand, unsigned int i,std::string name="");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, std::string name="");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, bool isVolatile, const std::string name="");
    llvm::Value * CreateConvolve(llvm::Value *AOperand, llvm::Value *BOperand, std::string name="");
    void          CreatePositionalStore(llvm::Value * AOperand, llvm::Value * BOperand, bool isVolatile = false);

    void SetInitInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(initBlock, bb); };
    void SetLoadInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(loadBlock, bb); };
    void SetCalcInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(calcBlock, bb); };
    void SetStoreInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(storeBlock, bb); };



    void SetCurrentFunction(llvm::Function* currentFunction_) { currentFunction=currentFunction_; }

    void SetDeclareConvolve(llvm::Type* type, uintptr_t addr);

    llvm::Type*     getLLVMType(TypeEn targetTy);
    llvm::Function* getCurrentFunction() { return currentFunction; }

private:
    typedef llvm::BasicBlock* BasicBlockPtr;



    void SetCInsertPoint(BasicBlockPtr & prot, llvm::BasicBlock * bb=NULL) {
        if (bb == NULL)
            bb=prot;
        else
            prot=bb;
        SetInsertPoint(bb);
    }

    llvm::BasicBlock* initBlock;
    llvm::BasicBlock* calcBlock;
    llvm::BasicBlock* loadBlock;
    llvm::BasicBlock* storeBlock;


    llvm::Value * convolveDoubleFunction=NULL;
    llvm::Value * convolveFloatFunction=NULL;

    llvm::Value * convolveI64Function=NULL;
    llvm::Value * convolveI32Function=NULL;


    Table * table=NULL;
    llvm::Function* currentFunction=NULL;
};


#endif // !IR_GENERATOR_H


