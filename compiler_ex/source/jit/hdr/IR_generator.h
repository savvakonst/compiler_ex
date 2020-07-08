
#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <iostream>
#include <vector>
#include "llvm/IR/IRBuilder.h"
#include "types_jty.h"

class Table;
class Variable;

class IRGenerator : public llvm::IRBuilder <>
{
public:
    IRGenerator(llvm::LLVMContext & context, Table * table_=NULL);

    ~IRGenerator();


    bool CheckExistence(llvm::Value *var) { 
        for (auto &i : initializedVariablesList)
            if (i == var) return true;
        return false;
    }
    void AddInitializedVariable(llvm::Value *var) { initializedVariablesList.push_back(var); }
    void ClearInitializedVariablesList() { initializedVariablesList.clear(); }


    llvm::Value * CreateFPow(llvm::Value *AOperand, llvm::Value *BOperand, const std::string &name="");
    llvm::Value * CreateConst(uint64_t & binaryValue, TypeEn targetTy, const std::string &name="");
    llvm::Value * CreateArithmetic(llvm::Value *AOperand, llvm::Value *BOperand, opCodeEn opCode, const std::string &name="");
    llvm::Value * CreateInv(llvm::Value * AOperand, opCodeEn opCode, const std::string &name="");
    llvm::Value * CreateTypeConv(llvm::Value *AOperand,  opCodeEn opCode, TypeEn targetTy, const std::string &name="");
    llvm::Value * CreateBuiltInFunc(llvm::Value *AOperand, opCodeEn opCode, const std::string &name="");
    llvm::Value * CreatePositionalAlloca(llvm::Type *AOperand, unsigned int i, const std::string &name="");
    llvm::Value * CreatePositionalOffset( std::string name="", uint64_t startValue=0);
    llvm::Value * CreatePositionalOffsetAlloca(std::string name="", uint64_t startValue=0);
    llvm::Value * CreatePositionalInBoundsGEP(llvm::Value *Ptr, llvm::ArrayRef<llvm::Value *> IdxList, const std::string &Name = "");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, const  std::string &name="");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, bool isVolatile, const std::string &name="");
    llvm::Value * CreateLoadOffset( const std::string &name="common_offset");
    llvm::Value * CreateConvolve(llvm::Value *AOperand, llvm::Value *BOperand, const std::string &name="");
    void          CreatePositionalStore(llvm::Value * AOperand, llvm::Value * BOperand, bool isVolatile = false);
    llvm::Value * CreateBufferInit(TypeEn targetTy, const std::string &name="");
    void          CreateStartBRs();
    void          CreateMidleBRs();


    void SetInitInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(initBlock, bb); };
    void SetLoadInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(loadBlock, bb); };
    void SetCalcInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(calcBlock, bb); };
    void SetStoreInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(storeBlock, bb); };
    
    void SetLoopEnterInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(loopEnterBlock, bb); };
    void SetIntermediateInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(intermediateBlock, bb); };
    void SetCycleExitInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(cycleExitBlock, bb); };
    void SetExitInsertPoint(llvm::BasicBlock*bb=NULL) { SetCInsertPoint(exitBlock, bb); };

    void SetLastInsertPoint() { SetInsertPoint(bbList.back()); };



    void SetCurrentFunction(llvm::Function* currentFunction_) { currentFunction=currentFunction_; }
    void SetDeclareConvolve(llvm::Type* type, uintptr_t addr);

    void SetCurrentOffsetValue(llvm::Value* currentOffsetValue_) { currentOffsetValue=currentOffsetValue; }
    void SetCurrentCMPRes(llvm::Value *currentCMPRes_) {currentCMPRes=currentCMPRes_;}

    void SetOffsetToZero();

    void DropBaseInsertPoint() {
        calcBlock=NULL;
        loadBlock=NULL;
        storeBlock=NULL;
    };


    llvm::BasicBlock* getInitBlock() { return initBlock; }
    llvm::BasicBlock* getLoadBlock() { return loadBlock; }
    llvm::BasicBlock* getCalcBlock() { return calcBlock; }
    llvm::BasicBlock* getStoreBlock() { return storeBlock; }
    llvm::BasicBlock* getCurrentBlock() { return BB; }

    llvm::BasicBlock* getBlock(int N) { return bbList[N]; }
    llvm::BasicBlock* getLastBlock(int N) { return bbList.back(); }

    llvm::Value*    getCurrentOffsetValueAlloca() { return currentOffsetValueAlloca; }
    llvm::Value*    getCurrentOffsetValue() { return currentOffsetValue;}
    llvm::Value*    getCurrentCMPRes() { return currentCMPRes; }

    llvm::Type*     getLLVMType(TypeEn targetTy);
    llvm::Function* getCurrentFunction() { return currentFunction; }




private:
    typedef llvm::BasicBlock* BasicBlockPtr;

    void SetCInsertPoint(BasicBlockPtr & prot, llvm::BasicBlock * bb=NULL) {
        if (bb == NULL)
            bb=prot;
        else {
            prot=bb;
            bbList.push_back(bb);
        }
        SetInsertPoint(bb);
    }

    llvm::BasicBlock* initBlock=NULL;
    llvm::BasicBlock* calcBlock=NULL;
    llvm::BasicBlock* loadBlock=NULL;
    llvm::BasicBlock* storeBlock=NULL;
    llvm::BasicBlock* intermediateBlock=NULL;
    llvm::BasicBlock* loopEnterBlock=NULL;
    llvm::BasicBlock* cycleExitBlock=NULL;
    llvm::BasicBlock* exitBlock=NULL;

    std::vector <llvm::BasicBlock*> bbList;

    //  list of variables that are alrady loaded in current loop block,
    //  this list should be cleared every time in the transition to a next block.
    std::vector <llvm::Value*>  initializedVariablesList; 

    llvm::Value * convolveDoubleFunction=NULL;
    llvm::Value * convolveFloatFunction=NULL;

    llvm::Value * convolveI64Function=NULL;
    llvm::Value * convolveI32Function=NULL;

    llvm::Value * currentOffsetValueAlloca =NULL;
    llvm::Value * currentOffsetValue =NULL;
    llvm::Value * currentCMPRes      =NULL;

    Table * table=NULL;
    llvm::Function* currentFunction=NULL;


    std::vector<llvm::Value *> buffersList;
    //std::vector <void *> bufferList;
};


#endif // !IR_GENERATOR_H


