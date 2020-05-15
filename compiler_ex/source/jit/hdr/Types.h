//===-- examples/HowToUseJIT/HowToUseJIT.cpp - An example use of the JIT --===//
#ifndef GO_CGO_EXPORT_PROLOGUE_H
#define GO_CGO_EXPORT_PROLOGUE_H

#include "stdint.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

#include <string.h>



typedef  int64_t GoInt;
typedef  GoInt GoJitType;
typedef struct {
    const char* p;
    ptrdiff_t n;
} GoString;

template < typename TType >
struct GoSlice_t{
    TType* data;
    int64_t len;
    int64_t cap;
};

template < typename TType > class GoSlice
{
public:
    GoSlice(int len);
    GoSlice(TType* array, int len);
    ~GoSlice();
    GoSlice_t <TType> getValue();
    GoSlice_t <TType> Slise;

private:

};

template<typename TType>
GoSlice<TType>::GoSlice(int len)
{
    Slise.data = new TType[len];
    Slise.cap = len;
    Slise.len = len;
}


template<typename TType>
GoSlice<TType>::GoSlice(TType* array, int len)
{
    Slise.data = new TType[len];
    Slise.cap = len;
    Slise.len = len;
    for (int i = 0; i < len; i++)
        Slise.data[i] = array[i];
}

template<typename TType>
GoSlice<TType>::~GoSlice()
{
    delete Slise.data;
}

template<typename TType>
GoSlice_t<TType> GoSlice<TType>::getValue()
{
    return Slise;
}




typedef int (*jit_f)(int *, int *);
typedef int (*jit_fptr)(int, int *);
typedef int (*Jit_Call_t)( int**);

typedef int32_t(*update_buffer_t)(int32_t);

namespace GoC {


enum {
  Add = 0,
  Sub,
  Mul,
  Div,
  FAdd,
  FSub,
  FMul,
  FDiv,
  Store,
  Load,
  InBoundsGEP,
  newBlock,
  End,
  Alloca_,
  CondBr,
  ICmpULT,
  BR,
  UpdateBuffer,
  And
};

enum {
  int32_T = 0,
  int32c_T,
  int64_T,
  int64c_T,
  float_T,
  floatC_T,
  double_T,
  doubleC_T,

  int32alloc_T,
  int32allocC_T,
  int64alloc_T,
  int64allocC_T,
  f32alloc_T,
  f32allocC_T,
  f64alloc_T,
  f64allocC_T,

  int32arg_T,
  int32argC_T,
  int64arg_T,
  int64argC_T,
  f32arg_T,
  f32argC_T,
  f64arg_T,
  f64argC_T
};

enum { intENDalloc_T = f64allocC_T, intENDarg_T = f64argC_T };

enum {
  Fp2si = 32,
  Si2fp,
  Fpext,
  Fptrunc,
  Sext,
  Trunc,
  Sin_,
  Cos_,
  Sqrt_,
  Pow_,
  Powi_,
  Exp_,
  Log_,
  Log10_,
  Fabs
};

} // namespace GoC

typedef struct {
  int64_t V;
  int64_t Type;
  int64_t len;
} value, *value_ptr; 

typedef struct {
  value ret;
  value val1;
  value val2;
  value val3;
  int64_t op;
  int64_t param;
} Command, *Command_ptr;

typedef struct {
  uint64_t len_com;
  Command_ptr commands;
  uint64_t len_val;
  value_ptr initVal;
  uint64_t len_var;
  uint64_t BasicBlock_N;
}  Header_ptr;



std::string VString2(value l, int i);
std::string VString(value l);
void printOp(int opN, int opCode);
void printValueT(int opN, int opCode);
void printValueT( int opCode);
using namespace llvm;

Type*  getType(value l, LLVMContext& Context);
Value* getValue(Value** ConstVars, Value** Vars, value l);



#endif

//===-- examples/HowToUseJIT/HowToUseJIT.cpp - An example use of the JIT --===//
#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoUint64 GoUint;

typedef float GoFloat32;
typedef double GoFloat64;

typedef char
    _check_for_64_bit_pointer_matching_GoInt[sizeof(void *) == 64 / 8 ? 1 : -1];

typedef void *GoMap;
typedef void *GoChan;
typedef struct {
  void *t;
  void *v;
} GoInterface;

#endif



