#ifndef VARIABLE_H
#define VARIABLE_H
#include <iostream>
#include <string>
#include "types_jty.h"
#include "common.h"
#include "table.h"
#include "IR_generator.h"

using std::string;



class Variable
{
public:
    Variable() {
    };

    Variable(string text, TypeEn type_) {
        textValue = text;
        type = type_;

        binaryValue = 0;

        switch (type_)
        {
            case TypeEn::Int1_jty:   *((bool   *)(&binaryValue)) = (bool)stoi(textValue); break;
            case TypeEn::Int8_jty:   *((int8_t *)(&binaryValue)) = stoi(textValue); break;
            case TypeEn::Int16_jty:  *((int16_t*)(&binaryValue)) = stoi(textValue); break;
            case TypeEn::Int32_jty:  *((int32_t*)(&binaryValue)) = stoi(textValue); break;
            case TypeEn::Int64_jty:  *((int64_t*)(&binaryValue)) = stol(textValue); break;
            case TypeEn::Float_jty:  *((float  *)(&binaryValue)) = stof(textValue); break;
            case TypeEn::Double_jty: *((double *)(&binaryValue)) = stod(textValue); break;
            case TypeEn::Unknown_jty:                                               break;
            default: print_error("constant reading error");                         break;
        }

    };

    Variable(int64_t value, TypeEn type_) {
        binaryValue = value;
        type = type_;

        switch (type_)
        {
            case TypeEn::Int1_jty:   textValue = std::to_string(*((bool   *)(&binaryValue))); break;
            case TypeEn::Int8_jty:   textValue = std::to_string(*((int8_t *)(&binaryValue))); break;
            case TypeEn::Int16_jty:  textValue = std::to_string(*((int16_t*)(&binaryValue))); break;
            case TypeEn::Int32_jty:  textValue = std::to_string(*((int32_t*)(&binaryValue))); break;
            case TypeEn::Int64_jty:  textValue = std::to_string(*((int64_t*)(&binaryValue))); break;
            case TypeEn::Float_jty:  textValue = std::to_string(*((float  *)(&binaryValue))); break;
            case TypeEn::Double_jty: textValue = std::to_string(*((double *)(&binaryValue))); break;
            case TypeEn::Unknown_jty:                                                         break;
            default: print_error("constant calc error");                                      break;
        }
    };

    Variable(Variable* arg1, Variable* arg2, Variable* arg3) {
        if (!(isConst(arg1) && isConst(arg2) && isConst(arg3))) {
            print_error("range args must be a constant"); 
            return;
        }
        else {
            print_error("range(start_num,stop_num,step) -signature is not supported yet");
            return;
        }
    };

    Variable(Variable* arg1, Variable* arg2) {
        if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2) ) {
            length    = arg2->getBinaryValue() - arg1->getBinaryValue();
            dstype    = DataStructTypeEn::smallArr_dsty;
            type      = TypeEn::Double_jty;
            textValue = "range(" + std::to_string(arg1->getBinaryValue()) + "," + std::to_string(arg2->getBinaryValue()) + ")";
        }else {
            print_error("range(start_num,stop_num) - arg must be integer consant");
        }
    };

    Variable(Variable* arg1) {
        if (isConst(arg1)&& isInteger(arg1) ) {
            length    = arg1->getBinaryValue();
            dstype    = DataStructTypeEn::smallArr_dsty;
            type      = TypeEn::Double_jty;
            textValue = "range(" + std::to_string(arg1->getBinaryValue()) + ")";
        }
        else {
            print_error("range(len) - arg must be integer consant");
        }
    };


    void setBuffered(){
        if (isLargeArr(this)) {
            is_buffered=true;
        }
    }

    void setBufferLength(uint64_t left, uint64_t right) {
        if (isLargeArr(this)) {
            leftBufferLength=maxInt(leftBufferLength, left);
            rightBufferLength=maxInt(rightBufferLength, right);
        }
    }

    void setBufferLength(Variable * var) {
        if(isLargeArr(this)){
            leftBufferLength =maxInt(leftBufferLength, var->getLeftBufferLen());
            rightBufferLength =maxInt(rightBufferLength, var->getRightBufferLen());
        }
    }

    string  getTxtDSType() {
        string t = "pass";
#define ENUM2STR(x) case (DataStructTypeEn::x):t=#x;   break
        switch (dstype)
        {
            ENUM2STR(constant_dsty);
            ENUM2STR(smallArr_dsty);
            ENUM2STR(largeArr_dsty);
        }
        return  t;
#undef ENUM2STR
    }



    template< typename T >
    T            getConvTypeVal   () { return *((T*)(&binaryValue)); }
    int64_t      getBinaryValue   () { return *((int64_t*)(&binaryValue)); }
    string       getTextValue     () { return textValue; }
    string       getUniqueName    () { return uniqueName; }
    int64_t      getUsageCounter  () { return usageCounter; }
    int64_t      getLength        () { return length; }
    int64_t      getLevel         () { return level; }
    int64_t      getDecimation    () { return decimation; }
    int64_t      getLeftBufferLen () { return leftBufferLength; }
    int64_t      getRightBufferLen() { return rightBufferLength; }
    NodeTypeEn   getNodeType      () { return NodeTypeEn::TerminalLine; }
    TypeEn       getType          () { return type; }
    DataStructTypeEn getDSType    () { return dstype; }
    llvm::Value* getIRValue       () { return IRValue; }



    virtual bool isTermialLargeArray    () { return false; }
    bool         isUnused               () { return is_unused; }
    bool         isArray                () { return dstype != DataStructTypeEn::constant_dsty; }
    bool         isVisited              () { return is_visited; }
    bool         isBuffered             () { return is_buffered; }


    //safe functions .external stack is used
    void         commoMmarkUnusedVisitEnter(stack<Variable*>* visitorStack) { usageCounter++; };


    virtual void visitEnter          (stack<Variable*>* visitorStack) { visitorStack->push(this); is_visited = true; };
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) { commoMmarkUnusedVisitEnter(visitorStack); is_unused = false; };
    //virtual void genBlocksVisitEnter (stack<Variable*>* visitorStack) { visitorStack->push(this); is_visited = true;  };


    virtual void genBodyVisitExit    (stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr = NULL) { Stack->push(new Variable(textValue, type)); is_visited = false; };
    virtual void printVisitExit      (stack<string> *Stack) { Stack->push(textValue); is_visited = false; };
    virtual void genBlocksVisitExit  (TableGenContext*  context) {
        uniqueName ="c" + std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited = false;
    };

    virtual string printUint() { return uniqueName+"="+textValue; }
    virtual void   setupIR(IRGenerator & builder);

