
#include "operations.h"
#include <string>
#include <sstream>
#include <set>
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
	if (isConvolve(op_code_)) {
		auto smallArray = operand_[1];
		auto shift = shift_parameter_;

		int64_t len = smallArray->getLength();
		int64_t le  = len / 2;
		int64_t ri  = (len - 1) / 2;

		auto left   =this->getLeftBufferLen() + ((shift < 0) ? 0 : shift) + le;
		auto right  =this->getRightBufferLen() + ((shift > 0) ? 0 : -shift) + ri;


		operand_[0]->getAssignedVal(true)->setBuffered();
		operand_[0]->setBufferLength(left, right);
	}
	else if (isShift(op_code_)) {
		auto shift  = shift_parameter_;

		auto left   = this->getLeftBufferLen() + ((shift < 0) ? 0 : shift) ;
		auto right  = this->getRightBufferLen() + ((shift > 0) ? 0 : -shift) ;

		operand_[0]->getAssignedVal(true)->setBuffered();
		operand_[0]->setBufferLength(left, right);
	}
	else if (isDecimation(op_code_)) {
		//print_error("decimation operation is not supported yet");

		operand_[0]->getAssignedVal(true)->setBuffered();
		operand_[0]->setBufferLength(this);
	}
	else {
		for (auto i : operand_) {
			i->setBufferLength(this);
		}
	}
}


