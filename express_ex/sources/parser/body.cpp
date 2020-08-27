#include <string>
#include "body.h"
#include "types_jty.h"

//void print_error(const std::string &content);

Body::Body( std::string name,  bool isPrototype )
{
	garbage_contaiiner_ = new GarbageContainer;
	name_ = name;
	isPrototype_ = isPrototype;
	lines_.reserve(30);
}

Body::~Body()
{
	delete garbage_contaiiner_;
	/*
	for (auto& value : lines_) {
		delete value;
	}
	*/
}



void Body::addLine(const std::string &name, Variable* var)
{
	
	auto line = new Line(name,var);
	garbage_contaiiner_->add(line);
	lines_.push_back(line);
}

void Body::addArg(const std::string &name)
{
	auto line = new Line(name);
	garbage_contaiiner_->add(line);
	argCount_++;
	lines_.push_back(line);
}

void Body::addParam(Line * line)
{
	garbage_contaiiner_->add(line);
	argCount_++;
	lines_.push_back(line);
}

void Body::addParam(const std::string &name, TypeEn ty, DataStructTypeEn dsty, uint64_t len)
{
	auto line = new Line(name,  ty,  dsty,  len);
	garbage_contaiiner_->add(line);
	argCount_++;
	lines_.push_back(line);
}

void Body::addParam(const std::string &name, const std::string &linkName,DataStructTypeEn dsty)
{
	auto line = new Line(name, linkName, dsty);
	garbage_contaiiner_->add(line);
	argCount_++;
	lines_.push_back(line);
}

void Body::addReturn(const std::string &name, Variable* var)
{
	auto line = new Line(name, var);
	garbage_contaiiner_->add(line);
	returnStack_.push_back(line);
}



//varStack push/pop 
void Body::push(Variable* line)
{
	garbage_contaiiner_->add(line);
	varStack_.push_back(line);
}

Variable* Body::pop()
{
	if (varStack_.size() == 0) 
		print_error("stack is empty");
	return varStack_.pop();
}

stack<Variable*> Body::pop(size_t length)
{
	//if (varStack_.size() < length_) 
	//	print_error("stack is empty");
	return varStack_.pop(length);
}


//create operation
Variable* Body::typeConvOp(TypeEn targetType, Variable* arg1)
{	
	return newTypeConvOp(garbage_contaiiner_, targetType,  arg1);
}

// maybe this code segment is excess/glut 
/*
void Body::typeConvOp(Variable* arg1, Variable* arg2, Variable* ret_arg1, Variable* ret_arg2)
{
	TypeEn targetType= max(arg1, arg2)->type;
	ret_arg1 = typeConvOp(targetType, arg1);
	ret_arg2 = typeConvOp(targetType, arg2);
}
*/

Variable* Body::builtInFuncOp(OpCodeEn uTypeOp, Variable* arg1 ){

	Variable* ret_arg1 = arg1;
	TypeEn targetType = TypeEn::DEFAULT_JTY;

	if (!isPrototype_) {
		if (TypeEn::float_jty > arg1->getType())
			ret_arg1 = typeConvOp(TypeEn::float_jty, arg1);
		targetType = arg1->getType();
	}
	return newBuiltInFuncOperation(garbage_contaiiner_,targetType, ret_arg1, uTypeOp);
}

