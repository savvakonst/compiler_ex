#include "operations.h"
#include "line.h"


void print_error(std::string content);


template <class T> inline const T& max(const T& a, const T& b) {
	return (a < b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

Line::Line(std::string name_, Variable* var, int index)
{

	names.push_back(name_);

	if (isConst(var)){
		binaryValue = var->getBinaryValue();
		textValue   = var->getTextValue();
	}

	leftShift   = var->getLeftShift ();
	rightShift  = var->getRightShift();

	assignedVal = var;
	dstype      = var->dstype;
	type        = var->type;
	length      = var->getLength();
}


Line::Line(std::string name_, TypeEn ty, DataStructTypeEn dsty, uint64_t len, int index)
{
	names.push_back(name_);

	dstype = dsty;
	type   = ty;
	length = len;
	is_arg = true;
}


Line::Line(std::string name_, int index)
{

	names.push_back(name_);
	type = TypeEn::Unknown_jty;
	is_arg = true;
}



Line::~Line()
{

}


bool Line::isArg() {
	return is_arg;
}

void Line::assignValue(Variable* var) {
	assignedVal = var;
	type = var->type;
}

bool Line::haveTargetName(std::string name)
{
	for (auto i : names) {
		if (i == name) return true; 
	}
	return false;
}

int Line::getUnicleIndex()
{
	return unicle_index;
}

//safe functions .external stack is used

void Line::markUnusedVisitEnter(stack<Variable*>* visitorStack)
{
	commonVisitEnter(visitorStack);

	if (!is_arg)
		visitorStack->push(assignedVal);
	is_unused = false;
}


void Line::visitEnter(stack<Variable*>* visitorStack)
{
	visitorStack->push(this); is_visited = true;
}

void Line::visitExit(stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr )
{
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

void Line::visitExit(stack<std::string>* varStack)
{
	is_visited = false;
	varStack->push( names[0] + "." + typeToStr(type));
}

//dangerous functions . recursive call is used
Variable* Line::generate()
{
	return nullptr;
}

std::string Line::Print()
{
	return names[0] + "." + typeToStr(type);
}