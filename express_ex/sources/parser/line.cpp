#include "operations.h"
#include "line.h"


//void print_error(std::string content);


template <class T> inline const T& max(const T& a, const T& b) {
	return (a < b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

bool Line::isArg() {
	return is_arg;
}

void Line::assignValue(Variable* var) {
	assigned_val_ = var;
	type_ = var->getType();
}

bool Line::haveTargetName(std::string name){
	if (name_ == name) 
		return true;
	/*
	for (auto i : names_) {
		if (i == name) return true; 
	}*/
	return false;
	
}

Variable * Line::getAssignedVal(bool deep)
{
	if (is_arg)
		return this;
	else if (deep){
		return assigned_val_->getAssignedVal(true);
	}
	else {
		return assigned_val_;
	}
}

//safe functions .external stack is used

void Line::markUnusedVisitEnter(stack<Variable*>* visitorStack){
	commoMmarkUnusedVisitEnter(visitorStack);
	if (!is_arg) {
		visitorStack->push(assigned_val_);
		assigned_val_->setBufferLength(this);
	}
	is_nused_ = false;
}

void Line::genBlocksVisitExit(TableGenContext * context)
{
	uniqueName_ = (isLargeArr(this) ? "vb" : "vs") + std::to_string(context->getUniqueIndex())+"."+ name_;
	//context_->setUint(this);
	is_visited_ = false;
}

void Line::visitEnter(stack<Variable*>* visitorStack){
	visitorStack->push(this);
	is_visited_ = true;
}

void Line::genBodyVisitExit(BodyGenContext * context) {
	is_visited_ = false;
	std::vector<Line*> namespace_ = context->getNamespace();

	std::string name = getName(true);
	if (namespace_.size() < 1)
		return;
	for (int i = namespace_.size() - 1; i >= 0; i--) {
		if (namespace_[i]->haveTargetName(name)) {
			context->push(namespace_[i]);  return;
		}
	}
	print_error("visitExit can't find var name");

}




void Line::printVisitExit(stack<std::string>* varStack){
	is_visited_ = false;
	varStack->push( name_ + "." + toString(type_));
}

