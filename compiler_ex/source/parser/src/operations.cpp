
#include "operations.h"
#include <string>
#include <sstream>

//void print_error(std::string &content);


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


		operand[0]->getAssignedVal(true)->setBuffered();
		operand[0]->setBufferLength(left, right);
	}
	else if (isShift(opCode)) {
		auto shift  = shiftParameter;

		auto left   = this->getLeftBufferLen() + ((shift < 0) ? 0 : shift) ;
		auto right  = this->getRightBufferLen() + ((shift > 0) ? 0 : -shift) ;

		operand[0]->getAssignedVal(true)->setBuffered();
		operand[0]->setBufferLength(left, right);
	}
	else if (isDecimation(opCode)) {
		//print_error("decimation operation is not supported yet");

		operand[0]->getAssignedVal(true)->setBuffered();
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
	else if (isSmallArrayDef(opCode)) {
		for (auto i = operand.rbegin(); i != operand.rend(); i++)
			visitorStack->push(*i);
	}
	else {
		print_error("visitEnter unknown command");
	}
}


void Operation::markUnusedVisitEnter(stack<Variable*>* visitorStack){
	commoMmarkUnusedVisitEnter(visitorStack);
	visitEnterSetupBuffer(visitorStack);
	visitEnterStackUpdate(visitorStack);
	is_nused_ = false;
}

void Operation::genBlocksVisitExit(TableGenContext * context)
{
	uniqueName_ =(isLargeArr(this)?"vb" :"vs") + std::to_string(context->getUniqueIndex());
	context->setUint(this);
	is_visited_ = false;
}

void Operation::reduceLinksVisitExit()
{
	//for (size_t i=0; i<operand.size();i++)
	//	operand[i]->getAssignedVal(true);

	is_visited_ = false;
}

/*
void Operation::genBlocksVisitEnter (stack<Variable*>* visitorStack){
	is_visited = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}
*/
void Operation::visitEnter(stack<Variable*>* visitorStack){
	is_visited_ = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}

void Operation::genBodyVisitExit( stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr ){

	is_visited_ = false;
	Variable* ret = NULL;
	if (isArithetic(opCode)) {
		auto op2 = Stack->pop();
		auto op1 = Stack->pop();
		
		if ((op2 == NULL) || (op1 == NULL)) {
			auto txtOperation = arSym[((int)opCode - (int)TypeOpCodeEn::arithetic)];
			print_error(
				txtOperation + " "
				"left operand:" + ((op1 != NULL) ? "a"  : "unknown") + ".  " +
				"right operand:" + ((op2 != NULL)? "a"  : "unknown")
			);
		}
		TypeEn targetType = max(op1, op2)->getType();
		
		ret = newArithmeticOperation(targetType, newTypeConvOp(targetType, op1), newTypeConvOp(targetType, op2),  (OpCodeEn)(int)opCode );
	}
	else if (isInv(opCode)) {
		auto op1 = Stack->pop();
		ret = newInvOperation(op1);
	}
	else if (isTypeConv(opCode)) {
		auto op1 = Stack->pop();
		ret = newTypeConvOp(type_, op1);
	}
	else if (isBuiltInFunc(opCode)) {
		auto * op1 = Stack->pop();
		if (TypeEn::float_jty > op1->getType()) {
			op1 = newTypeConvOp(TypeEn::float_jty, op1);
		}
		TypeEn targetType = op1->getType();
		ret = newBuiltInFuncOperation(targetType, op1 , opCode);
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
	}
	else if (isSlice(opCode)) {
		auto op1 = Stack->pop();
		ret =newSliceOp(op1,getSliceParameter(),opCode);
	}
	else if (isStoreToBuffer(opCode)) {
		print_error("visitExitTxt StoreToBuffer unknown command .");
	}
	else if (isSmallArrayDef(opCode)) {
		size_t size = operand.size();
		stack<Variable*> op;
		for (size_t i = 0; i < size; i++)
			op.push(Stack->pop());
		std::reverse(op.begin(), op.end());
		ret = newSmallArrayDefOp(op, opCode);
	}
	else {
		print_error("visitExit unknown command . In line : " +std::to_string(namespace_ptr->size() ) );
	}
	Stack->push(ret);
}



void Operation::printVisitExit(stack<std::string>* Stack) {
	is_visited_ = false;
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
		txtOperation = arBuiltIn[((int)opCode - (int)TypeOpCodeEn::builtInFunc)];
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
	else if (isSmallArrayDef(opCode)) {
		size_t size = operand.size();
		stack<string > op;

		for (size_t i = 0; i < size; i++)
			op.push(Stack->pop());
		std::reverse(op.begin(), op.end());

		std::string out="";
		for (auto &i : op)
			out += i + ", ";

		if (opCode == OpCodeEn::smallArrayDef)
			Stack->push("[" + out + "]");
		else
			Stack->push("range[" + out + "]");

	}else{
		print_error("visitExitTxt unknown command .");
	}
#undef OP
}


