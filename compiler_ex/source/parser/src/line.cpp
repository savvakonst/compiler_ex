#include "operations.h"
#include "line.h"


void print_error(std::string content);


template <class T> inline const T& max(const T& a, const T& b) {
	return (a < b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

bool Line::isArg() {
	return is_arg;
}

void Line::assignValue(Variable* var) {
	assignedVal = var;
	type = var->getType();
}

bool Line::haveTargetName(std::string name){
	for (auto i : names) {
		if (i == name) return true; 
	}
	return false;
}

int Line::getUnicleIndex(){
	return unicle_index;
}

Variable * Line::getAssignedVal(bool deep)
{
	if (is_arg)
		return this;
	else if (deep){
		if (is_buffered)
			getAssignedVal()->setBuffered();
		return assignedVal->getAssignedVal(true);
	}
	else {
		return assignedVal;
	}
}

//safe functions .external stack is used

void Line::markUnusedVisitEnter(stack<Variable*>* visitorStack){
	commoMmarkUnusedVisitEnter(visitorStack);
	if (!is_arg) {
		visitorStack->push(assignedVal);
		assignedVal->setBufferLength(this);
	}
	is_unused = false;
}

void Line::genBlocksVisitExit(TableGenContext * context)
{
	uniqueName = (isLargeArr(this) ? "vb" : "vs") + std::to_string(context->getUniqueIndex())+"."+ names[0];
	//context->setUint(this);
	is_visited = false;
}

/*
void Line::genBlocksVisitEnter(stack<Variable*>* visitorStack){
	visitorStack->push(this);
	if (!is_arg) {
		visitorStack->push(assignedVal);
	}
	is_visited = true;
}
*/

void Line::visitEnter(stack<Variable*>* visitorStack){
	visitorStack->push(this);
	is_visited = true;
}

void Line::genBodyVisitExit(stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr ){
	is_visited = false;
	auto namespace_ = *namespace_ptr;

	std::string name_ = getName();
	if (namespace_.size() < 1)
		return ;
	for (int i = namespace_.size() - 1; i >= 0; i--) {
		if (namespace_[i]->haveTargetName(name_)) { varStack->push(namespace_[i]);  return;
		}
	}
	print_error("visitExit can't find var name");

}

void Line::printVisitExit(stack<std::string>* varStack){
	is_visited = false;
	varStack->push( names[0] + "." + typeToStr(type));
}

