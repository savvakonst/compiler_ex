
#include "operations.h"
#include <string>


void print_error(std::string content);

Variable* newInvOperation(Variable* arg1){
	print_error("inverse operation is not supported yet");
	return arg1;
}

Variable* newTypeConvOp(TypeEn targetType, Variable* arg1)
{

	#define OP_LV2(T1,T2)   	 *((T1*)(&V)) = (T1 )arg1->getConvTypeVal<T2>()
	#define CONV_OP(depend,target) case (depend):  target ;  break
	
	#define OP_LV1(T)   		switch (arg1->type)  \
	{   \
		CONV_OP(TypeEn::Double_jty, OP_LV2(T,double)   ); \
		CONV_OP(TypeEn::Float_jty,  OP_LV2(T,float)    ); \
		CONV_OP(TypeEn::Int64_jty,  OP_LV2(T,int64_t)  ); \
		CONV_OP(TypeEn::Int32_jty,  OP_LV2(T,int32_t)  ); \
		CONV_OP(TypeEn::Int16_jty,  OP_LV2(T,int16_t)  ); \
		CONV_OP(TypeEn::Int8_jty,   OP_LV2(T,int8_t)   ); \
		CONV_OP(TypeEn::Int1_jty,   OP_LV2(T,bool)     ); \
	} 

	if (isConst(arg1) && !isUnknownTy(targetType)) {
		uint64_t V = 0;
		switch (targetType)
		{
			CONV_OP(TypeEn::Double_jty, OP_LV1(double)  );
			CONV_OP(TypeEn::Float_jty,  OP_LV1(float)   );
			CONV_OP(TypeEn::Int64_jty,  OP_LV1(int64_t) );
			CONV_OP(TypeEn::Int32_jty,  OP_LV1(int32_t) );
			CONV_OP(TypeEn::Int16_jty,  OP_LV1(int16_t) );
			CONV_OP(TypeEn::Int8_jty,   OP_LV1(int8_t)  );
			CONV_OP(TypeEn::Int1_jty,   OP_LV1( bool) );
		}
		return new Variable(V, targetType);
	}
	#undef CONV_OP
	#undef OP



	if (targetType == arg1->type) {
		return arg1;
	}
	else if (isFloating(targetType) && isFloating(arg1)) {
		if (targetType < arg1)
			return new Operation(opCodeEn::fptrunc, arg1, targetType);
		else
			return new Operation(opCodeEn::fpext, arg1, targetType);
	}
	else if (isFloating(targetType) && isInteger(arg1)) {
		return new Operation(opCodeEn::sitofp, arg1, targetType);;
	}
	else if (isInteger(targetType) && isFloating(arg1)) {
		return new Operation(opCodeEn::fptosi, arg1, targetType);
	}
	else //if (isInteger(targetType) && isFloating(arg1)) 
	{

	if (targetType < arg1)
		return new Operation(opCodeEn::trunc, arg1, targetType);
	else
		return new Operation(opCodeEn::sext, arg1, targetType);
	}
}

Variable* newBuiltInFuncOperation(TypeEn targetType, Variable* arg1, opCodeEn uTypeOp) {
	return new Operation(uTypeOp, arg1,targetType);
}

Variable* newArithmeticOperation(TypeEn targetType, Variable* arg1, Variable* arg2, opCodeEn uTypeOp){

	opCodeEn opType = opCodeEn::ADD;

	if (!isÑompatible(arg1, arg2))
		print_error("uncompatible values");


	#define CONV_OP(depend,target) case (depend):  (target) ;  break
	#define OP(T)   calcArithmeticOperation<T> ( arg1->getConvTypeVal<T>(),arg2->getConvTypeVal<T>(),uTypeOp )
	if (isConst(arg1) && isConst(arg2)&& !isUnknownTy(targetType) ) {
		uint64_t V=0;
		switch (targetType)
		{
			CONV_OP(TypeEn::Double_jty, V =OP(double));
			CONV_OP(TypeEn::Float_jty,  V =OP(float));
			CONV_OP(TypeEn::Int64_jty,  V =OP(int64_t));
			CONV_OP(TypeEn::Int32_jty,  V =OP(int32_t));
			CONV_OP(TypeEn::Int16_jty,  V =OP(int16_t));
			CONV_OP(TypeEn::Int8_jty,   V =OP(int8_t));
			CONV_OP(TypeEn::Int1_jty,   print_error(" Int1_jty ") );
		}
		return new Variable(V, targetType);
	}
	#undef CONV_OP
	#undef OP

	#define CONV_OP(depend,target) case (depend):  opType=(target) ;  break
	if (isInteger(targetType)||isUnknownTy(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(opCodeEn::ADD,  opCodeEn::ADD);
			CONV_OP(opCodeEn::SUB,  opCodeEn::SUB);
			CONV_OP(opCodeEn::MUL,  opCodeEn::MUL);
			CONV_OP(opCodeEn::SDIV, opCodeEn::SDIV);
			CONV_OP(opCodeEn::SREM, opCodeEn::SREM);
			CONV_OP(opCodeEn::POW,  opCodeEn::POW);
		}
	}
	else if (isFloating(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(opCodeEn::ADD, opCodeEn::FADD);
			CONV_OP(opCodeEn::SUB, opCodeEn::FSUB);
			CONV_OP(opCodeEn::MUL, opCodeEn::FMUL);
			CONV_OP(opCodeEn::SDIV, opCodeEn::FDIV);
			CONV_OP(opCodeEn::SREM, opCodeEn::FREM);
			CONV_OP(opCodeEn::POW, opCodeEn::FPOW);
		}
	}
	else {
		print_error("newArithmeticOperation - unsigned is not supported yet");

	}
	#undef CONV_OP
	return new Operation(opType, arg1, arg2);
}