string Operation::printUint() {
	is_visited_ = false;
	std::string txtOperation = "";
	std::string uName = getUniqueName();

#define OP(i) (operand[(i)]->getAssignedVal(true)->getUniqueName() )

	if (isArithetic(opCode))          return uName + " = " + OP(0) + txtArOp(opCode) + OP(1);
	else if (isInv(opCode))           return uName + " = " + "( -" + OP(0) + ")";
	else if (isTypeConv(opCode))      return uName + " = " + txtTConOp(opCode) + "( " + OP(0) + ")";
	else if (isBuiltInFunc(opCode))   return uName + " = " + txtBuiltInOp(opCode) + "( " + OP(0) + ")";
	else if (isSelect(opCode))        return uName + " = " + OP(0) + "? " + OP(1) + ": " + OP(2);
	else if (isConvolve(opCode))      return uName + " = convolve( " + OP(0) + ", " + OP(1) + ", " + std::to_string(shiftParameter) + ")";
	else if (isSlice(opCode))         return uName + " = " + txtSliceOp(opCode) + "( " + OP(0) + ", " + std::to_string(getSliceParameter()) + ")";
	else if (isStoreToBuffer(opCode)) print_error("visitExitTxt StoreToBuffer unknown command .");
	else if (isSmallArrayDef(opCode)) {
		std::string out="";
		for (auto &i : operand)
			out += i->getAssignedVal(true)->getUniqueName() + ", ";
	
		if (opCode == OpCodeEn::smallArrayDef)
			return "[" + out + "]";
		else
			return "range[" + out + "]";

	}
	else print_error("visitExitTxt unknown command .");
	return "";
#undef OP
}



void Operation::calculate(){

	#define OP(i) (operand[(i)]->getAssignedVal(true) )

	if (isArithetic(opCode)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);

		if (aOperand->isArray() && bOperand->isArray())
			bufferPtr_=calcAritheticSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), length_);
		else if (aOperand->isArray())
			bufferPtr_=calcAritheticSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), length_);
		else
			bufferPtr_=calcAritheticSmallArray(opCode, type_, bufferPtr_, aOperand->getBinaryValue(), bOperand->getBufferPtr(), length_);
	}
	else if (isInv(opCode))
		bufferPtr_=invAritheticSmallArray(type_, bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isTypeConv(opCode))
		bufferPtr_=typeConvSmallArray(type_, OP(0)->getType(), bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isBuiltInFunc(opCode))
		bufferPtr_=builtInFuncSmallArray(opCode, type_, bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isSelect(opCode)){
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		auto cOperand = OP(2);
		if (bOperand->isArray() && cOperand->isArray())
			bufferPtr_=calcSelectSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBufferPtr(), length_);
		else if (bOperand->isArray())
			bufferPtr_=calcSelectSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBinaryValue(), length_);
		else if (cOperand->isArray())
			bufferPtr_=calcSelectSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBufferPtr(), length_);
		else 
			bufferPtr_=calcSelectSmallArray(opCode, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBinaryValue(), length_);
	}
	else if (isConvolve(opCode)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		bufferPtr_=calcConvolveSmallArray(type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), aOperand->getLength(), bOperand->getLength());
	}
	else if (isSlice(opCode))         return;
	else if (isStoreToBuffer(opCode)) return;
	else if (isSmallArrayDef(opCode)) {
		if (opCode == OpCodeEn::smallArrayDef)
			bufferPtr_=calcSmallArrayDef(type_, operand);
		else
			smallArrayGen();
	}

}




void Operation::smallArrayGen() {
	size_t argsSize = operand.size();
	if (argsSize == 1)       smallArrayGen(operand[0]);
	else if (argsSize == 2)  smallArrayGen(operand[0], operand[1]);
	else if (argsSize == 3)  smallArrayGen(operand[0], operand[1], operand[2]);
}

void Operation::smallArrayGen(double stop , double start ){

	//double start = 0;//arg1->getDoubleValue();
	//double stop  = 0;// arg2->getDoubleValue();

	//size_t argsSize = operand.size();

	double delta=0.0;
	if (isInteger(type_)) {
		delta = (stop - start) / length_;
	}

	bufferPtr_=calcSmallArrayAlloc(type_, length_);

#define OP(T) {\
    T* point=(T*)bufferPtr_;\
    for (uint64_t i=0; i < length_; i++)  \
        *point++ =(T)(start + delta * i);}

SWITCH_TYPE_OP(type_, print_SA_error("samallarray range ");)
#undef OP
};



void Operation::smallArrayGen(Variable* arg1, Variable* arg2, Variable* arg3) {
	if ((isConst(arg1) && isConst(arg2) && isConst(arg3) && isInteger(arg3))) {

		double start     = arg1->getDoubleValue();
		double stop      = arg2->getDoubleValue();

		dsType_    = DataStructTypeEn::smallArr_dsty;
		type_      = TypeEn::double_jty;
		length_    = arg3->getBinaryValue();

		smallArrayGen(start, stop);
	}
	else {
		print_error("range(start_num,stop_num,length) -signature is not supported yet");
		return;
	}
};

void Operation::smallArrayGen(Variable* arg1, Variable* arg2) {
	if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2)) {

		double start     = arg1->getDoubleValue();
		double stop      = arg2->getDoubleValue();

		dsType_    = DataStructTypeEn::smallArr_dsty;
		type_      = TypeEn::int64_jty;
		length_    = (uint64_t)(stop - start);

		smallArrayGen(start, stop);
	}
	else {
		print_error("range(start_num,stop_num) - arg must be integer consant");
	}
};

void Operation::smallArrayGen(Variable* arg1) {
	if (isConst(arg1) && isInteger(arg1)) {

		double start     = 0;
		double stop      = arg1->getBinaryValue();
		
		dsType_    = DataStructTypeEn::smallArr_dsty;
		type_      = TypeEn::int64_jty;
		length_    = (uint64_t)stop;

		smallArrayGen(start, stop);
	}
	else {
		print_error("range(len) - arg must be integer consant");
	}
};