void Operation::visitEnterStackUpdate( stack<Variable*>* visitorStack ){
	if (isArithetic(op_code_)) {
		visitorStack->push(operand_[1]);
		visitorStack->push(operand_[0]);
	}
	else if (isComparsion(op_code_)) {
		visitorStack->push(operand_[1]);
		visitorStack->push(operand_[0]);
	}
	else if (isInv(op_code_)) {
		visitorStack->push(operand_[0]);
	}
	else if (isTypeConv(op_code_)) {
		visitorStack->push(operand_[0]);
	}
	else if (isBuiltInFunc(op_code_)) {
		visitorStack->push(operand_[0]);
	}
	else if (isSelect(op_code_)) {
		visitorStack->push(operand_[2]);
		visitorStack->push(operand_[1]);
		visitorStack->push(operand_[0]);
	}
	else if (isConvolve(op_code_)) {
		visitorStack->push(operand_[1]);
		visitorStack->push(operand_[0]);
	}
	else if (isSlice(op_code_)) {
		visitorStack->push(operand_[0]);
	}
	else if (isSmallArrayDef(op_code_)) {
		for (auto i = operand_.rbegin(); i != operand_.rend(); i++)
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

	PRMTypesEn RPMType=JITType2PRMType(type_);

	if (isSelect(op_code_)	 || 
		isArithetic(op_code_)  || 
		isComparsion(op_code_) ||
		isBuiltInFunc(op_code_)||
		isSelect(op_code_)	 ||
		isConvolve(op_code_)) {

		std::vector<SyncParameter *> p_list;
		for (auto i : operand_) p_list.push_back(i->getAssignedVal(true)->getPatameter());
		parameter_=intersection(p_list, RPMType, "");
	}
	else if (isTypeConv(op_code_) ) {
		parameter_ =retyping(operand_.front()->getAssignedVal(true)->getPatameter(), RPMType, "");
	}
	else if (isSlice(op_code_)) {

	}

	context->setParameter(parameter_);

	if (isReturned()) {
		parameter_ =new SyncParameter("", parameter_->getMainTimeInterval(), parameter_->getDataIntervalList(),false);
		context->setParameter(parameter_);
	}

	if (parameter_!=nullptr)
		context->setParameter(parameter_);
}


void Operation::visitEnter(stack<Variable*>* visitorStack){
	is_visited_ = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}


void Operation::genBodyVisitExit(BodyGenContext* context){
	GarbageContainer * garbage_container =context->getGarbageContainer();
	g_pos = pos;

	is_visited_ = false;
	Variable* ret = nullptr;
	if (isArithetic(op_code_)) {
		auto op2 = context->pop();
		auto op1 = context->pop();
		
		if ((op2 == nullptr) || (op1 == nullptr)) {
			auto txtOperation = txtArOp(op_code_);
			print_error(
				txtOperation + " "
				"left operand:" + ((op1 != nullptr) ? "a"  : "unknown") + ".  " +
				"right operand:" + ((op2 != nullptr)? "a"  : "unknown")
			);
		}
		TypeEn targetType = maxTypeVar(op1, op2)->getType();
		
		ret = newArithmeticOperation(
			garbage_container,
			targetType, 
			newTypeConvOp(garbage_container, targetType, op1), 
			newTypeConvOp(garbage_container, targetType, op2),  
			(OpCodeEn)(int)op_code_);

	}
	else if (isComparsion(op_code_)) {
		auto op2 = context->pop();
		auto op1 = context->pop();

		if ((op2 == nullptr) || (op1 == nullptr)) {
			auto txtOperation =txtCompOp(op_code_);
			print_error(
				txtOperation + " "
				"left operand:" + ((op1 != nullptr) ? "a" : "unknown") + ".  " +
				"right operand:" + ((op2 != nullptr) ? "a" : "unknown")
			);
		}
		TypeEn targetType = maxTypeVar(op1, op2)->getType();

		ret = newComparsionOperation(
			garbage_container,
			targetType,
			newTypeConvOp(garbage_container, targetType, op1),
			newTypeConvOp(garbage_container, targetType, op2),
			(OpCodeEn)(int)op_code_);

	}
	else if (isInv(op_code_)) {
		auto op1 = context->pop();
		ret = newInvOperation(garbage_container, op1);
	}
	else if (isTypeConv(op_code_)) {
		auto op1 = context->pop();
		ret = newTypeConvOp(garbage_container, type_, op1);
	}
	else if (isBuiltInFunc(op_code_)) {
		auto * op1 = context->pop();
		if (TypeEn::float_jty > op1->getType()) {
			op1 = newTypeConvOp(garbage_container, TypeEn::float_jty, op1);
		}
		TypeEn targetType = op1->getType();
		ret = newBuiltInFuncOperation(garbage_container, targetType, op1 , op_code_);
	}
	else if (isSelect(op_code_)) {
		auto op3 = context->pop();
		auto op2 = context->pop();
		auto op1 = context->pop();
		TypeEn targetType = maxTypeVar(op2, op3)->getType();
		ret = newSelectOp(
			garbage_container, 
			targetType, 
			op1, 
			newTypeConvOp(garbage_container, targetType, op2), 
			newTypeConvOp(garbage_container, targetType, op3));
	}
	else if(isConvolve(op_code_)){
		auto op2 = context->pop();
		auto op1 = context->pop();
		TypeEn targetType = maxTypeVar(op1, op2)->getType();
		ret = newConvolveOperation(
			garbage_container, 
			targetType, 
			newTypeConvOp(garbage_container, targetType, op1), 
			newTypeConvOp(garbage_container, targetType, op2), 
			shift_parameter_, 
			op_code_);
	}
	else if (isSlice(op_code_)) {
		auto op1 = context->pop();
		ret =newSliceOp(garbage_container, op1,getSliceParameter(),op_code_);
	}
	else if (isStoreToBuffer(op_code_)) {
		print_error("visitExitTxt StoreToBuffer unknown command .");
	}
	else if (isSmallArrayDef(op_code_)) {
		size_t size = operand_.size();
		stack<Variable*> op;
		for (size_t i = 0; i < size; i++)
			op.push(context->pop());
		std::reverse(op.begin(), op.end());
		ret = newSmallArrayDefOp(garbage_container, op, op_code_);
	}
	else {
		print_error("visitExit unknown command . In line : " +std::to_string(context->getNamespace().size() ) );
	}
	context->push(ret);

}


void Operation::printVisitExit(stack<std::string>* Stack) {
	is_visited_ = false;
	std::string txtOperation = "";
#define OP(i) (operand_[(i)]->getUniqueName() )

	if (isArithetic(op_code_)) {
		auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("(" + op1 + txtArOp(op_code_) + op2 + ")" ));
	}
	else if (isComparsion(op_code_)) {
		auto op2 = Stack->pop(); auto op1 = Stack->pop();
		Stack->push(checkBuffer("(" + op1 + txtCompOp(op_code_) + op2 + ")"));
	}
	else if (isInv(op_code_)) {
		Stack->push(checkBuffer("(-" + Stack->pop() + ")"));
	}
	else if (isTypeConv(op_code_)) {
		Stack->push(checkBuffer(txtTConOp(op_code_) + "(" + Stack->pop() + ")"));
	}
	else if (isBuiltInFunc(op_code_)) {
		txtOperation = arBuiltIn[((int)op_code_ - (int)TypeOpCodeEn::builtInFunc)];
		Stack->push(checkBuffer(txtBuiltInOp(op_code_) + "(" + Stack->pop() + ")"));
	}
	else if (isSelect(op_code_)) {
		auto op3 = Stack->pop();auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("(" + op1 +"? "+ op2 + ": "+ op3 + ")"));
	}
	else if (isConvolve(op_code_)) {
		auto op2 = Stack->pop();auto op1 = Stack->pop();
		Stack->push(checkBuffer("convolve(" + op1 + ", " + op2 + "," + std::to_string(shift_parameter_) + ")"));
	}
	else if (isSlice(op_code_)) {
		Stack->push(checkBuffer(txtSliceOp(op_code_) +"(" + Stack->pop() + ", " + std::to_string(getSliceParameter()) + ")"));
	}
	else if (isStoreToBuffer(op_code_)) {
		print_error("visitExitTxt StoreToBuffer unknown command .");
	}
	else if (isSmallArrayDef(op_code_)) {
		size_t size = operand_.size();
		stack<string > op;

		for (size_t i = 0; i < size; i++)
			op.push(Stack->pop());
		std::reverse(op.begin(), op.end());

		std::string out="";
		for (auto &i : op)
			out += i + ", ";

		if (op_code_ == OpCodeEn::smallArrayDef)
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

#define OP(i) (operand_[(i)]->getAssignedVal(true)->getUniqueName() )

	if (isArithetic(op_code_))          return uName + " = " + OP(0) + txtArOp(op_code_) + OP(1);
	if (isComparsion(op_code_))          return uName + " = " + OP(0) + txtCompOp(op_code_) + OP(1);
	else if (isInv(op_code_))           return uName + " = " + "( -" + OP(0) + ")";
	else if (isTypeConv(op_code_))      return uName + " = " + txtTConOp(op_code_) + "( " + OP(0) + ")";
	else if (isBuiltInFunc(op_code_))   return uName + " = " + txtBuiltInOp(op_code_) + "( " + OP(0) + ")";
	else if (isSelect(op_code_))        return uName + " = " + OP(0) + "? " + OP(1) + ": " + OP(2);
	else if (isConvolve(op_code_))      return uName + " = convolve( " + OP(0) + ", " + OP(1) + ", " + std::to_string(shift_parameter_) + ")";
	else if (isSlice(op_code_))         return uName + " = " + txtSliceOp(op_code_) + "( " + OP(0) + ", " + std::to_string(getSliceParameter()) + ")";
	else if (isStoreToBuffer(op_code_)) print_error("visitExitTxt StoreToBuffer unknown command .");
	else if (isSmallArrayDef(op_code_)) {
		std::string out="";
		for (auto &i : operand_)
			out += i->getAssignedVal(true)->getUniqueName() + ", ";
	
		if (op_code_ == OpCodeEn::smallArrayDef)
			return "[" + out + "]";
		else
			return "range[" + out + "]";

	}
	else print_error("visitExitTxt unknown command .");
	return "";
#undef OP
}



