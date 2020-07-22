#ifndef LINE_H
#define LINE_H
#include <iostream>
#include <vector>
#include "variable.h"


class Line : public Variable
{
public:

    Line(std::string name, Variable* var, int index=-1) {
        names_.push_back(name);

        if (isConst(var)) {
            binaryValue_ = var->getBinaryValue();
            textValue_   = var->getTextValue();
        }

        assignedVal = var;
        level_       = var->getLevel();
        dsType_      = var->getDSType();
        type_        = var->getType();
        length_      = var->getLength();
    }


    Line(std::string name, TypeEn ty, DataStructTypeEn dsty, uint64_t len, int index=-1) {
        names_.push_back(name);

        dsType_ = dsty;
        type_   = ty;
        length_ = len;
        is_arg = true;
    }

    Line(std::string name, int ind = -1) {
        names_.push_back(name);

        type_ = TypeEn::unknown_jty;
        is_arg = true;
    }

    ~Line() {
    }


    void assignValue(Variable* var);
    int  getUnicleIndex();
    //Variable* getAssignedVal() { return assignedVal; }
    virtual Variable* getAssignedVal(bool deep = false)  override;


    bool isArg();
    bool haveTargetName(std::string);
    bool isTermialLargeArray() { return isArg(); }

    std::string getName() { return checkBuffer(names_[0]); }


    //safe functions .external stack is used
    virtual void visitEnter (stack<Variable*>* visitorStack)                                            override;
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack)                                   override;
    //virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack)                                 override;

    virtual void genBodyVisitExit(BodyGenContext*  context)                                             override;
    virtual void printVisitExit  (stack<std::string>* varStack)                                         override;
    virtual void genBlocksVisitExit  (TableGenContext*  context)                                        override;
    virtual void setupIR(IRGenerator & builder)                                                         override;
    virtual void reduceLinksVisitExit() override { is_visited_ = false; }

    virtual string printUint() { return uniqueName_ + (is_arg?" = arg()"  :" = assign(" + assignedVal->getUniqueName()+")"); }
    //virtual Variable* getAssignedVal() { assignedVal; };
    


private:

    Variable* assignedVal=NULL;
    std::vector<std::string> names_;

    bool is_arg = false;


    int unicle_index=-1;


};

#endif