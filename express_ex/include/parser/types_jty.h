#ifndef TYPES_JTS_H
#define TYPES_JTS_H

#include "defWarningIgnore.h"
//#include "llvm/Support/JSON.h"
#include "undefWarningIgnore.h"


typedef  int64_t  untyped_t;
typedef  int64_t  ex_size_t;

typedef int32_t (*jit_simple)();
typedef int (*jit_f)(char *, char *);
typedef int (*jit_fptr)(char, char *);
typedef int32_t (*Jit_Call_t)(char***);


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


enum class DataStructureTypeEn
{
    kConstant,
    kVariable,
    kSmallArr,
    kLargeArr
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

    eq,//   equal
    ne,//   not equal
    ugt,//  unsigned greater than
    uge,//  unsigned greater or equal
    ult,//  unsigned less than
    ule,//  unsigned less or equal
    sgt,//  signed greater than
    sge,//  signed greater or equal
    slt,//  signed less than
    sle,//  signed less or equal

    oeq , // ordered and equal
    one,  // ordered and not equal
    ogt , // ordered and greater than
    oge , // ordered and greater than or equal
    olt , // ordered and less than
    ole , // ordered and less than or equal
    ord , // ordered (no nans)



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
    common_cast,

    convolve,
    convolve_f,

    decimation,
    upsampling,
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
    comparsion = (int)OpCodeEn::eq,
    bitwise = (int)OpCodeEn::LSHL,
    typeConv = (int)OpCodeEn::trunc,
    convolve_op = (int)OpCodeEn::convolve,
    slice_op = (int)OpCodeEn::decimation,
    storeToBuffer = (int)OpCodeEn::storeToBuffer,
    smallArrayDef = (int)OpCodeEn::smallArrayDef,
    builtInFunc = (int)OpCodeEn::log,
    

    invEnd = arithetic,
    aritheticEnd = comparsion,
    comparsionEnd = bitwise,
    bitwiseEnd = typeConv,
    typeConvEnd = convolve_op,
    convolve_opEnd= (int)OpCodeEn::convolve_f+1,
    smallArrayDefEnd=(int)OpCodeEn::smallArrayRange+1,
    slice_opEnd = (int)OpCodeEn::shift+1,
    builtInFuncEnd = (int)OpCodeEn::exp+1
};

enum class NodeTypeEn {
    kValue,
    kOperation,
    kLine,
    kCall,
    kTailCall,
    kTailCallSelect
};



inline bool isInv(OpCodeEn x) {
    TypeOpCodeEn t = (TypeOpCodeEn)x;
    return  (t < TypeOpCodeEn::invEnd);
}
inline bool isArithetic(OpCodeEn x){
    TypeOpCodeEn t=(TypeOpCodeEn)x;
    return (TypeOpCodeEn::arithetic <= t) && (t < TypeOpCodeEn::aritheticEnd);
}
inline bool isComparsion(OpCodeEn x) {
    TypeOpCodeEn t=(TypeOpCodeEn)x;
    return (TypeOpCodeEn::comparsion <= t) && (t < TypeOpCodeEn::comparsionEnd);
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
inline bool isUpsampling(OpCodeEn x) {
    return x == OpCodeEn::upsampling;
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
        ENUM2STR(int64_jty);
        ENUM2STR(float_jty);
        ENUM2STR(double_jty);
        ENUM2STR(unknown_jty);
    }
    return t;
#undef ENUM2STR
}


inline std::string toString(DataStructureTypeEn type){
    std::string t = "pass";
#define ENUM2STR(x) case (DataStructureTypeEn::x):t=#x;   break
    switch(type){
        ENUM2STR(kConstant);
        ENUM2STR(kVariable);
        ENUM2STR(kSmallArr);
        ENUM2STR(kLargeArr);
    }
    return  t;
#undef ENUM2STR
}



inline bool isUnknownTy(TypeEn type){ return type == TypeEn::unknown_jty; }
inline bool isFloating(TypeEn type){ return (type >= TypeEn::float_jty) && !isUnknownTy(type); }
inline bool isInteger(TypeEn type){ return type <= TypeEn::int64_jty; }
inline bool isUInteger(TypeEn type){ return false; }
inline bool isBool(TypeEn type){ return type == TypeEn::int1_jty; }

inline DataStructureTypeEn maxDS(const DataStructureTypeEn &var_a,const DataStructureTypeEn &var_b){ return var_a< var_b ? var_b : var_a; }

#include <vector>

class Signature:protected std::vector<TypeEn>{
public:
    using std::vector<TypeEn>::push_back;

    friend bool operator== (const Signature& a, const Signature& b);
    const std::vector<TypeEn>& getList() const {
        std::vector<TypeEn>& ret =*((std::vector<TypeEn>*)(this));
        return ret;
    }
};

inline bool operator== (const Signature& a, const Signature& b){
    if(a.size() != b.size())
        return false;

    for(size_t i = 0, size_c = a.size(); i < size_c; i++){
        if(a[i] != b[i])
            return false;
    }
    return true;
}

#endif // !1