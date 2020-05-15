#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include "variable.h"
#include "types_jty.h"



class Operation :public Variable
{
public:
	std::string arSym[14] = { "+","+.","-","-.","*","*.","/","/","/.","%","%","%.","**","**." };
	std::string arTConv[9] = { "trunc","zext","sext","fptrunc","fpext","fptoi","fptosi","uitofp","sitofp" };
	std::string arBuiltIn[5] = { "log" , "log10" , "cos" ,"sin" , "exp" };

	Operation() { opCode = opCodeEn::NONE_op;  }

	Operation(opCodeEn op, Variable* var, TypeEn targetType, int64_t intVal=0) {
		CommonSetup(op, var);
		type = targetType;

		if (op == opCodeEn::shift){
			auto v=var->getLength()+ var->getDercimation()*intVal;
			shiftParameter=intVal;
		}
		else if (op == opCodeEn::decimation) {
			bufferLength=
			decimationParameter=intVal;
		}

		operand.push_back(var); 
	}

	Operation(opCodeEn op, Variable* var1, Variable* var2, int64_t shift=0) {
		CommonSetup(op, maxDS(var1, var2));
		type = var1->type;
		shiftParameter = shift;
		operand.push_back(var1);
		operand.push_back(var2);
	}

	Operation(opCodeEn op, Variable* var1, Variable* var2, Variable* var3, TypeEn targetType) {
		CommonSetup(op, maxDS(var1, var2));
		type = targetType;
		operand.push_back(var1);
		operand.push_back(var2);
		operand.push_back(var3);
	}


	void CommonSetup(opCodeEn op, Variable* var) {
		opCode = op;
		dstype = var->dstype;
		length = var->getLength();
	}


	int64_t getSliceParameter() {
		if (opCode == opCodeEn::shift)
			return shiftParameter;
		else if (opCode == opCodeEn::decimation)
			return decimationParameter;
		else 
			print_error("getSliceParameter");
	}

	//safe functions .external stack is used
	virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override;
	virtual void visitEnter(stack<Variable*>* visitorStack )override;
	virtual void visitExit( stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr = NULL)override;
	virtual void visitExit( stack<std::string>* Stack  )override;

	//dangerous functions . used reccursive call
	virtual std::string Print() override;
	virtual Variable* generate() override;

private:
	std::vector<Variable*> operand;
	opCodeEn opCode = opCodeEn::NONE_op;

	// convolve params
	int64_t shiftParameter = 0; 
	int64_t decimationParameter = 1;
};



template< typename T >
uint64_t calcArithmeticOperation(T arg1, T arg2, opCodeEn uTypeOp)   // объ€вление и определение
{
	T value;
	uint64_t binaryValue = 0;
#define CONV_OP(depend,target) case (depend):  value=(target) ;  break
	switch (uTypeOp)
	{
		CONV_OP(opCodeEn::ADD,  arg1 + arg2);
		CONV_OP(opCodeEn::SUB,  arg1 - arg2);
		CONV_OP(opCodeEn::MUL,  arg1 * arg2);
		CONV_OP(opCodeEn::SDIV, arg1 / arg2);
		CONV_OP(opCodeEn::SREM,(T) fmod(arg1 ,arg2));
		CONV_OP(opCodeEn::POW, (T) pow((double)arg1,arg2) );
	}
	*((T*)(&binaryValue)) = value;
#undef CONV_OP
	return binaryValue;
}



Variable* newBuiltInFuncOperation(TypeEn targetType, Variable* arg1, opCodeEn uTypeOp);
Variable* newArithmeticOperation(TypeEn targetType, Variable* arg1, Variable* arg2, opCodeEn uTypeOp);
Variable* newConvolveOperation(TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift=0, opCodeEn uTypeOp = opCodeEn::convolve);
Variable* newTypeConvOp(TypeEn targetType, Variable* arg1);
Variable* newSelectOp(TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3);
Variable* newSliceOp(Variable* arg1, Variable* arg2, opCodeEn uTypeOp);



#endif