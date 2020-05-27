#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "common.h"


class Variable;

class Block
{
public:

    Block (uint64_t l) { 
        level =l; 
    }

    Block (Variable* var) {
        level=var->getLevel(); 
        setUint(var);
    }

    ~Block() {}

    uint64_t getLevel () { return level;  };
    uint64_t getLength() { return length; };
    void     setUint(Variable * var);

private:
    stack<Variable*> unitList;
    uint64_t level;
    uint64_t length;

};



class TableColumn 
{
public:
    TableColumn (uint64_t len) { length =len; }
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

    void setUint(Variable * var);


private:
    stack<TableColumn *> columnList;
};


class TableGenContext
{
public:
    TableGenContext () {}
    ~TableGenContext() {}

private:

};



#endif
