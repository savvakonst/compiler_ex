
#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <iostream>
#include <vector>
#include "types_jty.h"
#include "buffer.h"
#include "ParameterIO.h"
#include "llvm/IR/IRBuilder.h"



class Table;
class Variable;





inline llvm::raw_ostream &operator<<(llvm::raw_ostream & OS, Buffer & arg) {
    arg.stream(OS);
    return OS;
}

class IRGenerator : public llvm::IRBuilder <>
{
public:


    IRGenerator(llvm::LLVMContext & context, Table * table_=nullptr);
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
    llvm::Value * CreateArithmetic(llvm::Value *AOperand, llvm::Value *BOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateComparsion(llvm::Value *AOperand, llvm::Value *BOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateInv(llvm::Value * AOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateTypeConv(llvm::Value *AOperand,  OpCodeEn opCode, TypeEn targetTy, const std::string &name="");
    llvm::Value * CreateBuiltInFunc(llvm::Value *AOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateConvolve(llvm::Value * aOperand, char * ptr, int64_t length, int64_t shift, TypeEn type, const std::string & name);
    llvm::Value * CreatePositionalAlloca(llvm::Type *AOperand, int64_t i, const std::string &name="");
    llvm::Value * CreatePositionalOffset( std::string name="", int64_t startValue=0);
    llvm::Value * CreatePositionalOffsetAlloca(std::string name="", int64_t startValue=0);
    llvm::Value * CreatePositionalInBoundsGEP(llvm::Value *Ptr, llvm::ArrayRef<llvm::Value *> IdxList, const std::string &Name = "");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, const  std::string &name="");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, bool isVolatile, const std::string &name="");
    llvm::Value * CreateLoadOffset( const std::string &name="common_offset");
    llvm::Value * CreateConvolve(llvm::Value *AOperand, llvm::Value *BOperand, const std::string &name="");
    void          CreatePositionalStore(llvm::Value * AOperand, llvm::Value * BOperand, bool isVolatile = false);
    llvm::Value * CreateBufferInit(TypeEn targetTy, const std::string &name="");
    void          CreateStartBRs();
    void          CreateMidleBRs();

    void        * AddBufferAlloca(Buffer * s);

    void SetInitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(initBlock, bb); };
    void SetLoadInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(loadBlock, bb); };
    void SetCalcInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(calcBlock, bb); };
    void SetStoreInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(storeBlock, bb); };
    
    void SetLoopEnterInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(loopEnterBlock, bb); };
    void SetIntermediateInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(intermediateBlock, bb); };
    void SetCycleExitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(cycleExitBlock, bb); };
    void SetExitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(exitBlock, bb); };

    void SetLastInsertPoint() { SetInsertPoint(bbList.back()); };



    void SetCurrentFunction(llvm::Function* currentFunction_) { currentFunction=currentFunction_; }
    void SetBufferUpdateFunction(llvm::Value* bufferUpdateFunction_) { bufferUpdateFunction=bufferUpdateFunction_; }
    void SetDeclareConvolve(llvm::Type* type, uintptr_t addr);

    void SetCurrentOffsetValue(llvm::Value* currentOffsetValue_) { currentOffsetValue=currentOffsetValue; }
    void SetCurrentCMPRes(llvm::Value *currentCMPRes_) {currentCMPRes=currentCMPRes_;}

    void SetOffsetToZero();

    void SetOffsetTo(int64_t val);

    void DropBaseInsertPoint() {
        calcBlock=nullptr;
        loadBlock=nullptr;
        storeBlock=nullptr;
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


     
    std::vector<Buffer*>  * getBufferList(BufferTypeEn bufferType = BufferTypeEn::input)  {
        return &buffer_list;
    }


private:
    typedef llvm::BasicBlock* BasicBlockPtr;

    void SetCInsertPoint(BasicBlockPtr & prot, llvm::BasicBlock * bb=nullptr) {
        if (bb == nullptr)
            bb=prot;
        else {
            prot=bb;
            bbList.push_back(bb);
        }
        SetInsertPoint(bb);
    }


    std::vector <Buffer *>  buffer_list;



    llvm::BasicBlock* initBlock=nullptr;
    llvm::BasicBlock* calcBlock=nullptr;
    llvm::BasicBlock* loadBlock=nullptr;
    llvm::BasicBlock* storeBlock=nullptr;
    llvm::BasicBlock* intermediateBlock=nullptr;
    llvm::BasicBlock* loopEnterBlock=nullptr;
    llvm::BasicBlock* cycleExitBlock=nullptr;
    llvm::BasicBlock* exitBlock=nullptr;

    std::vector <llvm::BasicBlock*> bbList;

    //  list of variables that are alrady loaded in current loop block,
    //  this list should be cleared every time in the transition to a next block.
    std::vector <llvm::Value*>  initializedVariablesList; 

    llvm::Value * convolveDoubleFunction=nullptr;
    llvm::Value * convolveFloatFunction=nullptr;

    llvm::Value * convolveI64Function=nullptr;
    llvm::Value * convolveI32Function=nullptr;

    llvm::Value * currentOffsetValueAlloca =nullptr;
    llvm::Value * currentOffsetValue =nullptr;
    llvm::Value * currentCMPRes      =nullptr;



    Table * table=nullptr;

    

    llvm::Function* currentFunction=nullptr;
    llvm::Value* bufferUpdateFunction=nullptr;

    std::vector<llvm::Value *> buffersList;
    //std::vector <void *> bufferList;
};


#endif // !IR_GENERATOR_H


