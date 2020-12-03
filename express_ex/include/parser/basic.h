#ifndef BAISIC_H
#define BAISIC_H


#include <iostream>
#include <string>
#include <set>
#include "types_jty.h"


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

// body generation context from prototype
class BodyGenContext{
public:
    BodyGenContext (std::vector<Line*>* namespace_ptr, bool is_template, GarbageContainer* garbage_container){

        namespace_ptr_ = namespace_ptr;
        is_template_ = is_template;
        garbage_container_ = garbage_container;
    }
    ~BodyGenContext(){}


    inline void push(Value* var){ var_stack_.push(var); }
    inline Value* pop(){ return var_stack_.pop(); }
    inline std::vector< Line*>& getNamespace(){ return *namespace_ptr_; }
    inline GarbageContainer* getGarbageContainer(){ return garbage_container_; }
    inline bool isTemplate(){ return is_template_; }
private:


    GarbageContainer* garbage_container_ = nullptr;
    stack<Value*>   var_stack_;
    std::vector<Line*>* namespace_ptr_ = nullptr;
    bool is_template_ = false;
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

    friend class TailCallDirective;
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