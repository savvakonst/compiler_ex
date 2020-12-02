#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>

#include "common.h"
#include "basic.h"

#include "ParameterIO.h"
#include "table.h"


using std::string;
class IRGenerator;

class Value : public SmallArr
{
public:

    Value():SmallArr() {};
    Value(string text, TypeEn type);
    Value(untyped_t value, TypeEn type);
    Value(Value* arg1, Value* arg2, Value* arg3);
    Value(Value* arg1, Value* arg2);
    Value(Value* arg1);

    virtual void setBuffered();
    
    void setReturned(){ is_returned=true; }
    void setBufferLength(uint64_t central_length);
    void setBufferLength(uint64_t left, uint64_t right);
    void setBufferLength(Value* var);
    void setLevel(int64_t var);


    template< typename T >
    T getConvTypeVal()const{ return *((T*)(&binary_value_)); }
    untyped_t getBinaryValue()const{ return *((untyped_t*)(&binary_value_)); }
    double getDoubleValue()const;
    string getTextValue()const{ return text_value_; }
    string getTxtDSType()const;
    string getUniqueName()const{ return unique_name_; }
    int64_t getUsageCounter()const{ return usage_counter_; }
    int64_t getLength()const{ return length_; }
    int64_t getLevel()const{ return level_; }
    int64_t getDecimation()const{ return decimation_; }
    int64_t getBufferLen()const{ return buffer_length_; }
    uint64_t getLeftBufferLen()const{ return left_buffer_length_; }
    uint64_t getRightBufferLen()const{ return right_buffer_length_; }
    TypeEn getType()const{ return type_; }
    TypeEn getTempType()const{ return isUnknownTy(type_) ? temp_type_ : type_; }
    DataStructTypeEn getDSType()const{ return ds_type_; }
    SyncParameter* getPatameter()const{ return parameter_; }

    llvm::Value* getIRValue(IRGenerator& builder, int64_t parent_level);
    llvm::Value* getIRValueBasePtr(IRGenerator& builder, int64_t parent_level);
    llvm::Value* getIRValuePtr(IRGenerator& builder, int64_t parent_level);

    virtual NodeTypeEn getNodeType() const { return NodeTypeEn::value; }
    virtual Value* getAssignedVal(bool deep = false) { return this; }


    bool isUnused(){ return is_unused_; }
    bool isArray(){ return ds_type_ != DataStructTypeEn::constant_dsty; }
    bool isVisited(){ return is_visited_; }
    bool isBuffered(){ return is_buffered; }
    bool isReturned(){ return is_returned; }

    virtual bool isTermialLargeArray(){ return false; }
    //safe functions .external stack is used
    void         commoMmarkUnusedVisitEnter(stack<Value*>* visitorStack) { usage_counter_++; };

    virtual void visitEnter          (stack<Value*>* visitorStack) { 
        visitorStack->push(this); 
        is_visited_ = true; 
    };

    virtual void markUnusedVisitEnter(stack<Value*>* visitorStack) {
        commoMmarkUnusedVisitEnter(visitorStack); 
        is_unused_ = false; 
    };

    virtual void genBodyVisitExit(BodyGenContext* context) {
        context->push(
            context->getGarbageContainer()->add(
                new Value(text_value_, type_)));
        is_visited_ = false;
    };

    virtual void genBlocksVisitExit(TableGenContext*  context) {
        unique_name_ ="c" + std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited_ = false;
    };

    virtual void printVisitExit(stack<string> *Stack) {
        Stack->push(text_value_); is_visited_ = false;
    };

    virtual void genConstRecursiveVisitExit(ConstRecursiveGenContext* context){
        //context->setUint(this);  //not used. this should be removed.
        is_visited_ = false;
    };

    virtual void calculateConstRecursive(ConstRecursiveGenContext* context){
        
    }



    virtual string printUint() { return unique_name_+"="+text_value_; }
    virtual void   setupIR(IRGenerator & builder);

