#ifndef TYPES_JTS_H
#define TYPES_JTS_H
#include "llvm/Support/JSON.h"

enum class unknownTypeOpEn {
    uNEG,
    uADD,
    uSUB,
    uMUL,
    uDIV,
    uREM,
    uPOW
};

enum class TypeEn {
    int1_jty = 0,
    int8_jty,
    int16_jty,
    int32_jty,
    int64_jty,
    float_jty = 16,
    double_jty,

    unknown_jty=32
};

#define DEFAULT_JTY unknown_jty


enum class DataStructTypeEn
{
    constant_dsty,
    smallArr_dsty,
    largeArr_dsty
};


//enum class arrayTypeEn {
//	simpleArray
//};
 
///////////////////////////////////////////////////

enum class OpCodeEn {
    neg = 0,
    fneg,

    add,
    fadd,
    sub,
    fsub,
    mul,
    fmul,
    udiv,
    sdiv,
    fdiv,
    urem,
    srem,
    frem,
    pow,
    fpow,

    LSHL,
    LSHR,
    ASHR,
    AND,
    OR,
    XOR,

    trunc,
    zext,
    sext,
    fptrunc,//double 16777217.0 to float
    fpext,//float 3.125 to double 
    fptoi,
    fptosi,
    uitofp,
    sitofp,
    Unknown_conv,

    convolve,
    convolve_f,

    decimation,
    shift,

    storeToBuffer,

    smallArrayDef,
    smallArrayRange,

    select,

    call,

    log , 
    log2 ,
    log10 , 
    cos , 
    sin , 
    exp,

    none_op
};


enum class TypeOpCodeEn { 
    inv= (int)OpCodeEn::neg,
    arithetic = (int)OpCodeEn::add,
    bitwise = (int)OpCodeEn::LSHL,
    typeConv = (int)OpCodeEn::trunc,
    convolve_op = (int)OpCodeEn::convolve,
    slice_op = (int)OpCodeEn::decimation,
    storeToBuffer = (int)OpCodeEn::storeToBuffer,
    smallArrayDef = (int)OpCodeEn::smallArrayDef,
    builtInFunc = (int)OpCodeEn::log,
    

    invEnd = arithetic,
    aritheticEnd = bitwise,
    bitwiseEnd = typeConv,
    typeConvEnd = convolve_op,
    convolve_opEnd= (int)OpCodeEn::convolve_f+1,
    smallArrayDefEnd=(int)OpCodeEn::smallArrayRange+1,
    slice_opEnd = (int)OpCodeEn::shift+1,
    builtInFuncEnd = (int)OpCodeEn::exp+1
};

enum class NodeTypeEn {
    variable,
    operation,
    terminalLine,
    line,
    call
};



inline bool isInv(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return  (t < TypeOpCodeEn::invEnd);
}
inline bool isArithetic(OpCodeEn x){
    TypeOpCodeEn t=(TypeOpCodeEn)x;
    return (TypeOpCodeEn::arithetic <= t) && (t < TypeOpCodeEn::aritheticEnd);
}
inline bool isBitwise(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::bitwise <= t) && (t < TypeOpCodeEn::bitwiseEnd);
}
inline bool isTypeConv(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::typeConv <= t) && (t < TypeOpCodeEn::typeConvEnd);
}
inline bool isSelect(OpCodeEn x) {
    return (OpCodeEn::select == x);
}
inline bool isCall(OpCodeEn x) {
    return (OpCodeEn::call == x);
}
inline bool isConvolve(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::convolve_op <= t) && (t < TypeOpCodeEn::convolve_opEnd);
}
inline bool isSlice(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::slice_op <= t) && (t < TypeOpCodeEn::slice_opEnd);
}
inline bool isDecimation(OpCodeEn x) {
    return x == OpCodeEn::decimation;
}
inline bool isShift(OpCodeEn x) {
    return x == OpCodeEn::shift;
}
inline bool isStoreToBuffer(OpCodeEn x) {
    return (OpCodeEn::storeToBuffer == x);
}
inline bool isBuiltInFunc(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::builtInFunc <= t) && (t < TypeOpCodeEn::builtInFuncEnd);
}
inline bool isSmallArrayDef(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return (TypeOpCodeEn::smallArrayDef <= t) && (t < TypeOpCodeEn::smallArrayDefEnd);
}





inline std::string toString(TypeEn type)
{
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
    return t;
#undef ENUM2STR
}


class genContext
{
public:
    genContext();
    ~genContext();

private:

};


#endif // !1