#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include "types_jty.h"





void print_error(const std::string &content);
void print_IR_error(const std::string &content);
void print_SA_error(const std::string &content);


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
 int64_t      minInt  (int64_t   var1, int64_t   var2);
 Variable*    max     (Variable* var1, Variable* var2);
 Variable*    maxDS   (Variable* var1, Variable* var2);
 Variable*    maxLevel(Variable* var1, Variable* var2);
 Variable*    minLevel(Variable* var1, Variable* var2);


 inline std::string getTxtType(TypeEn type) {
     std::string t = "pass";
#define ENUM2STR(x) case (TypeEn::x):t=#x;   break
     switch (type)
     {
         ENUM2STR(int1_jty);
         ENUM2STR(int8_jty);
         ENUM2STR(int16_jty);
         ENUM2STR(int32_jty);
         ENUM2STR(float_jty);
         ENUM2STR(double_jty);
         ENUM2STR(unknown_jty);
     }
     return  t;
#undef ENUM2STR
 }


#define CONV_TYPE_OP(depend,target) case (depend):  target ;  break
#define  SWITCH_TYPE_OP(TYPE,DEFAULT) switch(TYPE){\
    CONV_TYPE_OP(TypeEn::double_jty, OP(double));\
    CONV_TYPE_OP(TypeEn::float_jty, OP(float));\
    CONV_TYPE_OP(TypeEn::int64_jty, OP(int64_t));\
    CONV_TYPE_OP(TypeEn::int32_jty, OP(int32_t));\
    CONV_TYPE_OP(TypeEn::int16_jty, OP(int16_t));\
    CONV_TYPE_OP(TypeEn::int8_jty, OP(int8_t));\
    CONV_TYPE_OP(TypeEn::int1_jty, OP(bool) );\
    default: DEFAULT ; break;\
    }




#define SWITCH_UINT(CASE_ARG,LOOP, X ) case CASE_ARG: LOOP { X }   
#define AR_SWITCH_OP(OP,LOOP,RET,ARG_A,ARG_B)   \
    switch (OP){\
    SWITCH_UINT(OpCodeEn::add, LOOP, RET=ARG_A + ARG_B; ) break ;\
    SWITCH_UINT(OpCodeEn::sub, LOOP, RET=ARG_A - ARG_B;) break ;\
    SWITCH_UINT(OpCodeEn::mul, LOOP, RET=ARG_A * ARG_B;) break ;\
    SWITCH_UINT(OpCodeEn::sdiv, LOOP, RET=ARG_A / ARG_B;) break ;\
    SWITCH_UINT(OpCodeEn::srem, LOOP, RET=(T)fmod(ARG_A, ARG_B);) break ;\
    SWITCH_UINT(OpCodeEn::pow, LOOP,  RET=(T)pow((double)ARG_A, ARG_B);) break ;\
    SWITCH_UINT(OpCodeEn::fadd, LOOP, RET=ARG_A + ARG_B;) break; \
    SWITCH_UINT(OpCodeEn::fsub, LOOP, RET=ARG_A - ARG_B;) break; \
    SWITCH_UINT(OpCodeEn::fmul, LOOP, RET=ARG_A * ARG_B;) break; \
    SWITCH_UINT(OpCodeEn::fdiv, LOOP, RET=ARG_A / ARG_B;) break; \
    SWITCH_UINT(OpCodeEn::frem, LOOP, RET=(T)fmod(ARG_A, ARG_B);) break; \
    SWITCH_UINT(OpCodeEn::fpow, LOOP, RET=(T)pow((double)ARG_A, ARG_B);) break; \
    default: break;\
    }



