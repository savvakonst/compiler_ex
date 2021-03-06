
#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <iostream>
#include <vector>
#include "parser/types_jty.h"
#include "buffer.h"
#include "ifs/ParameterIO.h"


#include "parser/defWarningIgnore.h"
#include "llvm/IR/IRBuilder.h"
#include "parser/undefWarningIgnore.h"


class Table;
class Value;





inline llvm::raw_ostream &operator<<(llvm::raw_ostream & OS, Buffer & arg) {
    arg.stream(OS);
    return OS;
}

class IRGenerator : public llvm::IRBuilder <>
{
public:


    IRGenerator(llvm::LLVMContext & context, Table * table, bool is_pure_function = false);
    ~IRGenerator();

     
    bool CheckExistence(llvm::Value *var) { 
        for (auto &i : initialized_value_list_)
            if (i == var) return true;
        return false;
    }
    void AddInitializedValue(llvm::Value *var) { initialized_value_list_.push_back(var); }
    void ClearInitializedValueList() { initialized_value_list_.clear(); }


    llvm::Value * CreateFPow(llvm::Value *AOperand, llvm::Value *BOperand, const std::string & name="");
    llvm::Value * CreateConst(int64_t & binaryValue, TypeEn targetTy, const std::string & name="");
    llvm::Value * CreateArithmetic(llvm::Value *AOperand, llvm::Value *BOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateComparsion(llvm::Value *AOperand, llvm::Value *BOperand, OpCodeEn opCode, const std::string &name="");
    llvm::Value * CreateInv(llvm::Value * AOperand, OpCodeEn opCode, const std::string & name="");
    llvm::Value * CreateTypeConv(llvm::Value *AOperand,  OpCodeEn opCode, TypeEn targetTy, const std::string &name="");
    llvm::Value * CreateBuiltInFunc(llvm::Value *AOperand, OpCodeEn opCode, const std::string & name="");
    llvm::Value * CreateConvolve(llvm::Value * aOperand, char * ptr, int64_t length, int64_t shift, TypeEn type, const std::string & name="");
    llvm::Value * CreateCall_(llvm::Value * Callee, llvm::ArrayRef<llvm::Value*> Args={}, const std::string & Name="");
    llvm::Value * CreateConvolve(llvm::Value *AOperand, llvm::Value *BOperand, const std::string & name="");
    llvm::Value * CreateGPUConvolve(llvm::Value * aOperand, char * ptr, int64_t length, int64_t shift, TypeEn type, const std::string & name="");
    llvm::Value * CreatePositionalAlloca(llvm::Type *AOperand, int64_t i, const std::string & name="");
    llvm::Value * CreatePositionalOffset( std::string name="", int64_t startValue=0);
    llvm::Value * CreatePositionalOffsetAlloca(std::string name="", int64_t startValue=0);
    llvm::Value * CreatePositionalInBoundsGEP(llvm::Value *Ptr, llvm::ArrayRef<llvm::Value *> IdxList, const std::string & Name = "");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, const  std::string & name="");
    llvm::Value * CreatePositionalLoad(llvm::Value *AOperand, bool isVolatile, const std::string & name="");
    llvm::Value * CreateLoadOffset( const std::string &name="common_offset");
    void          CreatePositionalStore(llvm::Value * AOperand, llvm::Value * BOperand, bool isVolatile = false);
    llvm::Value * CreateBufferInit(TypeEn targetTy, const std::string & name="");
    void          CreateStartBRs();
    void          CreateMidleBRs();

    void        * AddBufferAlloca(Buffer * s);




    llvm::BasicBlock* CreateNewIntermediateBB(const std::string& postfix){return CreateNewBB(init_bock_, "intermediate_" + postfix);}
    llvm::BasicBlock* CreateNewLoadBB(const std::string& postfix){return CreateNewBB(load_block_, "load_" + postfix);}
    llvm::BasicBlock* CreateNewCalcBB(const std::string& postfix){return CreateNewBB(calc_block_, "calc_" + postfix);}
    llvm::BasicBlock* CreateNewStoreBB(const std::string& postfix){return CreateNewBB(store_block_, "store_" + postfix);}