void Operation::calculate(){

#define OP(i) (operand_[(i)]->getAssignedVal(true) )

	if (isArithetic(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);

		if (aOperand->isArray() && bOperand->isArray())
			bufferPtr_=calcAritheticSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), length_);
		else if (aOperand->isArray())
			bufferPtr_=calcAritheticSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), length_);
		else
			bufferPtr_=calcAritheticSmallArray(op_code_, type_, bufferPtr_, aOperand->getBinaryValue(), bOperand->getBufferPtr(), length_);
	}
	if (isComparsion(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		auto localType=aOperand->getType();
		if (aOperand->isArray() && bOperand->isArray())
			bufferPtr_=calcComparsionSmallArray(op_code_, localType, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), length_);
		else if (aOperand->isArray())
			bufferPtr_=calcComparsionSmallArray(op_code_, localType, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), length_);
		else
			bufferPtr_=calcComparsionSmallArray(op_code_, localType, bufferPtr_, aOperand->getBinaryValue(), bOperand->getBufferPtr(), length_);
	}
	else if (isInv(op_code_))
		bufferPtr_=invAritheticSmallArray(type_, bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isTypeConv(op_code_))
		bufferPtr_=typeConvSmallArray(type_, OP(0)->getType(), bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isBuiltInFunc(op_code_))
		bufferPtr_=builtInFuncSmallArray(op_code_, type_, bufferPtr_, OP(0)->getBufferPtr(), length_);
	else if (isSelect(op_code_)){
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		auto cOperand = OP(2);
		if (bOperand->isArray() && cOperand->isArray())
			bufferPtr_=calcSelectSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBufferPtr(), length_);
		else if (bOperand->isArray())
			bufferPtr_=calcSelectSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBinaryValue(), length_);
		else if (cOperand->isArray())
			bufferPtr_=calcSelectSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBufferPtr(), length_);
		else 
			bufferPtr_=calcSelectSmallArray(op_code_, type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBinaryValue(), length_);
	}
	else if (isConvolve(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		bufferPtr_=calcConvolveSmallArray(type_, bufferPtr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), aOperand->getLength(), bOperand->getLength());
	}
	else if (isSlice(op_code_))         return;
	else if (isStoreToBuffer(op_code_)) return;
	else if (isSmallArrayDef(op_code_)) {
		if (op_code_ == OpCodeEn::smallArrayDef)
			bufferPtr_=calcSmallArrayDef(type_, operand_);
		else
			smallArrayGen();
	}
