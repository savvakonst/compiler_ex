#include <string>
#include "body.h"
#include "types_jty.h"
#include "call.h"
//void print_error(const std::string &content);

Body::Body( std::string name,  bool isPrototype )
{
	garbage_contaiiner_ = new GarbageContainer;
	name_ = name;
	is_prototype_ = isPrototype;
	lines_.reserve(30);
}

Body::~Body()
{
	delete garbage_contaiiner_; 
	/*
	for (auto& line : lines_) {
		delete line;
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
	arg_count_++;
	lines_.push_back(line);
}

void Body::addParam(Line * line)
{
	garbage_contaiiner_->add(line);
	arg_count_++;
	lines_.push_back(line);
}

void Body::addParam(const std::string &name, TypeEn ty, DataStructTypeEn dsty, uint64_t len)
{
	auto line = new Line(name,  ty,  dsty,  len);
	garbage_contaiiner_->add(line);
	arg_count_++;
	lines_.push_back(line);
}

void Body::addParam(const std::string &name, const std::string &linkName,DataStructTypeEn dsty)
{
	auto line = new Line(name, linkName, dsty);
	garbage_contaiiner_->add(line);
	arg_count_++;
	lines_.push_back(line);
}

void Body::addReturn(const std::string &name, Variable* var)
{
	auto line = new Line(name, var);

	if(is_tail_callable_){
		var = var->getAssignedVal(true);

		bool valid_recursion = var->getNodeType() == NodeTypeEn::tailCallSelect;
		if (!valid_recursion)
			print_error("it isn't tail recursion");
	}

	garbage_contaiiner_->add(line);
	return_stack_.push_back(line);
}

//varStack push/pop 
void Body::push(Variable* line)
{
	garbage_contaiiner_->add(line);
	var_stack_.push_back(line);
}

Variable* Body::pop()
{
	if (var_stack_.size() == 0) 
		print_error("stack is empty");
	return var_stack_.pop();
}

stack<Variable*> Body::pop(size_t length)
{
	//if (var_stack_.size() < length_) 
	//	print_error("stack is empty");
	return var_stack_.pop(length);
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
	TypeEn targetType= maxTypeVar(arg1, arg2)->type;
	ret_arg1 = typeConvOp(targetType, arg1);
	ret_arg2 = typeConvOp(targetType, arg2);
}
*/

Variable* Body::builtInFuncOp(OpCodeEn uTypeOp, Variable* arg1 ){

	Variable* ret_arg1 = arg1;
	TypeEn targetType = TypeEn::DEFAULT_JTY;

	if (!is_prototype_) {
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
	if (!is_prototype_) {
		targetType = maxTypeVar(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newArithmeticOperation(garbage_contaiiner_, targetType, ret_arg1, ret_arg2,  uTypeOp );
}

Variable* Body::comparsionOp(OpCodeEn uTypeOp, Variable* arg1, Variable* arg2)
{
	Variable * ret_arg1= arg1, * ret_arg2= arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!is_prototype_) {
		targetType = maxTypeVar(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newComparsionOperation(garbage_contaiiner_, targetType, ret_arg1, ret_arg2, uTypeOp);
}


Variable* Body::convolveOp(OpCodeEn uTypeOp, Variable* arg1, Variable* arg2,uint32_t shift) //necessary to add type maching
{

	Variable* ret_arg1 = arg1, * ret_arg2 = arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!is_prototype_) {
		targetType = maxTypeVar(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	is_opeator_ = true;
	return newConvolveOperation(garbage_contaiiner_,targetType, ret_arg1, ret_arg2, shift, uTypeOp);
}

Variable* Body::selectOp( Variable* arg1, Variable* arg2, Variable* arg3)
{
	Variable* ret_arg2 = arg2, * ret_arg3 = arg3;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	
	if (!is_prototype_) {
		targetType = maxTypeVar(arg2, arg3)->getType();
		ret_arg2   = typeConvOp(targetType, arg2);
		ret_arg3   = typeConvOp(targetType, arg3);
	}

	bool valid_recursion = false;

	if(is_tail_callable_){
		const NodeTypeEn p =ret_arg3->getAssignedVal(true)->getNodeType();
		valid_recursion =  (p == NodeTypeEn::tailCall);
		valid_recursion = valid_recursion || (ret_arg2->getAssignedVal(true)->getNodeType() == NodeTypeEn::tailCall);
		
	}

	return newSelectOp(garbage_contaiiner_, targetType, arg1, ret_arg2, ret_arg3, valid_recursion);
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
	push(newSmallArrayDefOp(garbage_contaiiner_,op,OpCodeEn::smallArrayDef, is_prototype_));
}

void Body::addCall(Body* body){
	stack<Variable*> a;
	a.resize(body->getArgCount());
	for (int i = body->getArgCount()-1; i >=0  ; i--) {
		a[i] = pop();
	}

	auto b =is_prototype_? body : body->genBodyByPrototype(a,is_prototype_);

	is_opeator_ = is_opeator_ || b->is_opeator_;

	if (body->is_tail_callable_)
		push(garbage_contaiiner_->add(new CallRecursiveFunction(b, a)));
	else 
		push(garbage_contaiiner_->add(new Call(b,a)));
}

void Body::addTailCall() {
	stack<Variable*> a;
	a.resize(this->getArgCount());
	for (int i = this->getArgCount() - 1; i >= 0; i--) {
		a[i] = pop();
	}

	if (is_tail_callable_)
		print_error("second recursive call");

	is_tail_callable_ = true;
	//new TailCallDirective(a);
	push(garbage_contaiiner_->add(new TailCallDirective(a)));
}



Line* Body::getLastLineFromName(std::string name){

	if (lines_.size() < 1)
		return nullptr;
	for (size_t i = lines_.size() - 1; i >= 0; i--) {
		if (lines_[i]->haveTargetName(name))
			return (lines_[i]);
	}
	print_error("unknown symbol " + name);
	return nullptr;
}

const stack<ParameterIfs*> Body::getOutputParameterList()
{
	stack<ParameterIfs*> pList;
	for (auto i : return_stack_) {
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
	std::string txt_line, txt_skip, txt_shifts;

	for (auto& value : lines_) {
		if (value->isArg()) {
			//std::string postfix = (!hideUnusedLines || !line->isUnused()) ? "" : " \t\t#unused";
			if (!hideUnusedLines || !value->isUnused()) {
				txt_line     = value->getName() + "=arg()" ;
				txt_shifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
				txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
				result     += txt_line + txt_skip + txt_shifts + "\n";
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
			//std::cout << tab << line->getName()+"."+ line->getTxtDSType()<< "=" << stringStack.pop()  << "\n";
			if (!hideUnusedLines || !value->isUnused()){
				txt_line     = tab + value->getName() + DST_postfix + "=" + stringStack.pop() ;
				txt_shifts   = std::to_string(value->getLeftBufferLen())+" : "+ std::to_string(value->getRightBufferLen()) + " : " + std::to_string(value->getLength());
				txt_skip     = std::string(max_line_length - ((txt_line.length() > max_line_length) ? max_line_length-2 : txt_line.length()), ' ');
				result     += txt_line + txt_skip + txt_shifts + "\n";
			}
		}
	}
	
	for (auto& value : return_stack_) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->printVisitExit(&stringStack);
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());

		auto DST_postfix = DSTEna ? "." + value->getTxtDSType() : "";


		txt_line = tab + value->getName() + DST_postfix + "  " + stringStack.pop();
		txt_shifts = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
		txt_skip = std::string(max_line_length - ((txt_line.length() > max_line_length) ? 0 : txt_line.length()), ' ');
		result += txt_line + txt_skip + txt_shifts + "\n";

	}




	return   result ;
}



Body* Body::genBodyByPrototype(stack<Variable*> args, bool isPrototype){

	// try replacing the "var_stack_" member with the "var_stack" local variable
	// it might be worth moving this variable to BodyGenContext

	if (is_prototype_ == false)
		return this; //dangerous place


	auto arg = args.begin();

	auto body = new Body(name_, isPrototype);
	body->is_opeator_ = is_opeator_;
	auto context = new BodyGenContext( &(body->lines_), isPrototype, body->getGarbageContainer());

	stack<Variable*> visitor_stack;


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
			visitor_stack.push(value->getAssignedVal());
			do {
				auto var = visitor_stack.pop();
				if (var->isVisited())
					var->genBodyVisitExit(context);
				else
					var->visitEnter(&visitor_stack);
			} while (!visitor_stack.empty());
			body->addLine(value->getName(), context->pop());
		}
	}
	for (auto& value : return_stack_) {
		visitor_stack.push(value->getAssignedVal());
		do {
			auto var = visitor_stack.pop();
			if (var->isVisited())
				var->genBodyVisitExit(context);
			else
				var->visitEnter(&visitor_stack);
		} while (!visitor_stack.empty());
		body->addReturn(return_stack_[0]->getName(), context->pop()); 
	}

	delete context;
	return body;
}




Body* Body::genConstRecusiveByPrototype(stack<Variable*> args){

	stack<Variable*> visitor_stack;
	stack<std::string> stringStack;

	
	auto context =new ConstRecursiveGenContext();

	std::vector<Variable*> instructions_list;

	for(auto& line : lines_){
		if(line->isArg()){

		}
		else{
			visitor_stack.push(line->getAssignedVal());
			do{
				auto var = visitor_stack.pop();
				if(var->isVisited())
					var->genConstRecursiveVisitExit(context);
				else
					var->visitEnter(&visitor_stack);
			} while(!visitor_stack.empty());

			line->setReference(context);
		}
	}

	for(auto& value : return_stack_){
		visitor_stack.push(value->getAssignedVal());
		do{
			auto var = visitor_stack.pop();
			if(var->isVisited())
				var->genConstRecursiveVisitExit(context);
			else
				var->visitEnter(&visitor_stack);
		} while(!visitor_stack.empty());
	}

	delete context;


	int32_t iteration_cnt = 0;

	for(bool loop_ena = true, int index = 0 ; loop_ena; iteration_cnt++ ){
		
		context->instructions_list_[index]->calculate();

		if(iteration_cnt == -1)
			print_error("recursion too deep");
	}

	return nullptr;
}




void Body::symplyfy(){

    stack<Variable*> visitorStack;
    for (auto& value : return_stack_) {
        visitorStack.push(value->getAssignedVal());
        do {
            auto var = visitorStack.pop();
            var->markUnusedVisitEnter(&visitorStack);

        } while (!visitorStack.empty());
    }
}


void Body::genTable(TableGenContext * context){

	stack<Variable*>  visitor_stack;

	if (name_ == "main")
		for (auto& value : return_stack_) 
			value->getAssignedVal(true)->setReturned();

	for (auto& value : lines_) {
		if (value->isArg() && (!value->isUnused())) {
			context->setUint(value);
		}
		else if (!value->isUnused()) {

			visitor_stack.push(value->getAssignedVal());
			do {
				auto var = visitor_stack.pop();
				if (var->isVisited())
					var->genBlocksVisitExit(context);
				else
					var->visitEnter(&visitor_stack);
			} while (!visitor_stack.empty());
			//context_->setUint(line);
		}
	}
	int64_t maxBufferLength = 0;
	for (auto& value : return_stack_) {

		visitor_stack.push(value->getAssignedVal());

		//if (name_ == "main")
		//	line->getAssignedVal(true)->setReturned();

		do {
			auto var = visitor_stack.pop();
			if (var->isVisited())
				var->genBlocksVisitExit(context);
			else
				var->visitEnter(&visitor_stack);
		} while (!visitor_stack.empty());
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