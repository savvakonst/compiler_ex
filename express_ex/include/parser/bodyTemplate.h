#ifndef BODY_TEMPLATE_H
#define BODY_TEMPLATE_H


#include <iostream>
#include <vector>
#include <map>
#include <list>

#include "common.h"

class BodyGenContext;
class GarbageContainer;
class Value;
class Line;
class Body;


class BodyTemplate{
public:

    BodyTemplate(std::string name, BodyTemplate* parent_body_template);
    ~BodyTemplate();

    bool isRetStackFull () const{ return (name_ != "main") ? 0 < return_stack_.size() : false; }
    bool isRetStackEmpty() const{ return 0 == return_stack_.size(); }

    //var_stack_ push/pop 
    void push(Value*);
    Value* pop();
    stack<Value*> pop(size_t length);

    //
    void addLine(const std::string &name, Value* var);
    void addArg(const std::string &name); //is necessary to add returned status_ value with line ,pos end error code and string;
    void addParam(Line * line);
    void addParam(const std::string &name, TypeEn ty, DataStructureTypeEn dsty, uint64_t len);
    void addParam(const std::string &name, const std::string & linkName, DataStructureTypeEn dsty=DataStructureTypeEn::kConstant);
    void addReturn(const std::string &name, Value* var); //is necessary to add returned status_ value with line ,pos end error code and string;
    //create operation and push to varStack
    void addTypeConvOp(TypeEn targetType);
    void addBuiltInFuncOp(OpCodeEn uTypeOp);
    void addInvOp();
    void addArithmeticOp(OpCodeEn uTypeOp);
    void addComarsionOp(OpCodeEn uTypeOp);
    void addConvolveOp(OpCodeEn uTypeOp, uint32_t shift = 0);
    void addSelectOp();

    void addRangeOp(size_t argCount);
    void addShiftOp();
    void addDecimationOp();
    void addSmallArrayDefinitionOp(size_t length);
    //create call
    void addCall(BodyTemplate* body);
    void addTailCall();



    std::map<std::string /*name*/, std::string /*link name*/> getParameterLinkNames(bool hide_unused = false) const;
    const stack<Line*>& getRet()const{ return return_stack_; }
    const int getArgCount() const{ return arg_count_; }
    Line* getLastLineFromName(const std::string& name) const;
    const std::string& getName() const{ return name_; };
    BodyTemplate* getParent() const{ return parent_body_template_; }
    BodyTemplate* getFunctionBody(const std::string& name) const;
    const stack<ParameterIfs*> getOutputParameterList() const;
    GarbageContainer* getGarbageContainer() const{ return garbage_contaiiner_; }

    // tree walker methods

    std::string print(std::string tab = "", bool DSTEna = false, bool hideUnusedLines = false) const;
    const std::list<std::string> getNamesOfDefinedFunctions() const;
    Body* genBodyByTemplate(Body* parent_body, stack<Value*> args, bool pure_function)const;
    untyped_t genConstRecusiveByTemplate(stack<Value*>& args)const;


    std::list<BodyTemplate*> child_body_template_list_;
private:
    const std::string name_ ;

    int arg_count_ = 0;

    bool is_operator_ = false;
    bool is_tail_callable_ = false;

    stack<Value*> var_stack_;
    stack<Line*> lines_;
    stack<Line*> return_stack_;

    BodyTemplate* parent_body_template_ = nullptr;
    GarbageContainer* garbage_contaiiner_;
    

    friend BodyGenContext;
};



#endif // !BODY_TEMPLATE_H
