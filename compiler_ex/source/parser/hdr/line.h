#ifndef LINE_H
#define LINE_H
#include <iostream>
#include <vector>
#include "variable.h"


class Line : public Variable
{
public:

    Line(std::string name_, Variable* var, int index=-1) {
        names.push_back(name_);

        if (isConst(var)) {
            binaryValue = var->getBinaryValue();
            textValue   = var->getTextValue();
        }

        assignedVal = var;
        level       = var->getLevel();
        dstype      = var->getDSType();
        type        = var->getType();
        length      = var->getLength();
    }


    Line(std::string name_, TypeEn ty, DataStructTypeEn dsty, uint64_t len, int index=-1) {
        names.push_back(name_);

        dstype = dsty;
        type   = ty;
        length = len;
        is_arg = true;
    }

    Line(std::string name_, int ind = -1) {
        names.push_back(name_);

        type = TypeEn::Unknown_jty;
        is_arg = true;
    }

    ~Line() {
    }

    void assignValue(Variable* var);
    int  getUnicleIndex();

    bool isArg();
    bool haveTargetName(std::string);
    bool isTermialLargeArray() { return isArg(); }

    std::string getName() { return checkBuffer(names[0]); }

    //safe functions .external stack is used
    virtual void visitEnter (stack<Variable*>* visitorStack)                                            override;
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack)                                   override;
    //virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack)                                 override;

    virtual void genBodyVisitExit(stack<Variable*>* varStack, std::vector<Line*>* namespace_ptr=NULL)   override;
    virtual void printVisitExit  (stack<std::string>* varStack)                                         override;
    virtual void genBlocksVisitExit  (TableGenContext*  context)                                        override;

    //virtual Variable* getAssignedVal() { assignedVal; };
    Variable* assignedVal=NULL;


private:
    std::vector<std::string> names;

    bool is_arg = false;


    int unicle_index=-1;


};

#endif