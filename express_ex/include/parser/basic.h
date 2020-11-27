#ifndef BAISIC_H
#define BAISIC_H


#include <iostream>
#include <string>
#include <set>
#include "types_jty.h"


class Variable;
class Body;

class GarbageContainer
{
public:
    ~GarbageContainer();
    /*
    ~GarbageContainer() {
        for (auto i : variable_set_)
            delete i;
    }
    */
    Variable* add(Variable* var) {
        if (var != nullptr)
            variable_set_.insert(var);
        return var;
    }

protected:
    std::set<Variable*> variable_set_;
};

// body generation context from prototype
class BodyGenContext{
public:
    BodyGenContext (std::vector<Line*>* namespace_ptr, bool is_prototype, GarbageContainer* garbage_container){

        namespace_ptr_ = namespace_ptr;
        is_prototype_ = is_prototype;
        garbage_container_ = garbage_container;
    }
    ~BodyGenContext(){}


    inline void push(Variable* var){ var_stack_.push(var); }
    inline Variable* pop(){ return var_stack_.pop(); }
    inline std::vector< Line*>& getNamespace(){ return *namespace_ptr_; }
    inline GarbageContainer* getGarbageContainer(){ return garbage_container_; }
    inline bool isPrototype(){ return is_prototype_; }
private:


    GarbageContainer* garbage_container_ = nullptr;
    stack<Variable*>   var_stack_;
    std::vector<Line*>* namespace_ptr_ = nullptr;
    bool is_prototype_ = false;
};

// constant value recursive clculation context 
class ConstRecursiveGenContext{
public:
    ConstRecursiveGenContext(){}
    ~ConstRecursiveGenContext(){}
    
    inline void setUint(Variable* var){ instructions_list_.push(var); }
    inline uint32_t getReference(){ return reference_cnt_++; }

    stack<Variable*> instructions_list_;

private:
    uint32_t reference_cnt_ = 0;
    
    stack<int64_t>   binary_value_list_;
    stack<int64_t>   binary_value_stack_;
    //std::map<std::string, Variable> namespace_ptr_ ;
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