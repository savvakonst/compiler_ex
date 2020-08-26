#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "common.h"
#include "ParameterIO.h"

using std::string;
class Variable;

enum class CycleStageEn
{
    start,
    midle,
    end
};

namespace llvm {
    class ConstantFolder;
    class IRBuilderDefaultInserter;

    class Module;
    class LLVMContext;
    class Function;
    class Type;
    class Value;
    template <typename T = ConstantFolder,
        typename Inserter = IRBuilderDefaultInserter> class IRBuilder;

    namespace legacy {
        class FunctionPassManager;
    }
}

class IRGenerator;

typedef std::map<OpCodeEn, llvm:: Function*>  BuiltInFuncMap;
class SubBlock
{
public:

    SubBlock (Variable* var);

    ~SubBlock() {}

    void     setUint(Variable * var);
    void     setBufferLength(uint64_t buffer_length) { buffer_length_=buffer_length; }
    uint64_t getLevel () { return 0; };
    uint64_t getLength() { return length_; };
    uint64_t getLeftLength() { return left_ength_; };
    uint64_t getRightLength() { return right_length_; };

    string   print();

    bool generateIR(IRGenerator &builder, CycleStageEn type=CycleStageEn::midle,
        std::string basicBlockPrefix="", std::string basicBlockPostfix="");

private:
    stack<Variable*> uint_list_;

    uint64_t left_ength_;
    uint64_t right_length_;
    uint64_t length_;
    uint64_t buffer_length_=1 << 20;
};

class Block
{
public:

    //Block (uint64_t l) {level =l; }
    Block (Variable* var);

    ~Block() {
        for (auto i : sub_block_list_)
            delete i;
    }

    void     setUint(Variable * var);
    void     setBufferLength(uint64_t bufferLength);
    uint64_t getLevel () { return level_;  };
    uint64_t getLength() { return length_; };
    void     recalcLeftRightBufferLengths();
    string   print();

    bool generateIR(IRGenerator &builder, CycleStageEn type = CycleStageEn::midle, std::string basicBlockPrefix="");


private:
    void setUintToSubtable(Variable * var);

    stack<Variable*> uint_list_;
    stack<SubBlock*> sub_block_list_;

    uint64_t left_ength_;
    uint64_t right_length_;
    uint64_t level_;
    uint64_t length_;
    uint64_t buffer_length_=1 << 20;
};



class TableColumn 
{
public:
    TableColumn (uint64_t len) { length_ =len; }
    TableColumn (Variable* var);

    ~TableColumn() {
        for (auto i : blockList_)
            delete i;
    }

    void   setUint(Variable * var);
    void   setBufferLength(uint64_t bufferLength_) { 
        for (auto i : blockList_)
            i->setBufferLength(bufferLength_);
    }

    uint64_t    getLength() { return length_; }
    Block *     getBlock(int level) {
        for (auto i : blockList_)
            if (i->getLength() == length_) {
                return i;
            }
        return nullptr;
    }
    void recalcLeftRightBufferLengths(){
        for (auto i : blockList_)
            i->recalcLeftRightBufferLengths();
    }
    string  print();


    bool generateIR(IRGenerator &builder, CycleStageEn type=CycleStageEn::midle, std::string basicBlockPrefix="");
    bool generateIR(IRGenerator &builder, CycleStageEn type, int64_t level, std::string basicBlockPrefix="");
private:
    uint64_t      length_;
    stack<Block*> blockList_;
};



class Table
{
public:
    Table ( int maxBufferLength=(1<<20),int minBufferLength=0) {
        
        max_buffer_length_=maxBufferLength;
        min_buffer_length_=minBufferLength;

    }


    ~Table();

    bool containsColumn(uint64_t length) {
        for (auto i : column_list_)
            if (i->getLength() == length) 
                return true;
        return false;
    }

    void setUint(Variable * var);

    TableColumn * getColumn(uint64_t length){
        for (auto i : column_list_)
            if (i->getLength() == length) {
                return i;
            }
        return nullptr;
    }
    void recalcLeftRightBufferLengths() {
        for (auto i : column_list_)
            i->recalcLeftRightBufferLengths();
    }
    llvm::Function* getFloatBIFunc(OpCodeEn op);
    llvm::Function* getDoubleBIFunc(OpCodeEn op);
    llvm::Function* getBIFunc(BuiltInFuncMap & UBIFMap, OpCodeEn op, llvm::Type * Ty);
    llvm::Function* getConvolveFunc(TypeEn type);
    void declareBuiltInFunctions(BuiltInFuncMap & UBIFMap, llvm::Type * Ty);

    string  print();
    void calculateBufferLength(std::string basicBlockPrefix="");

    bool llvmInit();
    
    bool generateIR(std::string basicBlockPrefix="");

    bool runOptimization();
    bool run();

    std::string printllvmIr();

    

    friend class TableGenContext;
private:
    void declareFunctions();
    llvm::Module* M_=nullptr;                //external 
    llvm::LLVMContext * context_=nullptr;    //external 

    std::unique_ptr<llvm::Module> module_U_ptr_;    
    std::unique_ptr<llvm::legacy::FunctionPassManager> the_FPM_;

    llvm::Function* mainFunction_;    //external 

    std::map<TypeEn, llvm::Function*> convolve_map_;    //external 

    IRGenerator* builder_=nullptr;         //internal 

    BuiltInFuncMap float_BI_func_map_;     //external 
    BuiltInFuncMap double_BI_func_map_;    //external 


     
    llvm::Type * fTy_ = nullptr;         //external 
    llvm::Type * dTy_ = nullptr;         //external

    stack<Variable *> const_list_;           //external 
    stack<Variable *> small_array_list_;     //external 

    uint64_t *max_column_depth_=0;           
    stack<TableColumn *> column_list_;       //internal

    int64_t topLevel=0;

    std::map<OpCodeEn, int> BIF2LLVMmap_;
    std::set<SyncParameter *> parameterSet_;    //external

    int min_buffer_length_;
    int max_buffer_length_;
    int iterations_=0;



    std::string error_info_    = "";
};


class TableGenContext
{
public:
    TableGenContext (Table *arg) { table_ =arg;  }
    ~TableGenContext() {}
    
    uint64_t          getUniqueIndex () { unique_name_counter_++; return (unique_name_counter_ -3); }
    void              setUint(Variable * var) { table_->setUint(var);};
    void              setParameter(SyncParameter * var) { table_->parameterSet_.insert(var); };
    void              setMaxBufferLength(int64_t length) { 
        int64_t temp = (int64_t)1 << (int8_t)(floor(log2(length)) - 1);
        if (table_->max_buffer_length_> temp)
            table_->max_buffer_length_= temp; 
    };

private:
    Table * table_;
    uint64_t unique_name_counter_=0;

};



#endif
