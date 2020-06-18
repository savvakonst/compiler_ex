#ifndef IR_GENERATOR
#define IR_GENERATOR

#include <iostream>
#include <vector>
#include "llvm/IR/IRBuilder.h"
#include "types_jty.h"



class Table;



class IRGenerator : public llvm::IRBuilder <>
{
public:
    IRGenerator(llvm::LLVMContext & context, Table * table_=NULL);

    ~IRGenerator()
    {
    }

    llvm::Value * CreateFPow(llvm::Value *AOperand, llvm::Value *BOperand, std::string name);
    llvm::Value * CreateConst(uint64_t & binaryValue, TypeEn targetTy, std::string name);
    llvm::Value * CreateArithmetic(llvm::Value *AOperand, llvm::Value *BOperand, opCodeEn opCode, std::string name);
    llvm::Value * CreateInv(llvm::Value * AOperand, opCodeEn opCode, std::string name);
    llvm::Value * CreateTypeConv(llvm::Value *AOperand,  opCodeEn opCode, TypeEn targetTy, std::string name);
    llvm::Value * CreateBuiltInFunc(llvm::Value *AOperand, opCodeEn opCode, std::string name);


private:
    Table * table=NULL;
};



#endif // !IR_GENERATOR