Variable* newConvolveOperation(TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift, opCodeEn uTypeOp ) {


	if (uTypeOp!= opCodeEn::convolve)
		print_error("convolve_f operation is not supported yet");

	if (isConst(arg1) || isConst(arg2)) {
		return newArithmeticOperation(targetType, arg1, arg2, opCodeEn::MUL);
	}
	else if (isSmallArr(arg1) && isSmallArr(arg2)) {
		print_error("convolve(SmallArr_t,SmallArr_t) - is not supported yet");
	}
	else if (isSmallArr(arg1) || isSmallArr(arg2)) {
		if (isSmallArr(arg2))
			return new Operation(uTypeOp, arg1, arg2, shift);
		else
			return new Operation(uTypeOp, arg2, arg1, shift);
	}
	else if (isLargeArr(arg1) && isLargeArr(arg2)) {
		print_error("convolve(LargeArr_t,LargeArr_t) - is not supported");
	}
	

	return new Operation(uTypeOp, arg1, arg2);
}


Variable* newSelectOp(TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3)
{
	if (!isÑompatible(arg2, arg3) ||!isÑompatible(arg1, arg2))
		print_error("uncompatible values");

	auto i1=newTypeConvOp(TypeEn::Int1_jty, arg1) ;
	if (isConst(i1))
		if (i1->getConvTypeVal<bool>())
			return arg2;
		else
			return arg3;

	return new Operation(opCodeEn::select, i1, arg2, arg3, targetType);
}

Variable* newSliceOp(Variable* arg1, Variable* arg2, opCodeEn uTypeOp) {
	if (!(isConst(arg2) && isInteger(arg2)))
		print_error("(arr,int) - second argument must be integer consant");
	int64_t intVal =arg2->getBinaryValue();
	return new Operation(uTypeOp, arg1, arg1->type, intVal);
}

Variable* newSliceOp(Variable* arg1, int64_t intVal , opCodeEn uTypeOp) {
	return new Operation(uTypeOp, arg1, arg1->type, intVal);
}



