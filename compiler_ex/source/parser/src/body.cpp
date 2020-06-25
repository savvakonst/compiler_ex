#include <string>
#include "body.h"
#include "types_jty.h"

void print_error(std::string content);

Body::Body( std::string name_,  bool isPrototype_ )
{
	name = name_;
	isPrototype = isPrototype_;
	lines.reserve(30);
}

Body::~Body()
{
	for (auto& value : lines) {
		delete value;
	}
}

Line* Body::getLastLineFromName(std::string name)
{
	if (lines.size()<1)
		return NULL;
	for (int i = lines.size()-1; i >= 0; i--) {
		if (lines[i]->haveTargetName(name)) 
			return (lines[i]);
	}
	print_error("unknown symbol " + name);

	//auto  L = std::find_if(lines.rbegin(), lines.rend(), [&name](Line x) -> bool {return x.name == name; });
}

void Body::addLine(std::string name, Variable* var)
{
	auto line = new Line(name,var, lines.size());
	lines.push_back(line);
}

void Body::addArg(std::string name)
{

	auto line = new Line(name,lines.size());
	argCount++;
	lines.push_back(line);
}


void Body::addParam(std::string name, TypeEn ty, DataStructTypeEn dsty, uint64_t len)
{
	auto line = new Line(name,  ty,  dsty,  len, lines.size());
	argCount++;
	lines.push_back(line);
}


void Body::addReturn(std::string name, Variable* var, int N)
{
	auto line = new Line(name, var, returnStack.size());
	returnStack.push_back(line);
}



//varStack push/pop 
void Body::push(Variable* line)
{
	varStack.push_back(line);
}

Variable* Body::pop()
{
	if (varStack.size() == 0) {
		print_error("stack is empty");
	}
	Variable* res = varStack.back();
	varStack.pop_back(); return res;
}




//create operation
Variable* Body::typeConvOp(TypeEn targetType, Variable* arg1)
{	
	return newTypeConvOp( targetType,  arg1);
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

Variable* Body::builtInFuncOp(opCodeEn uTypeOp, Variable* arg1 ){

	Variable* ret_arg1 = arg1;
	TypeEn targetType = TypeEn::DEFAULT_JTY;

	if (!isPrototype) {
		if (TypeEn::Float_jty > arg1->getType())
			ret_arg1 = typeConvOp(TypeEn::Float_jty, arg1);
		targetType = arg1->getType();
	}
	return newBuiltInFuncOperation(targetType, ret_arg1, uTypeOp);
}

Variable* Body::arithmeticOp(opCodeEn uTypeOp,Variable* arg1, Variable* arg2)
{
	Variable * ret_arg1= arg1, * ret_arg2= arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype) {
		targetType = max(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newArithmeticOperation(targetType, ret_arg1, ret_arg2,  uTypeOp );
}

Variable* Body::convolveOp(opCodeEn uTypeOp, Variable* arg1, Variable* arg2,uint32_t shift) //necessary to add type maching
{
	Variable* ret_arg1 = arg1, * ret_arg2 = arg2;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype) {
		targetType = max(arg1, arg2)->getType();
		ret_arg1   = typeConvOp(targetType, arg1);
		ret_arg2   = typeConvOp(targetType, arg2);
	}
	return newConvolveOperation(targetType, ret_arg1, ret_arg2, shift, uTypeOp);
}

Variable* Body::selectOp( Variable* arg1, Variable* arg2, Variable* arg3)
{
	Variable* ret_arg2 = arg2, * ret_arg3 = arg3;
	TypeEn targetType = TypeEn::DEFAULT_JTY;
	if (!isPrototype) {
		targetType = max(arg2, arg3)->getType();
		ret_arg2   = typeConvOp(targetType, arg2);
		ret_arg3   = typeConvOp(targetType, arg3);
	}
	return newSelectOp(targetType, arg1, ret_arg2, ret_arg3);
}







//create operation and push to varStack
void Body::addTypeConvOp(TypeEn targetType)
{
	Variable* arg1 = pop();
	push(typeConvOp(targetType, arg1));
}

void Body::addBuiltInFuncOp(opCodeEn uTypeOp)
{
	Variable* arg1 = pop();
	push(builtInFuncOp(uTypeOp, arg1));
}

void Body::addArithmeticOp(opCodeEn uTypeOp)
{

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(arithmeticOp(uTypeOp,arg1, arg2));
}

void Body::addConvolveOp(opCodeEn uTypeOp,uint32_t shift)
{

	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(convolveOp(uTypeOp, arg1, arg2,shift));
}

void Body::addSelectOp()
{
	Variable* arg3 = pop();
	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(selectOp(arg1, arg2, arg3));
}


void Body::addRangeOp(size_t argCount){
	if (argCount == 1) {
		Variable* arg1 = pop();
		push(new Variable(arg1));
	}else if (argCount == 2) {
		Variable* arg2 = pop();
		Variable* arg1 = pop();
		push(new Variable(arg1, arg2));
	}
	else if (argCount == 3) {
		Variable* arg3 = pop();
		Variable* arg2 = pop();
		Variable* arg1 = pop();
		push(new Variable(arg1, arg2, arg3));
	}
	else
		print_error("invalid signature of range(..) function");
}

void Body::addShiftOp()
{
	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(newSliceOp(arg1, arg2, opCodeEn::shift));
}

void Body::addDecimationOp()
{
	Variable* arg2 = pop();
	Variable* arg1 = pop();
	push(newSliceOp(arg1, arg2,opCodeEn::decimation));
}

void Body::addCall(Body* body_)
{
	stack<Variable*> a;
	a.resize(body_->getArgCount());
	for (int i = body_->getArgCount()-1; i >=0  ; i--) {
		a[i] = pop();
	}

	auto b = body_->genBodyByPrototype(a);
	push(new Call(b,a));
}


// tree walker methods
std::string   Body::print(std::string tab, bool DSTEna, bool hideUnusedLines){

	hideUnusedLines =true;
	stack<Variable*> visitorStack;
	stack<std::string> stringStack;

	const size_t max_line_length=90;
	
	std::string result = " " + getName() + "\n";
	std::string txtLine, txtSkip, txtShifts, txtUsaaage;

	for (auto& value : lines) {
		if (value->isArg()) {
			//std::string postfix = (!hideUnusedLines || !value->isUnused()) ? "" : " \t\t#unused";
			if (!hideUnusedLines || !value->isUnused()) {
				txtLine     = value->getName() + "=arg()" ;
				txtShifts   = std::to_string(value->getLeftBufferLen()) + " : " + std::to_string(value->getRightBufferLen());
				txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? 0 : txtLine.length()), '-');
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
				txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? 0 : txtLine.length()), '-');
				result     += txtLine + txtSkip + txtShifts + "\n";
			}
		}
	}
	
	for (auto& value : returnStack) {
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
		txtSkip     = std::string(max_line_length - ((txtLine.length() > max_line_length) ? 0 : txtLine.length()), '-');
		result     += txtLine + txtSkip + txtShifts + "\n";

	}


	return   result ;
}