    virtual void   calculate() override;

    std::string    printSmallArray();



protected:

    string checkBuffer(string arg) const {
        if (is_buffered)
            return "storeToBuffer(" + arg + ")";
        else
            return arg;
    }


    bool is_unused_   = true;
    bool is_visited_  = false;
    bool is_buffered = false;

    bool is_returned    = false;
    bool is_initialized = false;

    DataStructTypeEn    ds_type_ = DataStructTypeEn::constant_dsty;
    TypeEn              type_   = TypeEn::DEFAULT_JTY;
    TypeEn              temp_type_   = TypeEn::DEFAULT_JTY;


    string   text_value_ = "" ;
    string   unique_name_ = "" ;

    int64_t  length_     = 1;
    int64_t  decimation_ = 0;
    int64_t  level_      = 0;

    uint64_t left_buffer_length_  = 0;
    uint64_t right_buffer_length_ = 0;

    int64_t  buffer_length_ = 0;

    untyped_t binary_value_  = 0;
    uint64_t usage_counter_ = 0;

    uint64_t buffer_num_    = 0; //unused

    llvm::Value * IR_value_        = nullptr;
    llvm::Value * IR_loaded_buffer_value_ = nullptr;
    llvm::Value * IR_buffer_ptr_     = nullptr;
    llvm::Value * IR_buffer_base_ptr_ = nullptr;

    SyncParameter * parameter_= nullptr;
    friend class TailCallDirective;
};



inline bool operator==  (Value* var1, DataStructTypeEn var2){ return  var1->getDSType() == var2; }
inline bool operator==  (DataStructTypeEn var1, Value* var2){ return  var1 == var2->getDSType(); }
inline bool operator<   (TypeEn    var1, Value* var2){ return  var1 < var2->getType(); }
inline bool operator<   (Value* var1, TypeEn    var2){ return  var1->getType() < var2; }


inline bool isConst(Value* var1){ return var1 == DataStructTypeEn::constant_dsty; }
inline bool isSmallArr(Value* var1){ return var1 == DataStructTypeEn::smallArr_dsty; }
inline bool isLargeArr(Value* var1){ return var1 == DataStructTypeEn::largeArr_dsty; }
inline bool isSimilar(Value* var1, Value* var2){ return  (var1->getDSType() == var2->getDSType() && (var1->getLength() == var2->getLength())); }
inline bool isÑompatible(Value* var1, Value* var2){ return isConst(var1) || isConst(var2) || isSimilar(var1, var2); }


inline bool isUnknownTy(Value* var){ return isUnknownTy(var->getType()); }
inline bool isFloating(Value* var){ return isFloating(var->getType()); }
inline bool isInteger(Value* var){ return isInteger(var->getType()); }
inline bool isUInteger(Value* var){ return isUInteger(var->getType()); }
inline bool isBool(Value* var){ return isBool(var->getType()); }



inline Value* maxTypeVar(Value* var1, Value* var2){ return var1->getType  () < var2->getType  () ? var2 : var1; }
inline Value* maxTempTypeVar(Value* var1, Value* var2){ return var1->getTempType  () < var2->getTempType  () ? var2 : var1; }
inline Value* minTypeVar(Value* var1, Value* var2){ return var1->getType  () < var2->getType  () ? var1 : var2; }
inline Value* maxTypeVar(std::vector<Value*> args){
    Value* var=args[0];
    for(auto i : args)
        var=maxTypeVar(i, var);
    return var;
} //unsafe function .zero size array check missing

inline Value* maxDSVar(Value* var1, Value* var2){ return var1->getDSType() < var2->getDSType() ? var2 : var1; }
inline Value* maxLevelVar(Value* var1, Value* var2){ return var1->getLevel() < var2->getLevel() ? var2 : var1; }
inline Value* minLevelVar(Value* var1, Value* var2){ return var1->getLevel() < var2->getLevel() ? var1 : var2; }

#endif