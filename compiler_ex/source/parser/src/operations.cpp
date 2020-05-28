
#include "operations.h"
#include <string>


void print_error(std::string content);


//void Operation::setBufferLength(uint64_t left, uint64_t right) {
//	leftBufferLength=left;
//	rightBufferLength=right;
//}
//
//void Operation::setBufferLength(Variable * var) {
//	if (isLargeArr(this)) {
//		leftBufferLength =var->getLeftShift();
//		rightBufferLength =var->getRightShift();
//	}
//}


void Operation::visitEnterSetupBuffer(stack<Variable*>* visitorStack){
	if (isConvolve(opCode)) {
		auto smallArray = operand[1];
		auto shift = shiftParameter;

		int64_t len = smallArray->getLength();
		int64_t le  = len / 2;
		int64_t ri  = (len - 1) / 2;

		auto left   =this->getLeftBufferLen() + ((shift < 0) ? 0 : shift) + le;
		auto right  =this->getRightBufferLen() + ((shift > 0) ? 0 : -shift) + ri;

		operand[0]->setBuffered();
		operand[0]->setBufferLength(left, right);
	}
	else if (isShift(opCode)) {
		auto shift  = shiftParameter;

		auto left   = this->getLeftBufferLen() + ((shift < 0) ? 0 : shift) ;
		auto right  = this->getRightBufferLen() + ((shift > 0) ? 0 : -shift) ;

		operand[0]->setBuffered();
		operand[0]->setBufferLength(left, right);
	}
	else if (isDecimation(opCode)) {
		//print_error("decimation operation is not supported yet");

		operand[0]->setBuffered();
		operand[0]->setBufferLength(this);
	}
	else {
		for (auto i : operand) {
			i->setBufferLength(this);
		}
	}
}




void Operation::visitEnterStackUpdate( stack<Variable*>* visitorStack ){
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


void Operation::markUnusedVisitEnter(stack<Variable*>* visitorStack){
	commoMmarkUnusedVisitEnter(visitorStack);
	visitEnterSetupBuffer(visitorStack);
	visitEnterStackUpdate(visitorStack);
	is_unused = false;
}

void Operation::genBlocksVisitExit(TableGenContext * context)
{
	uniqueName =(isLargeArr(this)?"vb" :"vs") + std::to_string(context->getUniqueIndex());
	context->setUint(this);
	is_visited = false;
}
/*
void Operation::genBlocksVisitEnter (stack<Variable*>* visitorStack){
	is_visited = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}
*/
void Operation::visitEnter(stack<Variable*>* visitorStack){
	is_visited = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}


void Operation::genBodyVisitExit( stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr ){

	is_visited = false;
	Variable* ret = NULL;
	if (isArithetic(opCode)) {
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		
		if ((op2 == NULL) || (op1 == NULL)) {
			auto txtOperation = arSym[((int)opCode - (int)typeOpCodeEn::arithetic)];
			print_error(
				"Error : " + txtOperation + ". In line : " + std::to_string(namespace_ptr->size() ) + ".  "
				"Left operand:" + ((op2 != NULL) ? "a" /*op2->Print()*/ : "unknown") + ".  " +
				"Right operand:" + ((op1 != NULL) ? "a" /*op1->Print()*/ : "unknown") 
			);
		}
		TypeEn targetType = max(op1, op2)->getType();
		
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

		if (TypeEn::Float_jty > op1->getType())
			op1 = newTypeConvOp(TypeEn::Float_jty, op1);
		TypeEn targetType = op1->getType();
		ret = newBuiltInFuncOperation(type, op1 , opCode);
	}
	else if (isSelect(opCode)) {
		auto op3 = Stack->pop();
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		TypeEn targetType = max(op2, op3)->getType();
		ret = newSelectOp(targetType, op1, newTypeConvOp(targetType, op2), newTypeConvOp(targetType, op3));
	}
	else if(isConvolve(opCode)){
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		TypeEn targetType = max(op1, op2)->getType();
		ret = newConvolveOperation(targetType, newTypeConvOp(targetType, op1), newTypeConvOp(targetType, op2), shiftParameter, opCode);
		//print_error("visitExit convolve operation is unsupported");
	}
	else if (isSlice(opCode)) {
		auto op1 = Stack->pop();
		ret =newSliceOp(op1,getSliceParameter(),opCode);
	}
	else if (isStoreToBuffer(opCode)) {
		print_error("visitExitTxt StoreToBuffer unknown command .");
	}
	else {
		print_error("visitExit unknown command . In line : " +std::to_string(namespace_ptr->size() ) );
	}
	Stack->push(ret);
}



void Operation::printVisitExit(stack<std::string>* Stack) {
	is_visited = false;
	std::string txtOperation = "";
#define OP(i) (operand[(i)]->getUniqueName() )

	if (isArithetic(opCode)) {
		auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("(" + op1 + txtArOp(opCode) + op2 + ")" ));
	}
	else if (isInv(opCode)) {
		Stack->push(checkBuffer("(-" + Stack->pop() + ")"));
	}
	else if (isTypeConv(opCode)) {
		Stack->push(checkBuffer(txtTConOp(opCode) + "(" + Stack->pop() + ")"));
	}
	else if (isBuiltInFunc(opCode)) {
		txtOperation = arBuiltIn[((int)opCode - (int)typeOpCodeEn::builtInFunc)];
		Stack->push(checkBuffer(txtBuiltInOp(opCode) + "(" + Stack->pop() + ")"));
	}
	else if (isSelect(opCode)) {
		auto op3 = Stack->pop();auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("(" + op1 +"? "+ op2 + ": "+ op3 + ")"));
	}
	else if (isConvolve(opCode)) {
		auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("convolve(" + op1 + ", " + op2 + "," + std::to_string(shiftParameter) + ")"));
	}
	else if (isSlice(opCode)) {
		Stack->push(checkBuffer(txtSliceOp(opCode) +"(" + Stack->pop() + ", " + std::to_string(getSliceParameter()) + ")"));
	}
	else if (isStoreToBuffer(opCode)) {
		print_error("visitExitTxt StoreToBuffer unknown command .");
	}
	else {
		print_error("visitExitTxt unknown command .");
	}
#undef OP
}



string Operation::printUint() {
	is_visited = false;
	std::string txtOperation = "";
	std::string uName = getUniqueName();
#define OP(i) (operand[(i)]->getUniqueName() )

	if (isArithetic(opCode))          return uName + " = " + OP(0) + txtArOp(opCode) + OP(1);
	else if (isInv(opCode))           return uName + " = " + "( -" + OP(0) + ")";
	else if (isTypeConv(opCode))      return uName + " = " + txtTConOp(opCode) + "( " + OP(0) + ")";
	else if (isBuiltInFunc(opCode))   return uName + " = " + txtBuiltInOp(opCode) + "( " + OP(0) + ")";
	else if (isSelect(opCode))        return uName + " = " + OP(0) + "? " + OP(1) + ": " + OP(2);
	else if (isConvolve(opCode))      return uName + " = convolve( " + OP(0) + ", " + OP(1) + ", " + std::to_string(shiftParameter) + ")";
	else if (isSlice(opCode))         return uName + " = " + txtSliceOp(opCode) + "( " + OP(0) + ", " + std::to_string(getSliceParameter()) + ")";
	else if (isStoreToBuffer(opCode))
		print_error("visitExitTxt StoreToBuffer unknown command .");
	else print_error("visitExitTxt unknown command .");

#undef OP
}