#undef OP

}




void Operation::smallArray() {
	size_t argsSize = operand_.size();
	if (argsSize == 1)       smallArray(operand_[0]);
	else if (argsSize == 2)  smallArray(operand_[0], operand_[1]);
	else if (argsSize == 3)  smallArray(operand_[0], operand_[1], operand_[2]);
}

void Operation::smallArrayGen(){

	double delta=0.0;
	if (isInteger(type_)) {
		delta = (stop_ - start_) / length_;
	}

	bufferPtr_=calcSmallArrayAlloc(type_, length_);

#define OP(T) {\
    T* point=(T*)bufferPtr_;\
    for (uint64_t i=0; i < length_; i++)  \
        *point++ =(T)(start_ + delta * i);}

SWITCH_TYPE_OP(type_, print_SA_error("samallarray range ");)
#undef OP
};



void Operation::smallArray(Variable* arg1, Variable* arg2, Variable* arg3) {
	if ((isConst(arg1) && isConst(arg2) && isConst(arg3) && isInteger(arg3))) {
		start_     = arg1->getDoubleValue();
		stop_      = arg2->getDoubleValue();

		type_      = TypeEn::double_jty;
		length_    = arg3->getBinaryValue();
	}
	else {
		print_error("range(start_num,stop_num,length) -signature is not supported yet");
		return;
	}
};

void Operation::smallArray(Variable* arg1, Variable* arg2) {
	if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2)) {
		start_     = arg1->getDoubleValue();
		stop_      = arg2->getDoubleValue();

		type_      = TypeEn::int64_jty;
		length_    = (uint64_t)(stop_ - start_);
	}
	else {
		print_error("range(start_num,stop_num) - arg must be integer consant");
	}
};

void Operation::smallArray(Variable* arg1) {
	if (isConst(arg1) && isInteger(arg1)) {
		start_     = 0;
		stop_      = arg1->getBinaryValue();
		type_      = TypeEn::int64_jty;
		length_    = (uint64_t)stop_;
	}
	else {
		print_error("range(len) - arg must be integer consant");
	}
};