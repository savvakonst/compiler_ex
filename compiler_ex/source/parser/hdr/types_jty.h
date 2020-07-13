#ifndef TYPES_JTS_H
#define TYPES_JTS_H


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
    Int1_jty = 0,
    Int8_jty,
    Int16_jty,
    Int32_jty,
    Int64_jty,
    Float_jty = 16,
    Double_jty,

    Unknown_jty=32
};

#define DEFAULT_JTY Unknown_jty


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

enum class opCodeEn {
    NEG = 0,
    FNEG,

    ADD,
    FADD,
    SUB,
    FSUB,
    MUL,
    FMUL,
    UDIV,
    SDIV,
    FDIV,
    UREM,
    SREM,
    FREM,
    POW,
    FPOW,

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

    select,

    call,

    LOG , 
    LOG2 ,
    LOG10 , 
    COS , 
    SIN , 
    EXP,

    NONE_op
};


enum class typeOpCodeEn { 
    inv= (int)opCodeEn::NEG,
    arithetic = (int)opCodeEn::ADD,
    bitwise = (int)opCodeEn::LSHL,
    typeConv = (int)opCodeEn::trunc,
    convolve_op = (int)opCodeEn::convolve,
    slice_op = (int)opCodeEn::decimation,
    storeToBuffer = (int)opCodeEn::storeToBuffer,
    builtInFunc = (int)opCodeEn::LOG,
    

    invEnd = arithetic,
    aritheticEnd = bitwise,
    bitwiseEnd = typeConv,
    typeConvEnd = convolve_op,
    convolve_opEnd= (int)opCodeEn::convolve_f+1,
    slice_opEnd = (int)opCodeEn::shift+1,
    builtInFuncEnd = (int)opCodeEn::EXP+1
};

enum class NodeTypeEn {
    Variable,
    Operation,
    TerminalLine,
    Line,
    Call
};



inline bool isInv(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return  (t < typeOpCodeEn::invEnd);
}
inline bool isArithetic(opCodeEn x){
    typeOpCodeEn t=(typeOpCodeEn)x;
    return (typeOpCodeEn::arithetic <= t) && (t < typeOpCodeEn::aritheticEnd);
}
inline bool isBitwise(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return (typeOpCodeEn::bitwise <= t) && (t < typeOpCodeEn::bitwiseEnd);
}
inline bool isTypeConv(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return (typeOpCodeEn::typeConv <= t) && (t < typeOpCodeEn::typeConvEnd);
}
inline bool isSelect(opCodeEn x) {
    return (opCodeEn::select == x);
}
inline bool isCall(opCodeEn x) {
    return (opCodeEn::call == x);
}
inline bool isConvolve(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return (typeOpCodeEn::convolve_op <= t) && (t < typeOpCodeEn::convolve_opEnd);
}
inline bool isSlice(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return (typeOpCodeEn::slice_op <= t) && (t < typeOpCodeEn::slice_opEnd);
}
inline bool isDecimation(opCodeEn x) {
    return x == opCodeEn::decimation;
}
inline bool isShift(opCodeEn x) {
    return x == opCodeEn::shift;
}
inline bool isStoreToBuffer(opCodeEn x) {
    return (opCodeEn::storeToBuffer == x);
}
inline bool isBuiltInFunc(opCodeEn x) {
    typeOpCodeEn t = (typeOpCodeEn)x;
    return (typeOpCodeEn::builtInFunc <= t) && (t < typeOpCodeEn::builtInFuncEnd);
}




inline std::string typeToStr(TypeEn type)
{
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