protected:

    string checkBuffer(string arg) {
        if (is_buffered)
            return "storeToBuffer(" + arg + ")";
        else
            return arg;
    }

    bool is_unused   = true;
    bool is_visited  = false;
    bool is_buffered = false;

    DataStructTypeEn    dstype = DataStructTypeEn::constant_dsty;
    TypeEn              type   = TypeEn::DEFAULT_JTY;

    string   textValue="";
    string   uniqueName="";

    uint64_t length     = 1;
    int64_t  decimation = 0;
    int64_t  level      = 0;

    uint64_t leftBufferLength  = 0;
    uint64_t rightBufferLength = 0;

    uint64_t binaryValue  = 0;
    uint64_t usageCounter = 0;

    llvm::Value * IRValue = NULL;
};





inline bool operator==  (Variable*        var1, DataStructTypeEn var2) { return  var1->getDSType() == var2; }
inline bool operator==  (DataStructTypeEn var1, Variable*        var2) { return  var1 == var2->getDSType(); }
inline bool operator<   (TypeEn    var1, Variable* var2) { return  var1 < var2->getType(); }
inline bool operator<   (Variable* var1, TypeEn    var2) { return  var1->getType() < var2; }


inline bool isConst     (Variable* var1) { return var1 == DataStructTypeEn::constant_dsty; }
inline bool isSmallArr  (Variable* var1) { return var1 == DataStructTypeEn::smallArr_dsty; }
inline bool isLargeArr  (Variable* var1) { return var1 == DataStructTypeEn::largeArr_dsty; }
inline bool isSimilar   (Variable* var1, Variable* var2) { return  (var1->getDSType() == var2->getDSType() && (var1->getLength() == var2->getLength())); }
inline bool isÑompatible(Variable* var1, Variable* var2) { return isConst(var1) || isConst(var2) || isSimilar(var1, var2); }


inline bool isUnknownTy (TypeEn type) { return type == TypeEn::Unknown_jty; }
inline bool isFloating  (TypeEn type) { return type >= TypeEn::Float_jty; }
inline bool isInteger   (TypeEn type) { return type <= TypeEn::Int64_jty; }
inline bool isUInteger  (TypeEn type) { return false; }

inline bool isUnknownTy (Variable* var) { return var->getType() == TypeEn::Unknown_jty; }
inline bool isFloating  (Variable* var) { return var->getType() >= TypeEn::Float_jty; }
inline bool isInteger   (Variable* var) { return var->getType() <= TypeEn::Int64_jty; }
inline bool isUInteger  (Variable* var) { return false; }


inline int64_t      maxInt  (int64_t   var1, int64_t   var2) { return (var1 < var2) ? var2 : var1; }
inline Variable*    max     (Variable* var1, Variable* var2) { return var1->getType  () < var2->getType  () ? var2 : var1; }
inline Variable*    maxDS   (Variable* var1, Variable* var2) { return var1->getDSType() < var2->getDSType() ? var2 : var1; }
inline Variable*    maxLevel(Variable* var1, Variable* var2) { return var1->getLevel () < var2->getLevel () ? var2 : var1; }
inline Variable*    minLevel(Variable* var1, Variable* var2) { return var1->getLevel () < var2->getLevel () ? var1 : var2; }





#endif