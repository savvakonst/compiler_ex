#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <vector>
#include "variable.h"


class Line : public Variable
{
public:

    Line(std::string name, Variable* var) :Variable() {
        names_.push_back(name);
        name_=name;
        if (isConst(var)) {
            binaryValue_ = var->getBinaryValue();
            textValue_   = var->getTextValue();
        }
        assignedVal_ = var;
        level_       = var->getLevel();
        type_        = var->getType();
        dsType_      = var->getDSType();
        length_      = var->getLength();
    }


    Line(std::string name, TypeEn ty, DataStructTypeEn dsty, uint64_t len) :Variable() {
        names_.push_back(name);
        name_   = name;
        dsType_ = dsty;
        type_   = ty;
        length_ = len;
        is_arg = true;
    }


    Line(std::string name, std::string linkName, DataStructTypeEn dsty) :Variable() {
        //llvm::outs() <<"isLargeArr(dsty):"<< (DataStructTypeEn::largeArr_dsty==dsty)<<"\n";
        names_.push_back(name);
        name_=name;
        linkName_=linkName;
        dsType_ = dsty;
        is_arg = true;
    }
    /*
    Line(std::string name, ParameterInfo parameterInfo) {
        names_.push_back(name);
        name_       = name;
        linkName_   = parameterInfo.parameter_name;
        type_       = parameterInfo.extended_info->jit_type;
        length_     = parameterInfo.extended_info->virtual_size;
        dsType_     = DataStructTypeEn::largeArr_dsty;
        is_arg      = true;
         
        parameter_     = new SyncParameter(parameterInfo);
    } //to remove
    */
    Line(std::string name, SyncParameter * parameter) :Variable() {
        names_.push_back(name);
        name_       = name;
        linkName_   = parameter->getName();
        type_       = RPMType2JITType(parameter->getRPMType());
        length_     = parameter->getVirtualSize();
        dsType_     = DataStructTypeEn::largeArr_dsty;
        is_arg      = true;

        parameter_  = parameter;
    }




    Line(std::string name) :Variable() {
        names_.push_back(name);
        name_=name;
        type_ = TypeEn::unknown_jty;
        is_arg = true;
    }

    ~Line() {
    }


    void assignValue(Variable* var);
    //Variable* getAssignedVal() { return assignedVal; }
    virtual Variable* getAssignedVal(bool deep = false)  override;


    bool isArg();
    bool haveTargetName(std::string);
    bool isTermialLargeArray() { return isArg(); }

    const std::string getName(bool onlyName = false)      { return onlyName ? name_ :checkBuffer(name_); }
    const std::string getLinkName()  { return linkName_; }
    //virtual Variable* getAssignedVal() { assignedVal; };

    //safe functions .external stack is used
    virtual void visitEnter (stack<Variable*>* visitorStack)                                            override;
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack)                                   override;
    //virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack)                                 override;

    virtual void genBodyVisitExit(BodyGenContext* context)                                              override;
    virtual void printVisitExit  (stack<std::string>* varStack)                                         override;
    virtual void genBlocksVisitExit  (TableGenContext* context)                                         override;
    virtual void setupIR(IRGenerator & builder)                                                         override;
    virtual void reduceLinksVisitExit() override { is_visited_ = false; }

    virtual string printUint() { return uniqueName_ + (is_arg?" = arg()"  :" = assign(" + assignedVal_->getUniqueName()+")"); }

    


private:
    bool        is_arg       = false;

    Variable    *assignedVal_ = nullptr;

    std::vector<std::string> names_;

    std::string name_        = std::string();
    std::string linkName_    = std::string();

    
};

#endif