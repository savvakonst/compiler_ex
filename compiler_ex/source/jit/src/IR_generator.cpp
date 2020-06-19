

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
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

#include "IR_generator.h"
#include "table.h"
using namespace llvm;



IRGenerator::IRGenerator(LLVMContext & context,Table * table_):IRBuilder<>(context){
        table=table_;
    }

IRGenerator::~IRGenerator(){
    }



Value * IRGenerator::CreateFPow(Value *AOperand, Value *BOperand, std::string name) {
    if (table == NULL) return NULL;
    if (AOperand->getType() == getFloatTy())
        return (*table->getFloatBIFuncMap())[opCodeEn::FPOW];
    else
        return (*table->getDoubleBIFuncMap())[opCodeEn::FPOW];
}


Value * IRGenerator::CreateConst(uint64_t &binaryValue, TypeEn targetTy, std::string name) {
    Value * ret = NULL;

    auto &context = getContext();
    switch (targetTy)
    {
    case TypeEn::Int1_jty:   ret = getInt1(*((bool    *)(&binaryValue)));   break;
    case TypeEn::Int8_jty:   ret = getInt8(*((int8_t  *)(&binaryValue)));   break;
    case TypeEn::Int16_jty:  ret = getInt16(*((int16_t*)(&binaryValue)));   break;
    case TypeEn::Int32_jty:  ret = getInt32(*((int32_t*)(&binaryValue)));   break;
    case TypeEn::Int64_jty:  ret = getInt64(*((int64_t*)(&binaryValue)));   break;
    case TypeEn::Float_jty:  ret = ConstantFP::get(Type::getFloatTy(context), *((float  *)(&binaryValue)));  break;
    case TypeEn::Double_jty: ret = ConstantFP::get(Type::getDoubleTy(context), *((double *)(&binaryValue))); break;
    case TypeEn::Unknown_jty:ret = NULL;                                    break;
    default:                 ret = NULL;                                    break;
    }

    return ret;
}

Value * IRGenerator::CreateArithmetic(Value *AOperand, Value *BOperand, opCodeEn opCode, std::string name) {
    Value * ret = NULL;
    switch (opCode)
    {
    case opCodeEn::ADD:  ret = CreateAdd(AOperand, BOperand, name);  break;
    case opCodeEn::SUB:  ret = CreateSub(AOperand, BOperand, name);  break;
    case opCodeEn::MUL:  ret = CreateMul(AOperand, BOperand, name);  break;
    case opCodeEn::SDIV: ret = CreateSDiv(AOperand, BOperand, name); break;
    case opCodeEn::SREM: ret = CreateSRem(AOperand, BOperand, name); break;
    case opCodeEn::POW:  ret = NULL; break;

    case opCodeEn::FADD: ret = CreateFAdd(AOperand, BOperand, name); break;
    case opCodeEn::FSUB: ret = CreateFSub(AOperand, BOperand, name); break;
    case opCodeEn::FMUL: ret = CreateFMul(AOperand, BOperand, name); break;
    case opCodeEn::FDIV: ret = CreateFDiv(AOperand, BOperand, name); break;
    case opCodeEn::FREM: ret = CreateFRem(AOperand, BOperand, name); break;
    case opCodeEn::FPOW: ret = CreateFPow(AOperand, BOperand, name); break;
    }
    return ret;


}

Value * IRGenerator::CreateInv(Value *AOperand, opCodeEn opCode, std::string name) {
    Value * ret = NULL;

    if (opCode == opCodeEn::NEG)
        return CreateNeg(AOperand, name);
    else
        return CreateFNeg(AOperand,name);
}

Value * IRGenerator::CreateTypeConv(llvm::Value * AOperand, opCodeEn opCode, TypeEn targetTy, std::string name)
{
    Type* destTy = NULL;

    switch (targetTy)
    {
    case TypeEn::Int1_jty:      destTy=getInt1Ty();  break;
    case TypeEn::Int8_jty:      destTy=getInt8Ty();  break;
    case TypeEn::Int16_jty:     destTy=getInt16Ty(); break;
    case TypeEn::Int32_jty:     destTy=getInt32Ty(); break;
    case TypeEn::Int64_jty:     destTy=getInt64Ty(); break;
    case TypeEn::Float_jty:     destTy=getFloatTy(); break;
    case TypeEn::Double_jty:    destTy=getDoubleTy();break;
    default:break;
    }

    Value * ret=NULL;
    switch (opCode)
    {
    case opCodeEn::fptrunc: ret = CreateFPTrunc(AOperand, destTy, name);  break;
    case opCodeEn::fpext:   ret = CreateFPExt(AOperand, destTy, name);    break;
    case opCodeEn::sitofp:  ret = CreateSIToFP(AOperand, destTy, name);   break;
    case opCodeEn::fptosi:  ret = CreateFPToSI(AOperand, destTy, name);   break;
    case opCodeEn::trunc:   ret = CreateTrunc(AOperand, destTy, name);    break;
    case opCodeEn::sext:    ret = CreateSExt(AOperand, destTy, name);     break;
    }
    return ret;
}

Value * IRGenerator::CreateBuiltInFunc(llvm::Value * AOperand, opCodeEn opCode, std::string name)
{
    if (table == NULL) return NULL;
    if (AOperand->getType() == getFloatTy())
        return (*table->getFloatBIFuncMap())[opCode];
    else
        return (*table->getDoubleBIFuncMap())[opCode];

}









