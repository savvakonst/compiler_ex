#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "variable.h"
#include "line.h"
#include "operations.h"



class block
{
public:

private:
	uint64_t length;

};

class tableColumn
{
public:
	uint64_t getLength() {
		return length;
	}

private:
	uint64_t length;
};

class table
{

public:
	bool containsColumn(uint64_t length) {
		for (auto i : columns)
			if (i.getLength() == length) 
				return true;
		return false;
	}
	bool getColumn(uint64_t length,	tableColumn** column_ptr ){
		for (auto i : columns)
			if (i.getLength() == length) {
				*column_ptr = &i;
				return true;
			}

		*column_ptr = NULL;
		return false;
	}

private:
	std::vector<block> Row;
	std::vector<tableColumn> columns;
};



#endif
