#ifndef VARIABLE_H
#define VARIABLE_H
#include <iostream>
#include <string>
#include "types_jty.h"

void print_error(std::string content);

class Variable;
bool isLargeArr (Variable * );
bool isInteger  (Variable * );
bool isConst    (Variable * );

template< typename T > class stack :public  std::vector<T> {
public:
    T pop() {
        if (std::vector<T> ::size() == 0) {
            std::cout << "Error : stack is empty\n";
            return NULL;
        }
        T res = std::vector<T> ::back();
        std::vector<T> ::pop_back();

        return res;
    }
    void push(T var) { std::vector<T> ::push_back(var); }
};

class Line;


class Variable
{

public:
    Variable() {
    };

    Variable(std::string text, TypeEn type_) {
        textValue = text;
        type = type_;

        binaryValue = 0;

        if (type_ == TypeEn::Double_jty) {
            *((double*)(&binaryValue)) = stod(textValue);
        }else if (type_ == TypeEn::Float_jty) {
            *((float* )(&binaryValue)) = stof(textValue);
        }
        else if (type_ == TypeEn::Int64_jty) {
            *((int64_t*)(&binaryValue)) = stol(textValue);
        }
        else if (type_ == TypeEn::Int32_jty) {
            *((int32_t*)(&binaryValue)) = stoi(textValue);
        }
        else if (type_ == TypeEn::Int16_jty) {
            *((int16_t*)(&binaryValue)) = stoi(textValue);
        }
        else if (type_ == TypeEn::Int8_jty) {
            *((int8_t*)(&binaryValue)) = stoi(textValue);
        }
        else if (type_ == TypeEn::Int1_jty) {
            *((bool*)(&binaryValue)) = (bool)stoi(textValue);
        }
        else
            print_error("constant reading error");

    };

