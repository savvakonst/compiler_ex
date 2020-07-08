#ifndef VARIABLE_H
#define VARIABLE_H
#include <iostream>
#include <string>
#include "types_jty.h"
#include "common.h"
#include "table.h"
#include "IR_generator.h"

using std::string;


class smallArr{
public:
    void genRange() {};
    void loadFromFile(const string &filename) {

    };

    virtual void calculate(){}
protected:
    void * bufferPtr;
};





class Variable : public smallArr
{
public:
    Variable() {
    };

    Variable(string text, TypeEn type_);
    Variable(int64_t value, TypeEn type_);
    Variable(Variable* arg1, Variable* arg2, Variable* arg3);
    Variable(Variable* arg1, Variable* arg2);
    Variable(Variable* arg1);

    virtual void setBuffered();
    void         setReturned() { is_returned=true; }

    void setBufferLength(uint64_t left, uint64_t right);
    void setBufferLength(Variable * var);
    void setLevel(int64_t var);




    string  getTxtDSType();

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

    virtual Variable* getAssignedVal(bool deep = false) { return this; }

    llvm::Value* getIRValue       (IRGenerator & builder, int64_t parentLevel);
    llvm::Value* getIRValuePtr    (IRGenerator & builder, int64_t parentLevel);


    virtual bool isTermialLargeArray    () { return false; }
    bool         isUnused               () { return is_unused; }
    bool         isArray                () { return dstype != DataStructTypeEn::constant_dsty; }
    bool         isVisited              () { return is_visited; }
    bool         isBuffered             () { return is_buffered; }
    bool         isReturned             () { return is_returned; }

    //safe functions .external stack is used
    void         commoMmarkUnusedVisitEnter(stack<Variable*>* visitorStack) { usageCounter++; };

    virtual void visitEnter          (stack<Variable*>* visitorStack) { 
        visitorStack->push(this); 
        is_visited = true; 
    };

    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) {
        commoMmarkUnusedVisitEnter(visitorStack); 
        is_unused = false; 
    };

    virtual void genBodyVisitExit(stack<Variable*>* Stack, std::vector<Line*>* namespace_ptr = NULL) { 
        Stack->push(new Variable(textValue, type)); is_visited = false; 
    };

    virtual void genBlocksVisitExit(TableGenContext*  context) {
        uniqueName ="c" + std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited = false;
    };

    virtual void reduceLinksVisitExit(){
        is_visited = false;
    };

    virtual void printVisitExit(stack<string> *Stack) {
        Stack->push(textValue); is_visited = false;
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

    bool is_returned = false;
    bool is_initialized = false;

    DataStructTypeEn    dstype = DataStructTypeEn::constant_dsty;
    TypeEn              type   = TypeEn::DEFAULT_JTY;

    string   textValue  = "" ;
    string   uniqueName = "" ;

    uint64_t length     = 1;
    int64_t  decimation = 0;
    int64_t  level      = 0;

    uint64_t leftBufferLength  = 0;
    uint64_t rightBufferLength = 0;

    uint64_t binaryValue  = 0;
    uint64_t usageCounter = 0;

    uint64_t bufferNum    = 0;

    llvm::Value * IRValue       = NULL;
    llvm::Value * IRLoadedBufferValue = NULL;
    llvm::Value * IRBufferRefPtr = NULL;


    

};





inline bool operator==  (Variable*        var1, DataStructTypeEn var2) { return  var1->getDSType() == var2; }
inline bool operator==  (DataStructTypeEn var1, Variable*        var2) { return  var1 == var2->getDSType(); }
inline bool operator<   (TypeEn    var1, Variable* var2) { return  var1 < var2->getType(); }
inline bool operator<   (Variable* var1, TypeEn    var2) { return  var1->getType() < var2; }


inline bool isConst     (Variable* var1) { return var1 == DataStructTypeEn::constant_dsty; }
inline bool isSmallArr  (Variable* var1) { return var1 == DataStructTypeEn::smallArr_dsty; }
inline bool isLargeArr  (Variable* var1) { return var1 == DataStructTypeEn::largeArr_dsty; }
inline bool isSimilar   (Variable* var1, Variable* var2) { return  (var1->getDSType() == var2->getDSType() && (var1->getLength() == var2->getLength())); }
inline bool is�ompatible(Variable* var1, Variable* var2) { return isConst(var1) || isConst(var2) || isSimilar(var1, var2); }


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