#define BI_SWITCH_OP(OP,LOOP,RET,ARG_A)   \
    switch (OP){\
    SWITCH_UINT(OpCodeEn::log, LOOP, RET=log(ARG_A) ; ) break ;\
    SWITCH_UINT(OpCodeEn::log2, LOOP, RET=log2(ARG_A) ;) break ;\
    SWITCH_UINT(OpCodeEn::log10, LOOP, RET=log10(ARG_A) ;) break ;\
    SWITCH_UINT(OpCodeEn::cos, LOOP, RET=cos(ARG_A) ;) break ;\
    SWITCH_UINT(OpCodeEn::sin, LOOP, RET=sin(ARG_A);) break ;\
    SWITCH_UINT(OpCodeEn::exp, LOOP,  RET=exp(ARG_A);) break ;\
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
 T *  SmallArrayAlloc(int n, T* ptr=NULL) {
     if (ptr != NULL)
         return ptr;
     return new T[n];
 }


 template< typename T >
 uint64_t calcArithmeticOperation(T arg1, T arg2, OpCodeEn uTypeOp){
     T value = (T)0;;
     uint64_t binaryValue = 0;
     AR_SWITCH_OP(uTypeOp, ;, value, arg1, arg2);
     *((T*)(&binaryValue)) = value;
     return binaryValue;
 }

 template< typename T >
 uint64_t calcBuiltInFuncOperation(T arg, OpCodeEn uTypeOp) {
     T value = (T)0;
     uint64_t binaryValue = 0;
     BI_SWITCH_OP(uTypeOp, ;, value, arg);
     *((T*)(&binaryValue)) = value;
     return binaryValue;
 }

 template <typename T>
 void aritheticTemplate(OpCodeEn op, T * ret, T * a, T * b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i], b[i]);
 }

 template <typename T>
 void aritheticConstTemplate(OpCodeEn op, T * ret, T * a, T  b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i], b);
 }

 template <typename T>
 void aritheticConstTemplate(OpCodeEn op, T * ret, T  a, T * b, int n) {
     AR_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a, b[i]);
 }

 template <typename T>
 void invTemplate( T * ret, T * a, int n) {
     for (int i=0; i < n; i++) { ret[i]=-a[i]; }
 }

 template< typename T >
 void builtInFuncTemplate(OpCodeEn op, T * ret, T * a,  int n) {
     BI_SWITCH_OP(op, for (int i=0; i < n; i++), ret[i], a[i]);
 }

 template< typename T >
 void selectTemplate(OpCodeEn op, T * ret, bool * a, T * b, T * c, int n) {
     for (int i=0; i < n; i++) ret[i]=a[i] ? b[i] : c[i];
 }

 template< typename T >
 void selectTemplate(OpCodeEn op, T * ret, bool * a, T * b, T  c, int n) {
     for (int i=0; i < n; i++) ret[i]=a[i] ? b[i] : c;
 }

 template< typename T >
 void selectTemplate(OpCodeEn op, T * ret, bool * a, T  b, T * c, int n) {
     for (int i=0; i < n; i++) ret[i]=a[i] ? b : c[i];
 }

 template< typename T >
 void selectTemplate(OpCodeEn op, T * ret, bool * a, T  b, T  c, int n) {
     for (int i=0; i < n; i++) ret[i]=a[i] ? b : c;
 }
 
 template< typename T >
 void convolveTemplate_(T * ret, T * a, T * b, int na, int nb, int offset=0) {
     int nMax=na, nMin=nb;
     T * mainArr=a, * subArr=b;
     if (nb > na) {
         nMax=nb;
         nMin=na;
         mainArr=b;
         subArr=a;
     }

     
     int left_offset=(nMin / 2) + offset - (nMin - 1);
     for (int i=0; i < nMax; i++) {
         ret[i]=(T)0;
         /*

         for (int j=0; j < nMin; j++) {
             int k=i + j - left_offset;
             if ((k >= 0) && (k < nMax))
                 ret[i]+= mainArr[k] * subArr[nMin-1-j];
         }
         */

         for (int j=nMin-1; j > -1; j--) {
             int k=i -j - left_offset ;
             if ((k >= 0) && (k < nMax))
                 ret[i]+= mainArr[k] * subArr[j];
         }
     }

 }


#undef SWITCH_UINT
#undef AR_SWITCH_OP
#undef BI_SWITCH_OP

void * calcSmallArrayAlloc(TypeEn targetType, int N, void* ptr=NULL);

void * calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int n);
void * calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, int64_t  a, void *  b, int n);
void * calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, int n);

void * invAritheticSmallArray(TypeEn targetType, void * ret, void * a, int n);

void * builtInFuncSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int n);

void * calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, void * c, int n);
void * calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int64_t c, int n);
void * calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, void * c, int n);
void * calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t b, int64_t c, int n);

void * calcConvolveSmallArray( TypeEn targetType, void * ret, void * a, void * b, int aN,int bN);

void * typeConvSmallArray(TypeEn retType, TypeEn argType, void * ret, void* arg, int n);

void * calcSmallArrayDef(TypeEn targetType, std::vector<Variable*> &operand);



inline size_t tEnSizeof(TypeEn type) {
    size_t size=0;
#define OP(T)  size=sizeof(T)
    SWITCH_TYPE_OP(type, ;)
#undef OP
    return size;
}



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