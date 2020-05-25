#ifndef LINE_H
#define LINE_H
#include <iostream>
#include <vector>
#include "variable.h"




class Line : public Variable
{
public:

    Line(std::string name_, Variable* var,int index=-1);
    Line(std::string name_, TypeEn, DataStructTypeEn, uint64_t, int index = -1);
    Line(std::string name_, int index = -1);
    ~Line();


    void assignValue(Variable* var);
    int  getUnicleIndex();

    bool isArg();
    bool haveTargetName(std::string);
    bool isTermialLargeArray() { return isArg(); }

    std::string getName() { return names[0]; }

    //safe functions .external stack is used
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) override;

    virtual void visitEnter(stack<Variable*>* visitorStack )override;
    virtual void visitExit(stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr=NULL)override;
    virtual void visitExit(stack<std::string>* varStack )override;

    //virtual Variable* getAssignedVal() { assignedVal; };
    Variable* assignedVal=NULL;


private:
    std::vector<std::string> names;

    bool is_arg = false;


    int unicle_index=-1;


};

#endif