    Variable(int64_t value, TypeEn type_) {
        binaryValue = value;
        type = type_;

        if (type_ == TypeEn::Double_jty) {
            textValue =std::to_string(*((double*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Float_jty) {
            textValue = std::to_string(*((float*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Int64_jty) {
            textValue = std::to_string(*((int64_t*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Int32_jty) {
            textValue = std::to_string(*((int32_t*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Int16_jty) {
            textValue = std::to_string(*((int16_t*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Int8_jty) {
            textValue = std::to_string(*((int8_t*)(&binaryValue)));
        }
        else if (type_ == TypeEn::Int1_jty) {
            textValue = std::to_string(*((bool*)(&binaryValue)));
        }
        else
            print_error("constant calc error");

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


    std::string  getTxtDSType() {
        std::string t = "pass";
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

    template< typename T >
    T            getConvTypeVal   () { return *((T*)(&binaryValue)); }
    int64_t      getBinaryValue   () { return *((int64_t*)(&binaryValue)); }
    std::string  getTextValue     () { return textValue; }
    int64_t      getUsageCounter  () { return usageCounter; }
    int64_t      getLength        () { return length; }
    int64_t      getDecimation    () { return decimation; }
    int64_t      getLeftShift     () { return leftShift; }
    int64_t      getRightShift    () { return rightShift; }
    int64_t      getLeftBufferLen () { return leftBufferLength; }
    int64_t      getRightBufferLen() { return rightBufferLength; }
    NodeTypeEn   getNodeType      () { return NodeTypeEn::TerminalLine; }
    TypeEn       getType          () { return type; }
    DataStructTypeEn getDSType    () { return dstype; }



    virtual bool isTermialLargeArray    () { return false; }
    bool         isUnused               () { return is_unused; }
    bool         isArray                () { return dstype != DataStructTypeEn::constant_dsty; }
    bool         isVisited              () { return is_visited; }
    bool         isBuffered             () { return is_buffered; }

    //safe functions .external stack is used
    void         commonVisitEnter(stack<Variable*>* visitorStack) { usageCounter++; };
    virtual void markUnusedVisitEnter(stack<Variable*>* visitorStack) { commonVisitEnter(visitorStack);  is_unused = false;  };
    
    virtual void visitEnter  (stack<Variable *>  *visitorStack) { visitorStack->push(this); is_visited = true; };
    virtual void visitExit   (stack<Variable *>  *Stack, std::vector<Line *> *namespace_ptr = NULL) { Stack->push(new Variable(textValue, type)); is_visited = false; };
    virtual void visitExit   (stack<std::string> *Stack) { Stack->push(textValue); is_visited = false; };

protected:


    bool is_unused   = true;
    bool is_visited  = false;
    bool is_buffered = false;

    DataStructTypeEn    dstype = DataStructTypeEn::constant_dsty;
    TypeEn              type   = TypeEn::DEFAULT_JTY;


    std::string textValue;

    uint64_t    length = 1;
    //int64_t     shift = 0;
    int64_t     decimation=0;

    uint64_t    leftBufferLength  =0;
    uint64_t    rightBufferLength =0;

    uint64_t    leftShift    = 0;
    uint64_t    rightShift   = 0;

    uint64_t    binaryValue  = 0;
    uint64_t    usageCounter = 0;

};



inline bool operator==  (Variable*        var1, DataStructTypeEn var2) { return  var1->getDSType() == var2; }
inline bool operator==  (DataStructTypeEn var1, Variable*        var2) { return  var1 == var2->getDSType(); }
inline bool operator<   (TypeEn    var1, Variable* var2) { return  var1 < var2->getType(); }
inline bool operator<   (Variable* var1, TypeEn    var2) { return  var1->getType() < var2; }


inline bool isConst     (Variable* var1) { return var1 == DataStructTypeEn::constant_dsty; }
inline bool isSmallArr  (Variable* var1) { return var1 == DataStructTypeEn::smallArr_dsty; }
inline bool isLargeArr  (Variable* var1) { return var1 == DataStructTypeEn::largeArr_dsty; }
inline bool isSimilar   (Variable* var1,Variable* var2) {return  (var1->getDSType() == var2->getDSType() && (var1->getLength() == var2->getLength())); }
inline bool isÑompatible(Variable* var1, Variable* var2) { return isConst(var1) || isConst(var2) || isSimilar(var1, var2); }


inline bool isUnknownTy (TypeEn type) { return type == TypeEn::Unknown_jty; }
inline bool isFloating  (TypeEn type) { return type >= TypeEn::Float_jty; }
inline bool isInteger   (TypeEn type) { return type <= TypeEn::Int64_jty; }
inline bool isUInteger  (TypeEn type) { return false; }

inline bool isUnknownTy (Variable* var) { return var->getType() == TypeEn::Unknown_jty; }
inline bool isFloating  (Variable* var) { return var->getType() >= TypeEn::Float_jty; }
inline bool isInteger   (Variable* var) { return var->getType() <= TypeEn::Int64_jty; }
inline bool isUInteger  (Variable* var) { return false; }

inline Variable*    max     (Variable* var1, Variable* var2) { return var1->getType() < var2->getType() ? var2 : var1; }
inline Variable*    maxDS   (Variable* var1, Variable* var2) { return var1->getDSType() < var2->getDSType() ? var2 : var1; }
inline int64_t      maxInt  (int64_t   var1, int64_t   var2) { return (var1 < var2) ? var2 : var1; }




inline std::string getTxtType(TypeEn type) {
    std::string t = "pass";
#define ENUM2STR(x) case (TypeEn::x):t=#x;   break
    switch (type)
    {
        ENUM2STR(Int1_jty);
        ENUM2STR(Int8_jty);
        ENUM2STR(Int16_jty);
        ENUM2STR(Int32_jty);
        ENUM2STR(Float_jty);
        ENUM2STR(Double_jty);
        ENUM2STR(Unknown_jty);
    }
    return  t;
#undef ENUM2STR
}





#endif