//safe functions .external stack is used
void Operation::markUnusedVisitEnter(stack<Variable*>* visitorStack) {
	commonVisitEnter(visitorStack);
	if (isArithetic(opCode)) {
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isInv(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isTypeConv(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isBuiltInFunc(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isSelect(opCode)) {
		visitorStack->push(operand[2]);
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isConvolve(opCode)) {
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isConvolve(opCode)) {
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isSlice(opCode)) {
		visitorStack->push(operand[0]);
	}
	else {
		print_error("visitEnter unknown command");
	}
	is_unused = false;
}


void Operation::visitEnter(stack<Variable*>* visitorStack)
{
	is_visited = true;
	visitorStack->push(this);
	if (isArithetic(opCode)) {
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isInv(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isTypeConv(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isBuiltInFunc(opCode)) {
		visitorStack->push(operand[0]);
	}
	else if (isSelect(opCode)) {
		visitorStack->push(operand[2]);
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isConvolve(opCode)) {
		visitorStack->push(operand[1]);
		visitorStack->push(operand[0]);
	}
	else if (isSlice(opCode)) {
		visitorStack->push(operand[0]);
	}
	else {
		print_error("visitEnter unknown command");
	}
}


void Operation::visitExit( stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr )
{

	is_visited = false;
	Variable* ret = NULL;
	if (isArithetic(opCode)) {
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		
		if ((op2 == NULL) || (op1 == NULL)) {
			auto txtOperation = arSym[((int)opCode - (int)typeOpCodeEn::arithetic)];
			print_error(
				"Error : " + txtOperation + ". In line : " + std::to_string(namespace_ptr->size() ) + ".  "
				"Left operand:" + ((op2 != NULL) ? op2->Print() : "unknown") + ".  " +
				"Right operand:" + ((op1 != NULL) ? op1->Print() : "unknown") 
			);
		}


		TypeEn targetType = max(op1, op2)->type;
		
		ret = newArithmeticOperation(targetType, newTypeConvOp(targetType, op1), newTypeConvOp(targetType, op2),  (opCodeEn)(int)opCode );
	}
	else if (isInv(opCode)) {
		auto op1 = Stack->pop();
		ret = newInvOperation(op1);
	}
	else if (isTypeConv(opCode)) {
		auto op1 = Stack->pop();
		ret = newTypeConvOp(type, op1);
	}
	else if (isBuiltInFunc(opCode)) {
		auto op1 = Stack->pop();

		if (TypeEn::Float_jty > op1->type)
			op1 = newTypeConvOp(TypeEn::Float_jty, op1);
		TypeEn targetType = op1->type;
		ret = newBuiltInFuncOperation(type, op1 , opCode);
	}
	else if (isSelect(opCode)) {
		auto op3 = Stack->pop();
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		TypeEn targetType = max(op2, op3)->type;
		ret = newSelectOp(targetType, op1, newTypeConvOp(targetType, op2), newTypeConvOp(targetType, op3));
	}
	else if(isConvolve(opCode)){
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		TypeEn targetType = max(op1, op2)->type;
		ret = newConvolveOperation(targetType, newTypeConvOp(targetType, op1), newTypeConvOp(targetType, op2), shiftParameter, opCode);
		//print_error("visitExit convolve operation is unsupported");
	}
	else if (isSlice(opCode)) {
		auto op1 = Stack->pop();
		ret =newSliceOp(op1,getSliceParameter(),opCode);
	}
	else {
		print_error("visitExit unknown command . In line : " +std::to_string(namespace_ptr->size() ) );
	}
	Stack->push(ret);




}

void Operation::visitExit(stack<std::string>* Stack)
{
	is_visited = false;
	std::string txtOperation = "";
	if (isArithetic(opCode)) {
		txtOperation = arSym[((int)opCode - (int)typeOpCodeEn::arithetic)];
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		Stack->push( "(" + op1 + txtOperation + op2 + ")" );
	}
	else if (isInv(opCode)) {
		Stack->push("(-" + Stack->pop() + ")");
	}
	else if (isTypeConv(opCode)) {
		txtOperation = arTConv[((int)opCode - (int)typeOpCodeEn::typeConv)];
		Stack->push(txtOperation + "(" + Stack->pop() + ")");
	}
	else if (isBuiltInFunc(opCode)) {
		txtOperation = arBuiltIn[((int)opCode - (int)typeOpCodeEn::builtInFunc)];
		Stack->push(txtOperation + "(" + Stack->pop() + ")");
	}
	else if (isSelect(opCode)) {
		auto op3 = Stack->pop();
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		Stack->push("(" + op1 +"? "+ op2 + ": "+ op3 + ")");
	}
	else if (isConvolve(opCode)) {
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		Stack->push("convolve(" + op1 + ", " + op2 + "," + std::to_string(shiftParameter) + ")");
		//print_error("visitExitTxt convolve operation is unsupported");
	}
	else if (isSlice(opCode)) {
		auto op1 = Stack->pop();
		if (opCode == opCodeEn::shift)
			Stack->push("shift(" + op1 + ", "  + std::to_string(getSliceParameter()) + ")");
		else if (opCode == opCodeEn::decimation)
			Stack->push("decimation(" + op1 + ", " + std::to_string(getSliceParameter()) + ")");
		else
			print_error("visitExitTxt unknown command .");
	}
	else {
		print_error("visitExitTxt unknown command .");
	}
}


//dangerous functions . used reccursive call 

Variable* Operation::generate() {
	if (isArithetic(opCode)) {

		Variable* op1 = operand[0]->generate();
		Variable* op2 = operand[1]->generate();

		TypeEn targetType = max(op1, op2)->type;
		return newArithmeticOperation(targetType, newTypeConvOp(targetType, op1), newTypeConvOp(targetType, op2),  opCode );
	}
	else if (isInv(opCode)) {
		return newInvOperation(operand[0]->generate());
	}
	else if (isTypeConv(opCode)) {
		return newTypeConvOp(type, operand[0]->generate());
	}
	return NULL;
};

 std::string Operation::Print()  {
	std::string txtOperation = "";
	if (isArithetic(opCode)) {
		txtOperation = arSym[((int)opCode - (int)typeOpCodeEn::arithetic)];
		return "(" + operand[0]->Print() + txtOperation + operand[1]->Print() + ")";
	}
	else if (isInv(opCode)) {
		return "(-" + operand[0]->Print() + ")";
	}
	else if (isTypeConv(opCode)) {
		txtOperation = arTConv[((int)opCode - (int)typeOpCodeEn::typeConv)];
		return txtOperation + "(" + operand[0]->Print() + ").";
	}
	else return "error";
};