Body* Body::genBodyByPrototype(stack<Variable*> args)
{
	//std::cout << "debug args.size() " << args[0]->Print() << "\n";
	auto arg= args.begin();

	if (isPrototype == false)
		return this;

	auto body =new Body(name,false);
	stack<Variable*> visitorStack;
	
	for (auto& value : lines) {
		if (value->isArg()) {
			body->addLine(value->getName(), *(arg++));// in line 
		}
		else{
			visitorStack.push(value->getAssignedVal());
			do {
				auto var = visitorStack.pop();
				if (var->isVisited())
					var->genBodyVisitExit(&varStack, &(body->lines));
				else
					var->visitEnter(&visitorStack);
			} while (!visitorStack.empty());
			body->addLine(value->getName(), varStack.pop());
		}
	}


	for (auto& value : returnStack) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->genBodyVisitExit(&varStack, &(body->lines));
			else
				var->visitEnter(&visitorStack);
		} while (!visitorStack.empty());
		body->addReturn(returnStack[0]->getName(), varStack.pop());
	}
	return body;
}



void Body::symplyfy()
{
    stack<Variable*> visitorStack;
    for (auto& value : returnStack) {
        visitorStack.push(value->getAssignedVal());
        do {
            auto var = visitorStack.pop();
            var->markUnusedVisitEnter(&visitorStack);

        } while (!visitorStack.empty());
    }
}


void Body::reduce()
{
	stack<Variable*>  visitorStack;

	for (auto& value : lines) {
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

	for (auto& value : returnStack) {
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


void Body::genTable(TableGenContext * context)
{
	stack<Variable*>  visitorStack;

	for (auto& value : lines) {
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

	for (auto& value : returnStack) {
		visitorStack.push(value->getAssignedVal());
		do {
			auto var = visitorStack.pop();
			if (var->isVisited())
				var->genBlocksVisitExit(context);
			else
				var->visitEnter(&visitorStack);

		} while (!visitorStack.empty());
		//code
	}

}
