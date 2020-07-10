#include <iostream>
#include <string>
#include <vector>
#include "types_jty.h"

#ifndef COMMON_H
#define COMMON_H



void print_error(std::string content);
void print_IR_error(std::string content);

class Line;
class Variable;

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



 bool operator==  (Variable*        var1, DataStructTypeEn var2); 
 bool operator==  (DataStructTypeEn var1, Variable*        var2);
 bool operator<   (TypeEn    var1, Variable* var2);
 bool operator<   (Variable* var1, TypeEn    var2);

 bool isConst     (Variable* var1);
 bool isSmallArr  (Variable* var1);
 bool isLargeArr  (Variable* var1);
 bool isSimilar   (Variable* var1, Variable* var2);
 bool isÑompatible(Variable* var1, Variable* var2);

 bool isUnknownTy (TypeEn type);
 bool isFloating  (TypeEn type);
 bool isInteger   (TypeEn type);
 bool isUInteger  (TypeEn type);

 bool isUnknownTy (Variable* var);
 bool isFloating  (Variable* var);
 bool isInteger   (Variable* var);
 bool isUInteger  (Variable* var);

 int64_t      maxInt  (int64_t   var1, int64_t   var2);
 Variable*    max     (Variable* var1, Variable* var2);
 Variable*    maxDS   (Variable* var1, Variable* var2);
 Variable*    maxLevel(Variable* var1, Variable* var2);
 Variable*    minLevel(Variable* var1, Variable* var2);


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





#define SWITCH_UINT(CASE_ARG,LOOP, X ) case CASE_ARG: LOOP { X }   
#define AR_SWITCH_OP(OP,LOOP,RET,ARG_A,ARG_B)   \
    switch (OP){\
    SWITCH_UINT(opCodeEn::ADD, LOOP, RET=ARG_A + ARG_B; ) break ;\
    SWITCH_UINT(opCodeEn::SUB, LOOP, RET=ARG_A - ARG_B;) break ;\
    SWITCH_UINT(opCodeEn::MUL, LOOP, RET=ARG_A * ARG_B;) break ;\
    SWITCH_UINT(opCodeEn::SDIV, LOOP, RET=ARG_A / ARG_B;) break ;\
    SWITCH_UINT(opCodeEn::SREM, LOOP, RET=(T)fmod(ARG_A, ARG_B);) break ;\
    SWITCH_UINT(opCodeEn::POW, LOOP,  RET=(T)pow((double)ARG_A, ARG_B);) break ;\
    SWITCH_UINT(opCodeEn::FADD, LOOP, RET=ARG_A + ARG_B;) break; \
    SWITCH_UINT(opCodeEn::FSUB, LOOP, RET=ARG_A - ARG_B;) break; \
    SWITCH_UINT(opCodeEn::FMUL, LOOP, RET=ARG_A * ARG_B;) break; \
    SWITCH_UINT(opCodeEn::FDIV, LOOP, RET=ARG_A / ARG_B;) break; \
    SWITCH_UINT(opCodeEn::FREM, LOOP, RET=(T)fmod(ARG_A, ARG_B);) break; \
    SWITCH_UINT(opCodeEn::FPOW, LOOP, RET=(T)pow((double)ARG_A, ARG_B);) break; \
    default: break;\
    }



#define BI_SWITCH_OP(OP,LOOP,RET,ARG_A)   \
    switch (OP){\
    SWITCH_UINT(opCodeEn::LOG, LOOP, RET=log(ARG_A) ; ) break ;\
    SWITCH_UINT(opCodeEn::LOG2, LOOP, RET=log2(ARG_A) ;) break ;\
    SWITCH_UINT(opCodeEn::LOG10, LOOP, RET=log10(ARG_A) ;) break ;\
    SWITCH_UINT(opCodeEn::COS, LOOP, RET=cos(ARG_A) ;) break ;\
    SWITCH_UINT(opCodeEn::SIN, LOOP, RET=sin(ARG_A);) break ;\
    SWITCH_UINT(opCodeEn::EXP, LOOP,  RET=exp(ARG_A);) break ;\
    default: break;\
    }



 /*
 template< typename T >
 uint64_t calcArithmeticOperation(T arg1, T arg2, opCodeEn uTypeOp)
 {
     T value;
     uint64_t binaryValue = 0;
#define CONV_OP(depend,target) case (depend):  value=(target) ;  break
     switch (uTypeOp)
     {
         CONV_OP(opCodeEn::ADD, arg1 + arg2);
         CONV_OP(opCodeEn::SUB, arg1 - arg2);
         CONV_OP(opCodeEn::MUL, arg1 * arg2);
         CONV_OP(opCodeEn::SDIV, arg1 / arg2);
         CONV_OP(opCodeEn::SREM, (T)fmod(arg1, arg2));
         CONV_OP(opCodeEn::POW, (T)pow((double)arg1, arg2));
     }
     *((T*)(&binaryValue)) = value;
#undef CONV_OP
     return binaryValue;
 }
 */

 template< typename T >
 uint64_t calcArithmeticOperation(T arg1, T arg2, opCodeEn uTypeOp){
     T value;
     uint64_t binaryValue = 0;
     AR_SWITCH_OP(uTypeOp, ;, value, arg1, arg2);
     *((T*)(&binaryValue)) = value;
     return binaryValue;
 }

 template< typename T >
 uint64_t calcBuiltInFuncOperation(T arg, opCodeEn uTypeOp) {
     T value;
     uint64_t binaryValue = 0;
     BI_SWITCH_OP(uTypeOp, ;, value, arg);
     *((T*)(&binaryValue)) = value;
     return binaryValue;
 }

 template <typename T>
 void aritheticTemplate(opCodeEn op, T * ret, T * a, T * b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i], b[i]);
 }

 template <typename T>
 void aritheticConstTemplate(opCodeEn op, T * ret, T * a, T  b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i], b);
 }

 template <typename T>
 void aritheticConstTemplate(opCodeEn op, T * ret, T  a, T * b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a, b[i]);
 }

 template <typename T>
 void invTemplate( T * ret, T * a, int n) {
     for (int i=0; i < n; i++) { ret[i]=-a[i]; }
 }

 template< typename T >
 void builtInFuncTemplate(opCodeEn op, T * ret, T * a,  int n) {
     BI_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i]);
 }


#undef SWITCH_UINT
#undef AR_SWITCH_OP
#undef BI_SWITCH_OP

void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int n);
void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, int64_t  a, void *  b, int n);
void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, int n);

void invAritheticSmallArray(TypeEn targetType, void * ret, void * a, int n);
void typeConvSmallArray(TypeEn retType, TypeEn argType, void * ret, void* arg, int n);


#define DEBUG_STREAM()\
 std::string content(\
     "func calc(i, j):\n"\
     "a = 0 * 1\n"\
     "v=i+j**i\n"\
     "return v\n"\
     ";\n"\
     "\n"\
     "func calc_new(i, j) :\n"\
     "a = 0 * 1\n"\
     "v=i + j**i + 1\n"\
     "return a\n"\
     ";\n"\
     "\n"\
     "x, y, z =param(\"1000\", \"1000\", \"1000\") #asfafasdfs\n"\
     "X=range (0, 12 * 10 + 1)\n"\
     "x=convolve(X, x)\n"\
     "x=calc(x, y)\n"\
     "x=shift(x * 2, 5)\n"\
     "x=convolve(X, x)\n"\
     "\n"\
     "return x\n");\
 \
 \
 ANTLRInputStream input(\
     content)



#endif // COMMON_H