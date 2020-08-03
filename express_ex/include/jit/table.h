#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "common.h"
#include <map> 


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
}

class IRGenerator;

typedef std::map<OpCodeEn, llvm::Function*>  BuiltInFuncMap;
class SubBlock
{
public:

    SubBlock (Variable* var);

    ~SubBlock() {}

    void     setUint(Variable * var);
    void     setBufferLength(uint64_t bufferLength_) { bufferLength=bufferLength_; }

    uint64_t getLevel () { return 0; };
    uint64_t getLength() { return length; };
    uint64_t getLeftLength() { return leftLength; };
    uint64_t getRightLength() { return rightLength; };

    string   print();

    bool generateIR(IRGenerator &builder, CycleStageEn type=CycleStageEn::midle,
        std::string basicBlockPrefix="", std::string basicBlockPostfix="");

private:
    stack<Variable*> unitList;

    uint64_t leftLength;
    uint64_t rightLength;
    uint64_t length;
    uint64_t bufferLength=1 << 20;
};

class Block
{
public:

    //Block (uint64_t l) {level =l; }
    Block (Variable* var);

    ~Block() {}

    void     setUint(Variable * var);
    void     setBufferLength(uint64_t bufferLength_) {
        for (auto i : subBlockList)
            i->setBufferLength(bufferLength_);
        bufferLength=bufferLength_;
    }

    uint64_t getLevel () { return level; };
    uint64_t getLength() { return length; };

    string  print();

    bool generateIR(IRGenerator &builder, CycleStageEn type=CycleStageEn::midle, std::string basicBlockPrefix="");

private:
    void setUintToSubtable(Variable * var);

    stack<Variable*> unitList;
    stack<SubBlock*> subBlockList;
    uint64_t level;
    uint64_t length;
    uint64_t bufferLength=1 << 20;
};



class TableColumn
{
public:
    TableColumn (uint64_t len) { length =len; }
    TableColumn (Variable* var);

    ~TableColumn() {}

    void        setUint(Variable * var);
    void     setBufferLength(uint64_t bufferLength_) {
        for (auto i : blockList)
            i->setBufferLength(bufferLength_);
    }

    uint64_t    getLength() { return length; }
    Block *     getBlock(int level) {
        for (auto i : blockList)
            if (i->getLength() == length) {
                return i;
            }
        return NULL;
    }

    string  print();


    bool generateIR(IRGenerator &builder, CycleStageEn type=CycleStageEn::midle, std::string basicBlockPrefix="");

private:
    uint64_t      length;
    stack<Block*> blockList;
};




class Table
{
public:
    Table (llvm::Module * M_, int maxBufferLength_=(1 << 20), int minBufferLength_=0) {
        M=M_;
        declareFunctions();
        maxBufferLength=maxBufferLength_;
        minBufferLength=minBufferLength_;

    }
    ~Table() {}

    bool containsColumn(uint64_t length) {
        for (auto i : columnList)
            if (i->getLength() == length)
                return true;
        return false;
    }


    void setUint(Variable * var);


    TableColumn * getColumn(uint64_t length) {
        for (auto i : columnList)
            if (i->getLength() == length) {
                return i;
            }
        return NULL;
    }

    llvm::Function* getFloatBIFunc(OpCodeEn op);
    llvm::Function* getDoubleBIFunc(OpCodeEn op);
    llvm::Function* getBIFunc(BuiltInFuncMap & UBIFMap, OpCodeEn op, llvm::Type * Ty);


    void declareBuiltInFunctions(BuiltInFuncMap & UBIFMap, llvm::Type * Ty);

    string  print();
    void calculateBufferLength(std::string basicBlockPrefix="");
    bool generateIR(std::string basicBlockPrefix="");



private:
    void declareFunctions();
    llvm::Module* M;
    llvm::Function* currentFunction;

    BuiltInFuncMap floatBIFuncMap;
    BuiltInFuncMap doubleBIFuncMap;

    llvm::Type * fTy = NULL;
    llvm::Type * dTy = NULL;

    stack<Variable *> constList;
    stack<Variable *> smallArrayList;
    uint64_t *maxColumnDepth=0;
    stack<TableColumn *> columnList;


    std::map<OpCodeEn, int> BIF2LLVMmap;

    int minBufferLength;
    int maxBufferLength;
    int iterations=0;
};


class TableGenContext
{
public:
    TableGenContext (Table *arg) { table =arg; }
    ~TableGenContext() {}

    uint64_t          getUniqueIndex () { uniqueNameCounter++; return (uniqueNameCounter - 1); }
    void              setUint(Variable * var) { table->setUint(var); };

private:

    Table * table;
    uint64_t uniqueNameCounter=0;
};



#endif