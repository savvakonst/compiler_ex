#ifndef BAISIC_H
#define BAISIC_H


#include <iostream>
#include <string>
#include <set>
#include <list>
#include "types_jty.h"
#include "body.h"

class Value;
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
// body generation context from Template
class BodyGenContext{
public:

    BodyGenContext(Body* current_body, bool is_pure_function )
        :current_body_(current_body),
        namespace_ptr_(&current_body->lines_),
        is_pure_function_(is_pure_function),
        garbage_container_(current_body->getGarbageContainer())

    {}

    ~BodyGenContext(){}

    inline void push(Value* var){ var_stack_.push(var); }
    inline Value* pop(){ return var_stack_.pop(); }
    inline std::vector< Line*>& getNamespace() const { return *namespace_ptr_; }
    inline GarbageContainer* getGarbageContainer() const { return garbage_container_; }
    inline Body* getPureFunctionBody(const std::string &name, const Signature & signature) const {
        return current_body_->getPureFunctionBody(name, signature);
    }
    inline void setPureFunctionBody( Body* body) const{
        current_body_->setPureFunctionBody( body);
    }
    /* inline Body* getOrCreatePureFunctionBody(const BodyTemplate* body, const Signature& signature) const{
        current_body_->getOrCreatePureFunctionBody( body, signature);
    }*/

    inline const Body const * getCurrentBody(){ return current_body_; }

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
class ConstRecursiveGenContext{
public:
    ConstRecursiveGenContext(bool is_recursive = true){
        
        exit_from_loop_ = !is_recursive; 
    }
    ~ConstRecursiveGenContext(){}
    
    inline void setUint(Value* var){ instructions_list_.push(var); }
    inline uint32_t getReference(){ return reference_cnt_++; }
    void addArg(untyped_t* arg){ args_reference_.push(arg); }
    bool exitFromLoop(){ return exit_from_loop_; }
    stack<Value*> instructions_list_;

private:
    bool exit_from_loop_ = false;
    uint32_t reference_cnt_ = 0;
    
    stack<untyped_t*> args_reference_;

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