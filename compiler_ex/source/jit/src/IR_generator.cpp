

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
        return CreateCall(table->getFloatBIFunc(opCodeEn::FPOW), { AOperand, BOperand }, name);
    else
        return CreateCall(table->getDoubleBIFunc(opCodeEn::FPOW), { AOperand, BOperand }, name);
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
    Type* destTy = getLLVMType(targetTy);

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
    if (AOperand->getType() == getFloatTy()) {
        return CreateCall(table->getFloatBIFunc(opCode), { AOperand }, name);
    }
    else {
        return CreateCall(table->getDoubleBIFunc(opCode), { AOperand }, name);
    }

}

Value * IRGenerator::CreatePositionalAlloca(llvm::Type * ty,unsigned int i, std::string name)
{
    SetInitInsertPoint();
    Value* ret = CreateAlloca(ty, 0, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalLoad(llvm::Value * AOperand, const std::string name)
{
    SetLoadInsertPoint();
    Value* ret = CreateLoad(AOperand, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalLoad(llvm::Value * AOperand, bool isVolatile, std::string name)
{
    SetLoadInsertPoint();
    Value* ret = CreateLoad(AOperand, isVolatile, name);
    SetCalcInsertPoint();
    return ret;
}

Value * IRGenerator::CreateConvolve(Value * AOperand, Value * BOperand, std::string name)
{
    Value * ret =NULL,* convolveFunction=NULL;
    Type * type=AOperand->getType()->getPointerElementType();

    if (type == getDoubleTy())
        convolveFunction=convolveDoubleFunction;
    else if (type == getFloatTy())
        convolveFunction=convolveFloatFunction;
    else if (type == getInt64Ty())
        convolveFunction=convolveI64Function;
    else if (type == getInt32Ty())
        convolveFunction=convolveI32Function;
    else
        return ret;


    //if (ret == NULL) print_error("CreateConvolve :" );

    return CreateCall(convolveFunction, { AOperand ,BOperand }, name);
}



void IRGenerator::SetDeclareConvolve(llvm::Type * type, uintptr_t addr)
{
    typedef Value * (*lambla_ty)(llvm::Type * type, uintptr_t addr);

    auto  lambda =[this](llvm::Type * type, uintptr_t addr) {
        return CreateIntToPtr(
            ConstantInt::get(getInt64Ty(),
                uintptr_t(addr)),
            PointerType::getUnqual(
                FunctionType::get(type,
                    { type->getPointerTo(), type->getPointerTo() },
                    false
                )
            ));
    };

    if (type == getDoubleTy())
        convolveDoubleFunction=lambda(type, addr);
    else if (type == getFloatTy())
        convolveFloatFunction=lambda(type, addr);
    else if (type == getInt64Ty())
        convolveI64Function=lambda(type, addr);
    else if (type == getInt32Ty())
        convolveI32Function=lambda(type, addr);
    
}

llvm::Type * IRGenerator::getLLVMType(TypeEn targetTy) {
    Type * ret = NULL;
    //this->SetInsertPoint
    switch (targetTy)
    {
    case TypeEn::Int1_jty:   ret = getInt1Ty();   break;
    case TypeEn::Int8_jty:   ret = getInt8Ty();   break;
    case TypeEn::Int16_jty:  ret = getInt16Ty();  break;
    case TypeEn::Int32_jty:  ret = getInt32Ty();  break;
    case TypeEn::Int64_jty:  ret = getInt64Ty();  break;
    case TypeEn::Float_jty:  ret = getFloatTy();  break;
    case TypeEn::Double_jty: ret = getDoubleTy(); break;
    case TypeEn::Unknown_jty:ret = NULL;          break;
    default:                 ret = NULL;          break;
    }
    
    return ret;
}