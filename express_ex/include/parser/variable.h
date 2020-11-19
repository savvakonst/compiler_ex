#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <string>

#include "common.h"
#include "basic.h"

#include "ParameterIO.h"
#include "table.h"


using std::string;
class IRGenerator;

class Variable : public SmallArr
{
public:

    Variable():SmallArr() {};
    Variable(string text, TypeEn type);
    Variable(int64_t value, TypeEn type);
    Variable(Variable* arg1, Variable* arg2, Variable* arg3);
    Variable(Variable* arg1, Variable* arg2);
    Variable(Variable* arg1);

    virtual void setBuffered();
    
    void setReturned() { is_returned=true; }
    void setBufferLength(uint64_t central_length);
    void setBufferLength(uint64_t left, uint64_t right);
    void setBufferLength(Variable * var);
    void setLevel(int64_t var);




    template< typename T >
    T            getConvTypeVal   () { return *((T*)(&binaryValue_)); }
    int64_t      getBinaryValue   () { return *((int64_t*)(&binaryValue_)); }
    double       getDoubleValue   ();
    string       getTextValue     () { return textValue_; }
    string       getTxtDSType     ();
    string       getUniqueName    () { return uniqueName_; }
    int64_t      getUsageCounter  () { return usageCounter_; }
    int64_t      getLength        () { return length_; }
    int64_t      getLevel         () { return level_; }
    int64_t      getDecimation    () { return decimation_; }
    int64_t      getBufferLen     () { return bufferLength_; }
    int64_t      getLeftBufferLen () { return leftBufferLength_; }
    int64_t      getRightBufferLen() { return rightBufferLength_; }
    NodeTypeEn   getNodeType      () { return NodeTypeEn::terminalLine; }
    TypeEn       getType          () { return type_; }
    DataStructTypeEn getDSType    () { return dsType_; }
    SyncParameter *  getPatameter () { return parameter_; }


    virtual Variable * getAssignedVal(bool deep = false) { return this; }

    llvm::Value * getIRValue       (IRGenerator & builder, int64_t parentLevel);
    llvm::Value * getIRValueBasePtr    (IRGenerator & builder, int64_t parentLevel);
    llvm::Value * getIRValuePtr(IRGenerator & builder, int64_t parentLevel);


    virtual bool isTermialLargeArray    () { return false; }
    bool         isUnused               () { return is_nused_; }
    bool         isArray                () { return dsType_ != DataStructTypeEn::constant_dsty; }
    bool         isVisited              () { return is_visited_; }
    bool         isBuffered             () { return is_buffered; }
    bool         isReturned             () { return is_returned; }

    //safe functions .external stack is used
    void         commoMmarkUnusedVisitEnter(stack<Variable*>* visitorStack) { usageCounter_++; };

    virtual void visitEnter          (stack<Variable*>* visitorStack) { 
        visitorStack->push(this); 
        is_visited_ = true; 
    };

    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) {
        commoMmarkUnusedVisitEnter(visitorStack); 
        is_nused_ = false; 
    };

    virtual void genBodyVisitExit(BodyGenContext* context) {
        context->push(
            context->getGarbageContainer()->add(
                new Variable(textValue_, type_)));
        is_visited_ = false;
    };

    virtual void genBlocksVisitExit(TableGenContext*  context) {
        uniqueName_ ="c" + std::to_string(context->getUniqueIndex());
        context->setUint(this);
        is_visited_ = false;
    };

    virtual void reduceLinksVisitExit(){
        is_visited_ = false;
    };

    virtual void printVisitExit(stack<string> *Stack) {
        Stack->push(textValue_); is_visited_ = false;
    };

    virtual string printUint() { return uniqueName_+"="+textValue_; }
    virtual void   setupIR(IRGenerator & builder);

    virtual void   calculate() override;

    std::string    printSmallArray();



protected:

    string checkBuffer(string arg) {
        if (is_buffered)
            return "storeToBuffer(" + arg + ")";
        else
            return arg;
    }


    bool is_nused_   = true;
    bool is_visited_  = false;
    bool is_buffered = false;

    bool is_returned    = false;
    bool is_initialized = false;

    DataStructTypeEn    dsType_ = DataStructTypeEn::constant_dsty;
    TypeEn              type_   = TypeEn::DEFAULT_JTY;



    string   textValue_ = "" ;
    string   uniqueName_ = "" ;

    int64_t  length_     = 1;
    int64_t  decimation_ = 0;
    int64_t  level_      = 0;

    int64_t  leftBufferLength_  = 0;
    int64_t  rightBufferLength_ = 0;

    int64_t  bufferLength_ = 0;

    uint64_t binaryValue_  = 0;
    uint64_t usageCounter_ = 0;

    uint64_t bufferNum_    = 0; //unused

    llvm::Value * IRValue_        = nullptr;
    llvm::Value * IRLoadedBufferValue_ = nullptr;
    llvm::Value * IRBufferPtr_     = nullptr;
    llvm::Value * IRBufferBasePtr_ = nullptr;

    SyncParameter * parameter_= nullptr;
    
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


inline bool isUnknownTy (Variable* var) { return var->getType() == TypeEn::unknown_jty; }
inline bool isFloating  (Variable* var) { return var->getType() >= TypeEn::float_jty; }
inline bool isInteger   (Variable* var) { return var->getType() <= TypeEn::int64_jty; }
inline bool isUInteger  (Variable* var) { return false; }


inline int64_t      maxInt  (int64_t   var1, int64_t   var2) { return (var1 < var2) ? var2 : var1; }
inline int64_t      minInt  (int64_t   var1, int64_t   var2) { return (var1 < var2) ? var2 : var2; }
inline Variable*    maxTypeVar     (Variable* var1, Variable* var2) { return var1->getType  () < var2->getType  () ? var2 : var1; }
inline Variable*    minTypeVar     (Variable* var1, Variable* var2) { return var1->getType  () < var2->getType  () ? var1 : var2; }
inline Variable*    maxTypeVar     (std::vector<Variable*> args) { 
    Variable * var=args[0];
    for (auto i : args) 
        var=maxTypeVar(i, var);
    return var; 
} //unsafe function .zero size array check missing

inline Variable*    maxDSVar   (Variable* var1, Variable* var2) { return var1->getDSType() < var2->getDSType() ? var2 : var1; }
inline Variable*    maxLevelVar(Variable* var1, Variable* var2) { return var1->getLevel () < var2->getLevel () ? var2 : var1; }
inline Variable*    minLevelVar(Variable* var1, Variable* var2) { return var1->getLevel () < var2->getLevel () ? var1 : var2; }

#endif