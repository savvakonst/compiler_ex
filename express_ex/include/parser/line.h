#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <vector>
#include "variable.h"


class Line : public Value
{
public:

    Line(std::string name, Value* var) :Value() {
        names_.push_back(name);
        name_=name;
        if (isConst(var)) {
            binary_value_ = var->getBinaryValue();
            text_value_   = var->getTextValue();
        }
        assigned_val_ = var;
        level_       = var->getLevel();
        type_        = var->getType();
        dsType_      = var->getDSType();
        length_      = var->getLength();
    }


    Line(std::string name, TypeEn ty, DataStructTypeEn dsty, uint64_t len) :Value() {
        names_.push_back(name);
        name_   = name;
        dsType_ = dsty;
        type_   = ty;
        length_ = len;
        is_arg = true;
    }

    Line(std::string name, std::string linkName, DataStructTypeEn dsty) :Value() {

        names_.push_back(name);
        name_=name;
        link_name_=linkName;
        dsType_ = dsty;
        is_arg = true;
    }

    Line(std::string name, SyncParameter * parameter) :Value() {
        names_.push_back(name);
        name_       = name;
        link_name_   = parameter->getName();
        type_       = PRMType2JITType(parameter->getRPMType());
        length_     = parameter->getVirtualSize();
        dsType_     = DataStructTypeEn::largeArr_dsty;
        is_arg      = true;
        
        parameter_  = new SyncParameter(*parameter);
    }

    Line(std::string name) :Value() {
        names_.push_back(name);
        name_=name;
        type_ = TypeEn::unknown_jty;
        is_arg = true;
    }

    ~Line() {
    }


    void assignValue(Value* var);
    virtual Value* getAssignedVal(bool deep = false) override;

    bool isArg();
    bool haveTargetName(std::string);
    bool isTermialLargeArray() { return isArg(); }

    const std::string getName(bool onlyName = false) const { return onlyName ? name_ :checkBuffer(name_); }
    const std::string getLinkName() const{ return link_name_; }
    virtual NodeTypeEn getNodeType() const override{ return   NodeTypeEn::line; }
    untyped_t* getBinaryValuePtr(){ return &binary_value_; }


    //safe functions .external stack is used
    virtual void visitEnter (stack<Value*>* visitorStack) override;
    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack) override;

    virtual void genBodyVisitExit(BodyGenContext* context) override;
    virtual void printVisitExit  (stack<std::string>* varStack) override;
    virtual void genBlocksVisitExit  (TableGenContext* context) override;
    virtual void setupIR(IRGenerator & builder) override;


    virtual void setTempTypeAndBinaryValue(Value* var){
        temp_type_ = var->getType();
        binary_value_ = var->getBinaryValue();
    }


private:
    uint32_t reference_; //atavism
public:
    virtual void genConstRecursiveVisitExit(ConstRecursiveGenContext* context) override{
        context->setUint(this); 
        is_visited_ = false;
    };

    virtual void calculateConstRecursive(ConstRecursiveGenContext* context) override{
        binary_value_ = assigned_val_->getBinaryValue();
    }


    virtual string printUint() { return uniqueName_ + (is_arg?" = arg()"  :" = assign(" + assigned_val_->getUniqueName()+")"); }

private:
    bool        is_arg       = false;

    Value    *assigned_val_ = nullptr;

    std::vector<std::string> names_;

    std::string name_        = std::string();
    std::string link_name_    = std::string();

    
};

#endif