#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "common.h"
#include <map> 


using std::string;
class Variable;

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

typedef std::map<opCodeEn, llvm:: Function*>  BuiltInFuncMap;


class Block
{
public:

    Block (uint64_t l) {level =l; }
    Block (Variable* var);

    ~Block() {}

    uint64_t getLevel () { return level;  };
    uint64_t getLength() { return length; };
    void     setUint(Variable * var);
    string  print();

    void generateIR(IRGenerator &builder);

private:
    stack<Variable*> unitList;
    uint64_t level;
    uint64_t length;

};



class TableColumn 
{
public:
    TableColumn (uint64_t len) { length =len; }
    TableColumn (Variable* var);

    ~TableColumn() {}
    uint64_t    getLength(){ return length; }
    void        setUint(Variable * var);
    Block *     getBlock(int level) {
        for (auto i : blockList)
            if (i->getLength() == length) {
                return i;
            }
        return NULL;
    }

    string  print();

    void generateIR(IRGenerator &builder);


private:
    uint64_t      length;
    stack<Block*> blockList;
};




class Table
{
public:



    Table (llvm::Module * M_) { M=M_; declareFunctions();  }
    ~Table() {}

    bool containsColumn(uint64_t length) {
        for (auto i : columnList)
            if (i->getLength() == length) 
                return true;
        return false;
    }

    TableColumn * getColumn(uint64_t length){
        for (auto i : columnList)
            if (i->getLength() == length) {
                return i;
            }
        return NULL;
    }

    llvm::Function* getFloatBIFunc(opCodeEn op);
    llvm::Function* getDoubleBIFunc(opCodeEn op);
    llvm::Function* getBIFunc(BuiltInFuncMap & UBIFMap, opCodeEn op, llvm::Type * Ty);

    string  print();
    void declareBuiltInFunctions(BuiltInFuncMap & UBIFMap, llvm::Type * Ty);

    void generateIR();

    void setUint(Variable * var);

private:
    void declareFunctions();
    llvm ::Module* M;
    llvm::Function* currentFunction;

    BuiltInFuncMap floatBIFuncMap;
    BuiltInFuncMap doubleBIFuncMap;

    llvm::Type * fTy = NULL;
    llvm::Type * dTy = NULL;

    stack<Variable *> constList;
    stack<Variable *> smallArrayList;
    uint64_t *maxColumnDepth=0;
    stack<TableColumn *> columnList;


    std::map<opCodeEn, int> BIF2LLVMmap;

};


class TableGenContext
{
public:
    TableGenContext (Table *arg) { table =arg; }
    ~TableGenContext() {}
    
    uint64_t          getUniqueIndex () { uniqueNameCounter++; return (uniqueNameCounter -1); }
    void              setUint(Variable * var) { table->setUint(var);};

private:

    Table * table;
    uint64_t uniqueNameCounter=0;
};



#endif
