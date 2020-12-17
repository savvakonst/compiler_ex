#ifndef BODY_H
#define BODY_H

#include <iostream>
#include <vector>
#include <map>
#include <list>

#include "common.h"



namespace llvm{
    class Function;
}
class IRGenerator;
class BodyGenContext;
class TableGenContext;
class GarbageContainer;
class Value;
class Line;
class BodyTemplate;
class Body;


class DeclaratedBodysMap :private std::map< std::string, std::list<Body*> >{
public:
    //destructor is necessary
    DeclaratedBodysMap(const std::list<std::string>& names_list){
        for(auto i : names_list)
            operator[](i) = std::list<Body*>();
    }
    Body* getPureFunctionBody(const std::string& name, const Signature& signature) const;
    bool setPureFunctionBody(Body* body);

};



class Body{
public:

    Body(const std::string& name, const std::list<std::string>& names_of_defined_functions, Body * body, bool is_operator);
    ~Body();

    bool isRetStackFull (){ return (name_ != "main") ? 0 < return_stack_.size() : false; }
    bool isRetStackEmpty(){ return 0 == return_stack_.size(); }

    void addLine(const std::string& name, Value* var);
    void addVariableLine(const std::string& name, Value* var);
    void addParam(Line* line);
    void addReturn(const std::string& name, Value* var);

    //var_stack_ push/pop 
    void push(Value*);
    Value* pop();
    stack<Value*> pop(size_t length);

    void setPureFunctionBody(Body* body);

    std::map<std::string /*name*/, std::string /*link name*/> 
                        getParameterLinkNames(bool hide_unused = false) const;
    const stack<Line*>& getRet() const{ return return_stack_; }
    int                 getArgCount() const{ return arg_count_; }
    std::string         getName() const{ return name_; };
    GarbageContainer*   getGarbageContainer() const{ return garbage_contaiiner_; }

    Line*               getLastLineFromName(std::string name) const;
    const stack<ParameterIfs*> getOutputParameterList()const;
    const Signature     getSignature() const;
    Body*               getPureFunctionBody(const std::string& name, const Signature& signature) const;
    //Body*               getOrCreatePureFunctionBody(const BodyTemplate* name, const Signature& signature) const;

    // tree walker methods
    std::string print(std::string tab = "", bool DSTEna = false, bool hideUnusedLines = false);
    void symplyfy();
    void genTable(TableGenContext* tableGenContext);
    llvm::Function* getOrGenIRPureFunction(IRGenerator& builder);
    
    const bool is_operator_ = false;
    const bool is_pure_function_ = false;

protected:

    GarbageContainer* garbage_contaiiner_;

    llvm::Function* function_ = nullptr;
    bool is_tail_callable_ = false;

    const std::string name_ ;

    stack<Value*> var_stack_;
    stack<Line*> lines_;
    stack<Line*> return_stack_;


    Body * parent_body_ = nullptr;
    DeclaratedBodysMap declarated_pure_functions_map_;
    
    int arg_count_ = 0;
    
    friend BodyGenContext;
    friend BodyTemplate;
};



#endif // !BODY_H
