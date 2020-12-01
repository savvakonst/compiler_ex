
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
//void Operation::setBufferLength(Value * var) {
//	if (isLargeArr(this)) {
//		leftBufferLength =var->getLeftShift();
//		rightBufferLength =var->getRightShift();
//	}
//}


void Operation::visitEnterSetupBuffer(stack<Value*>* visitorStack){
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


void Operation::visitEnterStackUpdate( stack<Value*>* visitorStack ){
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


void Operation::markUnusedVisitEnter(stack<Value*>* visitorStack){
	commoMmarkUnusedVisitEnter(visitorStack);
	visitEnterSetupBuffer(visitorStack);
	visitEnterStackUpdate(visitorStack);
	is_unused_ = false;
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

void Operation::genConstRecursiveVisitExit(ConstRecursiveGenContext* context){
	context->setUint(this);
	is_visited_ = false;
}


void Operation::visitEnter(stack<Value*>* visitorStack){
	is_visited_ = true;
	visitorStack->push(this);
	visitEnterStackUpdate(visitorStack);
}


void Operation::genBodyVisitExit(BodyGenContext* context){
	GarbageContainer * garbage_container =context->getGarbageContainer();
	g_pos = pos;

	is_visited_ = false;
	Value* ret = nullptr;
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
		stack<Value*> op;
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


void Operation::calculateConstRecursive(ConstRecursiveGenContext* context){
	context->setUint(this);

#define OP(i)     (operand_[(i)] )

	if(isArithetic(op_code_)){
		temp_type_ = maxTempTypeVar(OP(0), OP(1))->getTempType();
		auto arg_a = calcTypeConvConst(temp_type_, OP(0)->getTempType(), OP(0)->getBinaryValue());
		auto arg_b = calcTypeConvConst(temp_type_, OP(1)->getTempType(), OP(1)->getBinaryValue());
		binary_value_ = calcAritheticConst(op_code_, temp_type_, arg_a, arg_b);
	}
	else if(isComparsion(op_code_)){
		auto local_type_ = maxTempTypeVar(OP(0), OP(1))->getTempType();
		auto arg_a = calcTypeConvConst(local_type_, OP(0)->getTempType(), OP(0)->getBinaryValue());
		auto arg_b = calcTypeConvConst(local_type_, OP(1)->getTempType(), OP(1)->getBinaryValue());
		binary_value_ = calcComparsionConst(op_code_, local_type_, arg_a, arg_b);

		temp_type_ = TypeEn::int1_jty;
	}
	else if(isInv(op_code_)){
		temp_type_ = OP(0)->getTempType();
		binary_value_ = invAritheticConst(OP(0)->getTempType(), OP(0)->getBinaryValue());
	}
	else if(isTypeConv(op_code_)){
		temp_type_ = type_;
		auto arg_b = calcTypeConvConst(temp_type_, OP(1)->getTempType(), OP(1)->getBinaryValue());
	}
	else if(isBuiltInFunc(op_code_)){
		untyped_t temp_var = 0;
		TypeEn temp_type_ = OP(0)->getTempType();
		if(TypeEn::float_jty > temp_type_){
			temp_var = calcTypeConvConst(TypeEn::float_jty, OP(0)->getTempType(), OP(0)->getBinaryValue());
			temp_type_ = TypeEn::float_jty;
		}
		binary_value_ = calcBuiltInFuncConst(op_code_, temp_type_, OP(0)->getBinaryValue());
	}
	else if(isSelect(op_code_)){
		auto arg_a = calcTypeConvConst(TypeEn::int1_jty, OP(0)->getTempType(), OP(0)->getBinaryValue());
		if(contain_rec_call_){
			bool cond = (*((bool*)&arg_a));
			context->exit_from_loop_ = cond != (OP(1)->getNodeType() == NodeTypeEn::tailCall);
			binary_value_ = (OP(1)->getNodeType() != NodeTypeEn::tailCall) ? OP(1)->getBinaryValue() : OP(2)->getBinaryValue();
		}
		else{
			temp_type_ = maxTempTypeVar(OP(1), OP(2))->getTempType();
			auto arg_b = calcTypeConvConst(temp_type_, OP(1)->getTempType(), OP(1)->getBinaryValue());
			auto arg_c = calcTypeConvConst(temp_type_, OP(2)->getTempType(), OP(2)->getBinaryValue());
			binary_value_ = calcSelectConst(op_code_, temp_type_, arg_a, arg_b, arg_c);
		}
	}
	else if(isConvolve(op_code_)){
		print_IR_error("calculateConstRecursive isConvolve invalid command .");
	}
	else if(isSlice(op_code_)){
		print_IR_error("calculateConstRecursive isSlice invalid command .");
	}
	else if(isStoreToBuffer(op_code_)){
		print_IR_error("calculateConstRecursive isStoreToBuffer invalid command .");
	}
	else{
		print_IR_error("calculateConstRecursive unknown command .");
	}
#undef OP
};


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
	int length = (int)length_;
	if (isArithetic(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);

		if (aOperand->isArray() && bOperand->isArray())
			buffer_ptr_=calcAritheticSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), length);
		else if (aOperand->isArray())
			buffer_ptr_=calcAritheticSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), length);
		else
			buffer_ptr_=calcAritheticSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBinaryValue(), bOperand->getBufferPtr(), length);
	}
	if (isComparsion(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		auto local_type=aOperand->getType();
		if (aOperand->isArray() && bOperand->isArray())
			buffer_ptr_=calcComparsionSmallArray(op_code_, local_type, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), length);
		else if (aOperand->isArray())
			buffer_ptr_=calcComparsionSmallArray(op_code_, local_type, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), length);
		else
			buffer_ptr_=calcComparsionSmallArray(op_code_, local_type, buffer_ptr_, aOperand->getBinaryValue(), bOperand->getBufferPtr(), length);
	}
	else if (isInv(op_code_))
		buffer_ptr_=invAritheticSmallArray(type_, buffer_ptr_, OP(0)->getBufferPtr(), length);
	else if (isTypeConv(op_code_))
		buffer_ptr_=calcTypeConvSmallArray(type_, OP(0)->getType(), buffer_ptr_, OP(0)->getBufferPtr(), length);
	else if (isBuiltInFunc(op_code_))
		buffer_ptr_=calcBuiltInFuncSmallArray(op_code_, type_, buffer_ptr_, OP(0)->getBufferPtr(), length);
	else if (isSelect(op_code_)){
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		auto cOperand = OP(2);
		if (bOperand->isArray() && cOperand->isArray())
			buffer_ptr_=calcSelectSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBufferPtr(), length);
		else if (bOperand->isArray())
			buffer_ptr_=calcSelectSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), cOperand->getBinaryValue(), length);
		else if (cOperand->isArray())
			buffer_ptr_=calcSelectSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBufferPtr(), length);
		else 
			buffer_ptr_=calcSelectSmallArray(op_code_, type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBinaryValue(), cOperand->getBinaryValue(), length);
	}
	else if (isConvolve(op_code_)) {
		auto aOperand = OP(0);
		auto bOperand = OP(1);
		buffer_ptr_=calcConvolveSmallArray(type_, buffer_ptr_, aOperand->getBufferPtr(), bOperand->getBufferPtr(), (int)aOperand->getLength(), (int)bOperand->getLength());
	}
	else if (isSlice(op_code_))         return;
	else if (isStoreToBuffer(op_code_)) return;
	else if (isSmallArrayDef(op_code_)) {
		if (op_code_ == OpCodeEn::smallArrayDef)
			buffer_ptr_=calcSmallArrayDef(type_, operand_);
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

	buffer_ptr_=calcSmallArrayAlloc(type_, (int) length_);

#define OP(T) {\
    T* point=(T*)buffer_ptr_;\
    for (uint64_t i=0; i < length_; i++)  \
        *point++ =(T)(start_ + delta * i);}

SWITCH_TYPE_OP(type_, print_SA_error("samallarray range ");)
#undef OP
};



void Operation::smallArray(Value* arg1, Value* arg2, Value* arg3) {
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

void Operation::smallArray(Value* arg1, Value* arg2) {
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

void Operation::smallArray(Value* arg1) {
	if (isConst(arg1) && isInteger(arg1)) {
		start_     = 0;
		stop_      = (double) arg1->getBinaryValue();
		type_      = TypeEn::int64_jty;
		length_    = (uint64_t)stop_;
	}
	else {
		print_error("range(len) - arg must be integer consant");
	}
};