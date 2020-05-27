#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
//#include "variable.h"
//#include "line.h"
//#include "operations.h"

class Variable;

class Block
{
public:
	Block () {}
	~Block() {}
	uint64_t getStage () { return stage;  };
	uint64_t getLength() { return length; };
	void setVariable(Variable * var);

private:
	std::vector<Variable*> units;
	uint64_t stage;
	uint64_t length;

};



class TableColumn 
{
public:
	TableColumn () {}
	~TableColumn() {}
	uint64_t getLength(){ return length; }
	void setVariable(Variable * var);
private:
	uint64_t length;
	std::vector<Block*> Row;
};




class Table
{
public:
	Table () {}
	~Table() {}

	bool containsColumn(uint64_t length) {
		for (auto i : columns)
			if (i->getLength() == length) 
				return true;
		return false;
	}

	TableColumn * getColumn(uint64_t length){
		for (auto i : columns)
			if (i->getLength() == length) {
				return i;
			}
		return NULL;

	}

	void setVariable(Variable * var);


private:
	std::vector<TableColumn *> columns;
};


class TableGenContext
{
public:
	TableGenContext () {}
	~TableGenContext() {}

private:

};



#endif