    void SetInitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(init_bock_, bb); };
    void SetLoadInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(load_block_, bb); };
    void SetCalcInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(calc_block_, bb); };
    void SetStoreInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(store_block_, bb); };
    void SetTerminalOpInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(terminal_op_block_, bb); };
    


    void SetLoopEnterInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(loop_enter_block_, bb); };
    void SetIntermediateInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(intermediate_block_, bb); };
    void SetCycleExitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(cycle_exit_block_, bb); };
    void SetExitInsertPoint(llvm::BasicBlock*bb=nullptr) { SetCInsertPoint(exit_block_, bb); };

    void SetLastInsertPoint() { SetInsertPoint(bb_List_.back()); };

    void SetCurrentFunction(llvm::Function* currentFunction_) { current_Function_=currentFunction_; }
    void SetBufferUpdateFunction(llvm::Value* bufferUpdateFunction_) { buffer_update_function_=bufferUpdateFunction_; }
    void SetDeclareConvolve(llvm::Type* type, uintptr_t addr);

    void SetCurrentOffsetValue(llvm::Value* currentOffsetValue_) { current_offset_value_=current_offset_value_; }
    void SetCurrentCMPRes(llvm::Value *currentCMPRes_) {current_CMP_res_=currentCMPRes_;}

    void SetOffsetToZero();

    void SetOffsetTo(int64_t val);

    void DropBaseInsertPoint() {
        calc_block_=nullptr;
        load_block_=nullptr;
        store_block_=nullptr;
        intermediate_block_=nullptr;
    };

    llvm::BasicBlock* getInitBlock() { return init_bock_; }
    llvm::BasicBlock* getLoadBlock() { return load_block_; }
    llvm::BasicBlock* getCalcBlock() { return calc_block_; }
    llvm::BasicBlock* getStoreBlock() { return store_block_; }
    llvm::BasicBlock* getTerminalOpBlock() { return terminal_op_block_; }
    llvm::BasicBlock* getCurrentBlock() { return BB; }
    llvm::BasicBlock* getExitBlock(){ return exit_block_; }

    llvm::BasicBlock* getBlock(int N) { return bb_List_[N]; }
    llvm::BasicBlock* getLastBlock(int N) { return bb_List_.back(); }

    llvm::Value* getCurrentOffsetValueAlloca() { return current_offset_value_alloca_; }
    llvm::Value* getCurrentOffsetValue() { return current_offset_value_;}
    llvm::Value* getCurrentCMPRes() { return current_CMP_res_; }

    llvm::Type* getLLVMType(TypeEn targetTy);
    llvm::Function* getCurrentFunction() { return current_Function_; }
    llvm::Module* getCurrentModule();
    
    std::vector<std::list<Buffer*>*>* getBufferList(BufferTypeEn bufferType = BufferTypeEn::input) {

        return &buffer_list_;
    }

    void nextBufferGeoup(){
        buffer_list_.push_back(new std::list<Buffer*>());
    }

    std::vector<llvm::Value*> arg_ptr_list_;
    const bool is_pure_function_;
private:




    typedef llvm::BasicBlock* BasicBlockPtr;

    llvm::BasicBlock* CreateNewBB(BasicBlockPtr& prot, const std::string& name="");


    void SetCInsertPoint(BasicBlockPtr & prot, llvm::BasicBlock * bb=nullptr) {
        if (bb == nullptr)
            bb=prot;
        else {
            prot=bb;
            bb_List_.push_back(bb);
        }
        SetInsertPoint(bb);
    }

    std::vector<std::list<Buffer*>*> buffer_list_;

    llvm::BasicBlock* init_bock_=nullptr;
    llvm::BasicBlock* calc_block_=nullptr;
    llvm::BasicBlock* load_block_=nullptr;
    llvm::BasicBlock* store_block_=nullptr;
    llvm::BasicBlock* terminal_op_block_=nullptr;
    llvm::BasicBlock* intermediate_block_=nullptr;
    llvm::BasicBlock* loop_enter_block_=nullptr;
    llvm::BasicBlock* cycle_exit_block_=nullptr;
    llvm::BasicBlock* exit_block_=nullptr;

    std::vector <llvm::BasicBlock*> bb_List_;

    //  list of values that are alrady loaded in current loop block,
    //  this list should be cleared every time in the transition to a next block.
    std::vector <llvm::Value*>  initialized_value_list_; 

    llvm::Value* convolve_double_function_=nullptr;
    llvm::Value* convolve_float_function_=nullptr;

    llvm::Value* convolve_I64_function_=nullptr;
    llvm::Value* convolve_I32_function_=nullptr;

    llvm::Value* current_offset_value_alloca_=nullptr;
    llvm::Value* current_offset_value_=nullptr;
    llvm::Value* current_CMP_res_=nullptr;



    Table * table_=nullptr;

    

    llvm::Function* current_Function_=nullptr;
    llvm::Value* buffer_update_function_=nullptr;

    std::vector<llvm::Value *> buffers_List_;
    //std::vector <void *> bufferList;
};


#endif // !IR_GENERATOR_H


