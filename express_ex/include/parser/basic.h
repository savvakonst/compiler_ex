#ifndef BAISIC_H
#define BAISIC_H


#include <iostream>
#include <string>
#include <set>
#include <list>
#include "types_jty.h"
#include "body.h"

class Value;
class Line;
class Body;


class GarbageContainer
{
public:
    ~GarbageContainer();
    /*
    ~GarbageContainer() {
        for (auto i : value_set_)
            delete i;
    }
    */
    Value* add(Value* var) {
        if (var != nullptr)
            value_set_.insert(var);
        return var;
    }

protected:
    std::set<Value*> value_set_;
};


class CallRecursiveFunctionTemplate;
class CallTemplate;
#include <iterator>

class PrintBodyContext{
public:
    PrintBodyContext(std::string tab, bool DST_ena, bool hide_unused_lines)
        :hide_unused_lines_(hide_unused_lines),
        DST_ena_(DST_ena),
        tab_(tab)
    {
    }

    void createArg(Line* value);
    void createLine(Line* value);
    void createReurn(Line* value);
    void setName(const std::string& name);
    inline void push(const std::string &var){ string_stack_.push(var); }
    inline const std::string pop(){ return string_stack_.pop(); }
    inline void addVoid(const std::string& var){ return void_definition_list_.push(var); }
    inline std::string getResult(){ 
        std::ostringstream imploded;
        std::copy(void_definition_list_.begin(), void_definition_list_.end(), std::ostream_iterator<std::string>(imploded," \n"));
        
        return   tab_ + "\n" + name_  + imploded.str()  + "\n"+result_;
    }
    const std::string tab_;
    const bool DST_ena_;
    const bool hide_unused_lines_;
private:

    const size_t max_line_length_ = 90;

    stack<std::string> void_definition_list_;
    stack<std::string> string_stack_;
    std::string result_ = "";
    std::string name_ = "";

};


// body generation context from Template
class BodyGenContext{
public:

    BodyGenContext(Body* current_body, bool is_pure_function )
        :current_body_(current_body),
        namespace_ptr_(&current_body->lines_),
        is_pure_function_(is_pure_function),
        garbage_container_(current_body->getGarbageContainer()){
    }

    ~BodyGenContext(){}

    inline void push(Value* var){ var_stack_.push(var); }
    inline Value* pop(){ return var_stack_.pop(); }
    inline std::vector< Line*>& getNamespace() const { return *namespace_ptr_; }
    inline GarbageContainer* getGarbageContainer() const { return garbage_container_; }
    inline Body* getPureFunctionBody(const std::string &name, const Signature & signature) const { 
        return current_body_->getPureFunctionBody(name, signature);}
    inline void setPureFunctionBody( Body* body) const{ current_body_->setPureFunctionBody( body);}
    /* inline Body* getOrCreatePureFunctionBody(const BodyTemplate* body, const Signature& signature) const{
        current_body_->getOrCreatePureFunctionBody( body, signature);
    }*/

    inline const Body  * getCurrentBody(){ return current_body_; }
    const bool is_pure_function_;
private:
    GarbageContainer* garbage_container_ = nullptr;
    stack<Value*> var_stack_;
    Body* current_body_ =nullptr;
    std::vector<Line*>* namespace_ptr_ = nullptr;
     
    
    friend CallRecursiveFunctionTemplate;
    friend CallTemplate;
};

// constant value recursive clculation context 
class RecursiveGenContext{
public:
    RecursiveGenContext(bool is_recursive, bool hide_const_values = true)
        :exit_from_loop_(!is_recursive),
        hide_const_values_(hide_const_values)
    {}

    ~RecursiveGenContext(){}
    
    inline void setUint(Value* var){ instructions_list_.push(var); }
    inline uint32_t getReference(){ return reference_cnt_++; }

    void addArg(Value* arg){ 
        if(!hide_const_values_)
            instructions_list_.push(arg);
        args_reference_.push(arg); 
    }
    bool exitFromLoop(){ return exit_from_loop_; }
    stack<Value*> instructions_list_;
    const bool hide_const_values_;
private:
    
    bool exit_from_loop_ ;
    uint32_t reference_cnt_ = 0;
    
    stack<Value* > args_reference_;

    friend class TailCallDirectiveTemplate;
    friend class Operation;
    //std::map<std::string, Value> namespace_ptr_ ;
};


extern PosInText g_pos;

class SmallArr {
public:
    SmallArr() {
        pos=g_pos;
    }

    ~SmallArr() {
        if (buffer_ptr_ != nullptr)
            delete buffer_ptr_;
    }

    void genRange() {};

    void loadFromFile(const std::string &filename) {
    };

    virtual void calculate() {}
    char * getBufferPtr() {
        return buffer_ptr_;
    }

protected:
    PosInText pos;
    double start_ = 0;
    double stop_ = 0;

    char * buffer_ptr_ = nullptr;
};




#endif