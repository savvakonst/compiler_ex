#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "common.h"

using std::string;
class Variable;
class Module;
class LLVMContext;




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


private:
    uint64_t      length;
    stack<Block*> blockList;
};




class Table
{
public:
    Table () {}
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

    string  print();

    void setUint(Variable * var);

private:
    Module* M, 
    LLVMContext& Context


    stack<Variable *> constList;
    stack<Variable *> smallArrayList;
    uint64_t *maxColumnDepth=0;
    stack<TableColumn *> columnList;
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
