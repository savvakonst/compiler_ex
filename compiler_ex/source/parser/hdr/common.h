#include <iostream>
#include <string>
#include <vector>
#include "types_jty.h"

#ifndef COMMON_H
#define COMMON_H



void print_error(std::string content);

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