Variable* Body::arithmeticOp(OpCodeEn uTypeOp,Variable* arg1, Variable* arg2)
{
	Variable * ret_arg1= arg1, * ret_arg2= arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype_) {
		targetType = max(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newArithmeticOperation(garbage_contaiiner_,targetType, ret_arg1, ret_arg2,  uTypeOp );
}

Variable* Body::comparsionOp(OpCodeEn uTypeOp, Variable* arg1, Variable* arg2)
{
	Variable * ret_arg1= arg1, * ret_arg2= arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype_) {
		targetType = max(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newComparsionOperation(garbage_contaiiner_, targetType, ret_arg1, ret_arg2, uTypeOp);
}


Variable* Body::convolveOp(OpCodeEn uTypeOp, Variable* arg1, Variable* arg2,uint32_t shift) //necessary to add type maching
{
	Variable* ret_arg1 = arg1, * ret_arg2 = arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype_) {
		targetType = max(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newConvolveOperation(garbage_contaiiner_,targetType, ret_arg1, ret_arg2, shift, uTypeOp);
}

Variable* Body::selectOp( Variable* arg1, Variable* arg2, Variable* arg3)
{
	Variable* ret_arg2 = arg2, * ret_arg3 = arg3;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype_) {
		targetType = max(arg2, arg3)->getType();
		ret_arg2   = typeConvOp(targetType, arg2);
		ret_arg3   = typeConvOp(targetType, arg3);
	}
	return newSelectOp(garbage_contaiiner_,targetType, arg1, ret_arg2, ret_arg3);
}



//create operation and push to varStack
void Body::addTypeConvOp(TypeEn targetType){

	Variable* arg1 = pop();
	push(typeConvOp(targetType, arg1));
}

void Body::addBuiltInFuncOp(OpCodeEn uTypeOp){
	Variable* arg1 = pop();
	push(builtInFuncOp(uTypeOp, arg1));
}

void Body::addInvOp() {
	Variable* arg = pop();
	Variable* zero = garbage_contaiiner_->add(new  Variable("0", TypeEn::int32_jty ));
	push(arithmeticOp( OpCodeEn::sub , zero, arg) );
}

void Body::addArithmeticOp(OpCodeEn uTypeOp){

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(arithmeticOp(uTypeOp,arg1, arg2));
}

void Body::addComarsionOp(OpCodeEn uTypeOp) {

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(comparsionOp(uTypeOp, arg1, arg2));
}


void Body::addConvolveOp(OpCodeEn uTypeOp,uint32_t shift){

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(convolveOp(uTypeOp, arg1, arg2,shift));
}

void Body::addSelectOp(){

	Variable* arg3 = pop();
	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(selectOp(arg1, arg2, arg3));
}

void Body::addRangeOp(size_t argCount){
	if((argCount < 1)||(argCount>3))
		print_error("invalid signature of range(..) function");

	stack<Variable*> v=pop(argCount);
	push(newSmallArrayDefOp(garbage_contaiiner_,v,OpCodeEn::smallArrayRange));

}

void Body::addShiftOp()
{
	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(newSliceOp(garbage_contaiiner_,arg1, arg2, OpCodeEn::shift));
}

void Body::addDecimationOp(){

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(newSliceOp(garbage_contaiiner_,arg1, arg2,OpCodeEn::decimation));
}

void Body::addSmallArrayDefinitionOp(size_t size) {
	stack<Variable* > op;

	for (size_t i = 0; i < size; i++)
		op.push(pop());
	std::reverse(op.begin(), op.end());
	push(newSmallArrayDefOp(garbage_contaiiner_,op,OpCodeEn::smallArrayDef, isPrototype_));
}

void Body::addCall(Body* body){
	stack<Variable*> a;
	a.resize(body->getArgCount());
	for (int i = body->getArgCount()-1; i >=0  ; i--) {
		a[i] = pop();
	}
	

	auto b =isPrototype_? body : body->genBodyByPrototype(a,isPrototype_);
	push(garbage_contaiiner_->add(new Call(b,a)));
}



Line* Body::getLastLineFromName(std::string name)
{
	if (lines_.size() < 1)
		return nullptr;
	for (int i = lines_.size() - 1; i >= 0; i--) {
		if (lines_[i]->haveTargetName(name))
			return (lines_[i]);
	}
	print_error("unknown symbol " + name);
	return nullptr;
}

const stack<ParameterIfs*> Body::getOutputParameterList()
{
	stack<ParameterIfs*> pList;
	for (auto i : returnStack_) {
		pList.push(i->getAssignedVal(true)->getPatameter());
	}
	return pList;
}



// tree walker methods
std::string   Body::print(std::string tab, bool DSTEna, bool hideUnusedLines){

	//hideUnusedLines =true;
	stack<Variable*> visitorStack;
	stack<std::string> stringStack;

	const size_t max_line_length=90;
	
	std::string result = " " + getName() + "\n";
	std::string txtLine, txtSkip, txtShifts, txtUsaaage;

	for (auto& value : lines_) {
		if (value->isArg()) {
			//std::string postfix = (!hideUnusedLines || !value->isUnused()) ? "" : " \t\t#unused";
			if (!hideUnusedLines || !value->isUnused()) {
				txtLine     = value->getName() + "=arg()" ;
				txtShifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
				txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? 0 : txtLine.length()), ' ');
				result     += txtLine + txtSkip + txtShifts + "\n";
			}
		}
		else {
			visitorStack.push(value->getAssignedVal());
			do {
				auto var = visitorStack.pop();
				if (var->isVisited())
					var->printVisitExit(&stringStack);
				else
					var->visitEnter(&visitorStack);
			} while (!visitorStack.empty());

			auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";
			//std::cout << tab << value->getName()+"."+ value->getTxtDSType()<< "=" << stringStack.pop()  << "\n";
			if (!hideUnusedLines || !value->isUnused()){
				txtLine     = tab + value->getName() + DST_postfix + "=" + stringStack.pop() ;
				txtShifts   = std::to_string(value->getLeftBufferLen())+" : "+ std::to_string(value->getRightBufferLen()) + " : " + std::to_string(value->getLength());
				txtUsaaage  = std::to_string(value->getUsageCounter());
				txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? max_line_length-2 : txtLine.length()), ' ');
				result     += txtLine + txtSkip + txtShifts + "\n";
			}
		}
	}
	
	for (auto& value : returnStack_) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->printVisitExit(&stringStack);
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());

		auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";


		txtLine     = tab + value->getName() + DST_postfix + "  " + stringStack.pop();
		txtShifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
		txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? 0 : txtLine.length()), ' ');
		result     += txtLine + txtSkip + txtShifts + "\n";

	}

	return   result ;
}



