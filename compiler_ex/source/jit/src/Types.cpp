 
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

#define _DECL_DLLMAIN

#include "Types.h"
#include <Process.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <windows.h>

//;$(llvmObjLibPath)\LLVMCore.lib; $(llvmObjLibPath)\LLVMExecutionEngine.lib; $(llvmObjLibPath)\LLVMInterpreter.lib; $(llvmObjLibPath)\LLVMSupport.lib;

std::string VString(value l) {
  std::string s;
  switch (l.Type) {
  case GoC::int32_T:
    s = "i32V";
    break;
  case GoC::int64_T:
    s = "i64V";
    break;
  case GoC::int32c_T:
    s = "i32C";
    break;
  case GoC::int64c_T:
    s = "i64C";
    break;

  case GoC::float_T:
    s = "f32V";
    break;
  case GoC::double_T:
    s = "f64V";
    break;

  case GoC::int32arg_T:
    s = "i32Arg";
    break;
  case GoC::int64arg_T:
    s = "i64Arg";
    break;

  case GoC::int32alloc_T:
    s = "i32Alloc";
    break;
  case GoC::int64alloc_T:
    s = "i64Alloc";
    break;
  default:
    break;
  }
  return s + std::to_string(l.V);
}
 
std::string VString2(value l, int i) {
  std::string s;
  switch (l.Type) {
  case GoC::int32_T: 
    s = "i32V";
    break;
  case GoC::int64_T:
    s = "i64V";
    break;
  case GoC::int32c_T:
    s = "i32C";
    break;
  case GoC::int64c_T:
    s = "i64C";
    break;

  case GoC::int32arg_T:
    s = "i32Arg";
    break;
  case GoC::int64arg_T:
    s = "i64Arg";
    break;

  case GoC::int32alloc_T:
    s = "i32Alloc";
    break;
  case GoC::int64alloc_T:
    s = "i64Alloc";
    break;
  default:
    break;
  }
  return s + std::to_string(i) + std::string("= init ") + std::to_string(l.V);
}

using namespace llvm;



Value *getValue(Value **ConstVars, Value **Vars, value l) {
  Value *s =NULL;

  switch (l.Type) {
  case GoC::int32_T:
    s = Vars[l.V];
    break;
  case GoC::int64_T:
    s = Vars[l.V];
    break;

  case GoC::int32c_T:
    s = ConstVars[l.V];
    break;
  case GoC::int64c_T:
    s = ConstVars[l.V];
    break;
  case GoC::float_T:
    s = Vars[l.V];
    break;
  case GoC::double_T:
    s = Vars[l.V];
    break;
  case GoC::floatC_T:
    s = ConstVars[l.V];
    break;
  case GoC::doubleC_T:
    s = ConstVars[l.V];
    break;


  case GoC::int32alloc_T:
    s = Vars[l.V];
    break;
  case GoC::int64alloc_T:
    s = Vars[l.V];
  case GoC::f32alloc_T:
    s = Vars[l.V];
    break;
  case GoC::f64alloc_T:
    s = Vars[l.V];
    break;

  case GoC::int32arg_T:
    s = ConstVars[l.V];
    break;
  case GoC::int64arg_T:
    s = ConstVars[l.V];
    break;
  case GoC::f32arg_T:
    s = ConstVars[l.V];
      break;
  case GoC::f64arg_T:
    s = ConstVars[l.V];
  default:
    break;
  }
  return s;
}

Type *getType(value l, LLVMContext &Context) {
  Type *s;

  switch (l.Type) {
  case GoC::int32_T:
    s = Type::getInt32Ty(Context);
    break;
  case GoC::int64_T:
    s = Type::getInt64Ty(Context);
    break;
  case GoC::int32c_T:
    s = Type::getInt32Ty(Context);
    break;
  case GoC::int64c_T:
    s = Type::getInt64Ty(Context);
    break;
  case GoC::float_T:
    s = Type::getFloatTy(Context);
    break;
  case GoC::floatC_T:
    s = Type::getFloatTy(Context);
    break;
  case GoC::double_T:
    s = Type::getDoubleTy(Context);
    break;
  case GoC::doubleC_T:
    s = Type::getDoubleTy(Context);
    break;

  case GoC::int32alloc_T:
    s = Type::getInt32PtrTy(Context);
    break;
  case GoC::int64alloc_T:
    s = Type::getInt64PtrTy(Context);
  case GoC::f32alloc_T:
    s = Type::getFloatPtrTy(Context);
    break;
  case GoC::f64alloc_T:
    s = Type::getDoublePtrTy(Context);
    break;

  case GoC::int32arg_T:
    s = Type::getInt32PtrTy(Context);
    break;
  case GoC::int64arg_T:
    s = Type::getInt64PtrTy(Context);
    break;
  case GoC::f32arg_T:
    s = Type::getFloatPtrTy(Context);
    break;
  case GoC::f64arg_T:
    s = Type::getDoublePtrTy(Context);
    break;
  default:
    break;
  }
  return s;
}

std::string lowOpNames[19]={"Add", "Sub", "Mul", "Div", "FAdd", "FSub", "FMul", "FDiv", "Store", "Load", "InBoundsGEP", "newBlock", "End", "Alloca_", "CondBr", "ICmpULT", "BR", "UpdateBuffer", "And"};
std::string highOpNames[15] = { "Fp2si","Si2fp","Fpext","Fptrunc","Sext","Trunc","Sin_","Cos_","Sqrt_","Pow_","Powi_","Exp_","Log_","Log10_","Fabs" };
std::string ValueTNames[24] = { "int32_T","int32c_T","int64_T","int64c_T","float_T","floatC_T","double_T","doubleC_T","int32alloc_T","int32allocC_T","int64alloc_T","int64allocC_T","f32alloc_T","f32allocC_T","f64alloc_T","f64allocC_T","int32arg_T","int32argC_T","int64arg_T","int64argC_T","f32arg_T","f32argC_T","f64arg_T","f64argC_T" };
void printValueT(int opN, int opCode)
{
    if (opCode <= 24)
        llvm::outs() << opN << ":" << ValueTNames[opCode] << "\n";
    else
        llvm::outs() << opN << ":\n";
}

void printValueT(int opCode)
{
    if (opCode <= 24)
        llvm::outs() <<  ":" << ValueTNames[opCode] << "";
    else
        llvm::outs() <<  ": error" << opCode;
}

void printOp(int opN,int opCode)
{
    if (opCode <= GoC::And)
        llvm::outs() << "\n" << opN << ":" << lowOpNames[opCode] ;
    else if (opCode >= GoC::Fp2si)
        llvm::outs() << "\n" << opN << ":" << lowOpNames[opCode- GoC::Fp2si] ;
    else
        llvm::outs() << "\n" << opN << ":";
}