Body* Body::genBodyByPrototype(stack<Variable*> args, bool isPrototype){

	//std::cout << "debug args.size() " << args[0]->Print() << "\n";
	
	if (isPrototype_ == false)
		return this;


	auto arg	= args.begin();

	auto body	= new Body(name_, isPrototype);
	auto context= new BodyGenContext(&varStack_, &(body->lines_), isPrototype, body->getGarbageContainer());

	stack<Variable*> visitorStack;

	for (auto& value : lines_) {
		if (value->isArg()) {
			if (name_ == "main") {
				body->addParam((Line*)*(arg));
				arg++;
			}
			else {
				body->addLine(value->getName(), *(arg));// in line 
				arg++;
			}
		}
		else{
			visitorStack.push(value->getAssignedVal());
			do {
				auto var = visitorStack.pop();
				if (var->isVisited())
					var->genBodyVisitExit(context);
				else
					var->visitEnter(&visitorStack);
			} while (!visitorStack.empty());
			body->addLine(value->getName(), varStack_.pop());
		}
	}
	for (auto& value : returnStack_) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->genBodyVisitExit(context);
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());
		body->addReturn(returnStack_[0]->getName(), varStack_.pop());
	}

	delete context;
	return body;


}



void Body::symplyfy(){

    stack<Variable*> visitorStack;
    for (auto& value : returnStack_) {
        visitorStack.push(value->getAssignedVal());
        do {
            auto var = visitorStack.pop();
            var->markUnusedVisitEnter(&visitorStack);

        } while (!visitorStack.empty());
    }
}

// this function doesn't work correctly
void Body::reduce(){

	stack<Variable*>  visitorStack;

	for (auto& value : lines_) {
		if (value->isArg() && (!value->isUnused())) {
			//code
		}
		else if (!value->isUnused()) {

			visitorStack.push(value->getAssignedVal());
			do {
				auto var = visitorStack.pop();
				if (var->isVisited())
					var->reduceLinksVisitExit();
				else
					var->visitEnter(&visitorStack);
			} while (!visitorStack.empty());
		}
	}

	for (auto& value : returnStack_) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->reduceLinksVisitExit();
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());
		//code
	}
}


void Body::genTable(TableGenContext * context){

	stack<Variable*>  visitorStack;

	if (name_ == "main")
		for (auto& value : returnStack_) 
			value->getAssignedVal(true)->setReturned();

	for (auto& value : lines_) {
		if (value->isArg() && (!value->isUnused())) {
			context->setUint(value);
		}
		else if (!value->isUnused()) {

			visitorStack.push(value->getAssignedVal());
			do {
				auto var = visitorStack.pop();
				if (var->isVisited())
					var->genBlocksVisitExit(context);
				else
					var->visitEnter(&visitorStack);
			} while (!visitorStack.empty());
			//context->setUint(value);
		}
	}
	int64_t maxBufferLength = 0;
	for (auto& value : returnStack_) {

		visitorStack.push(value->getAssignedVal());

		//if (name_ == "main")
		//	value->getAssignedVal(true)->setReturned();

		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->genBlocksVisitExit(context);
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());
		//code

		auto temp= value->getLength();
		if (maxBufferLength< temp)
			maxBufferLength = temp;
	}

	if (name_ == "main")
		context->setMaxBufferLength(maxBufferLength);

}


GarbageContainer::~GarbageContainer() {
	for (auto i : variable_set_)
		delete i;
}