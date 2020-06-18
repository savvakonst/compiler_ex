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

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <stddef.h>
#include <stdio.h>
#include <windows.h>



#include "Types.h"
#include "IO.h"

using namespace llvm;

uint32_t update_buffer(uint32_t);


uint32_t getFunctionRetSize(Header_ptr* header) {
    uint32_t val;
    for (int i = 0; i < header->len_val; i++)
        if ((header->initVal[i].Type >= GoC::int32arg_T) && (header->initVal[i].Type <= GoC::intENDarg_T)) 
            val = ((uint32_t)(GoC::int32arg_T - header->initVal[i].Type)) >> 1;

    return ((val % 2) + 1) * 4;
}


extern Function* CreateMainFunction(Module * M, LLVMContext & Context, Header_ptr*header) {

    std::vector<Type*> FArgsList;

    for (int i = 0; i < header->len_val; i++) {
        if ((header->initVal[i].Type >= GoC::int32arg_T) && (header->initVal[i].Type <= GoC::intENDarg_T)) {
            FArgsList.push_back(getType(header->initVal[i], Context));
        }
    }

    FunctionType* Powi32_t = FunctionType::get(
        Type::getFloatTy(Context),
        { Type::getFloatTy(Context), Type::getFloatTy(Context) }, false);

    auto & context_r = M->getContext();

    //Declaration of f32 functions
    Function* Powi32 = Intrinsic::getDeclaration(M, Intrinsic::pow, Type::getFloatTy(Context));
    Function* Sini32 = Intrinsic::getDeclaration(M, Intrinsic::sin, Type::getFloatTy(Context));
    Function* Cosi32 = Intrinsic::getDeclaration(M, Intrinsic::cos, Type::getFloatTy(Context));
    Function* Expi32 = Intrinsic::getDeclaration(M, Intrinsic::exp, Type::getFloatTy(Context));
    Function* Logi32 = Intrinsic::getDeclaration(M, Intrinsic::log, Type::getFloatTy(Context));

    //Declaration of f64 functions
    Function* Powi64 = Intrinsic::getDeclaration(M, Intrinsic::pow, Type::getDoubleTy(Context));
    Function* Sini64 = Intrinsic::getDeclaration(M, Intrinsic::sin, Type::getDoubleTy(Context));
    Function* Cosi64 = Intrinsic::getDeclaration(M, Intrinsic::cos, Type::getDoubleTy(Context));
    Function* Expi64 = Intrinsic::getDeclaration(M, Intrinsic::exp, Type::getDoubleTy(Context));
    Function* Logi64 = Intrinsic::getDeclaration(M, Intrinsic::log, Type::getDoubleTy(Context));

    Function* MainF = Function::Create(
        FunctionType::get(Type::getInt32Ty(Context), FArgsList, false),
        Function::ExternalLinkage, "MainF", M);


    BasicBlock** BB = new BasicBlock * [header->BasicBlock_N + 1];
    for (int i = 0; i < header->BasicBlock_N + 1; i++) {
        BB[i] =
            BasicBlock::Create(Context, "EntryBlock_" + std::to_string(i), MainF);
    }
    
    IRBuilder<> builder(Context);

    assert(MainF->arg_begin() != MainF->arg_end());

    Value** ConstVars = new Value * [header->len_val];
     
    Value** Vars      = new Value * [header->len_var];

    // FunctionType *fdefinition_sqrt = FunctionType::get(FType, def_args, false);
    // Function *func_sqrt = Function::Create(fdefinition_sqrt,
    // GlobalValue::ExternalLinkage, "llvm.sqrt.f64", TheModule);
    // func_sqrt->setCallingConv(CallingConv::C);

    Command l;
    int arg_count = 0;
    for (int i = 0; i < header->len_val; i++) {
        if ((header->initVal[i].Type >= GoC::int32arg_T) && (header->initVal[i].Type <= GoC::intENDarg_T)) {
            ConstVars[i] = &*MainF->arg_begin() + arg_count;
            ConstVars[i]->setName("AnArg");
            arg_count++;
        }
        else {
            switch (header->initVal[i].Type) {
            case GoC::int32c_T:
                ConstVars[i] = builder.getInt32(header->initVal[i].V);
                break;
            case GoC::int64c_T:
                ConstVars[i] = builder.getInt64(header->initVal[i].V);
                break;
            case GoC::floatC_T:
                ConstVars[i] =ConstantFP::get(Type::getFloatTy(Context),(double)(*((float*)&(header->initVal[i].V))));
                break;
            case GoC::doubleC_T:
                ConstVars[i] = ConstantFP::get(Type::getDoubleTy(Context),*((double*)&(header->initVal[i].V)));
                break;
            }
        }
    }
    auto UpdateBufferF = builder.CreateIntToPtr(ConstantInt::get(builder.getInt64Ty(),
        uintptr_t(update_buffer)),
        PointerType::getUnqual(
            FunctionType::get(builder.getInt32Ty(),
                { builder.getInt32Ty() },
                false
            )
        ));

    Value* AOperand,* BOperand;
    Value** Ret;
    /*
    for (int i = 0; i < header->len_com; i++) {
        l = header->commands[i];


        AOperand = getValue(ConstVars, Vars, l.val1); //dangerous 
        BOperand = getValue(ConstVars, Vars, l.val2); //dangerous 
        Ret = &(Vars[l.ret.V]);

        switch (l.op) {
        case GoC::Add: 
            *Ret = builder.CreateAdd(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::Sub:
            *Ret = builder.CreateSub(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::Mul:
            *Ret = builder.CreateMul(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::Div:
            *Ret = builder.CreateSDiv(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::Powi_:
            *Ret = builder.CreateCall(AOperand->getType()->isDoubleTy() ? Powi64 : Powi32, { AOperand, BOperand }, "calltmp");
            break;
        case GoC::Sin_: 
            *Ret = builder.CreateCall(AOperand->getType()->isDoubleTy() ? Sini64 : Sini32, AOperand, "calltmp");
            break;
        case GoC::Cos_:
            *Ret = builder.CreateCall(AOperand->getType()->isDoubleTy() ? Cosi64 : Cosi32, AOperand, "calltmp");
            break;
        case GoC::Exp_:
            *Ret = builder.CreateCall(AOperand->getType()->isDoubleTy() ? Expi64 : Expi32, AOperand, "calltmp");
            break;
        case GoC::Log_:
            *Ret = builder.CreateCall(AOperand->getType()->isDoubleTy() ? Logi64 : Logi32, AOperand, "calltmp");
            break;
            //-----------------float point op-----------------------------
        case GoC::FAdd:
            *Ret = builder.CreateFAdd(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::FSub:
            *Ret = builder.CreateFSub(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::FMul:
            *Ret = builder.CreateFMul(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::FDiv:
            *Ret = builder.CreateFDiv(AOperand, BOperand, VString(l.ret));
            break;

            //-----------------type conv.----------------------------
        case GoC::Fp2si:
            *Ret = builder.CreateFPToSI(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
        case GoC::Si2fp:
            *Ret = builder.CreateSIToFP(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
        case GoC::Fpext:
            *Ret = builder.CreateFPExt(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
        case GoC::Fptrunc:
            *Ret = builder.CreateFPTrunc(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
        case GoC::Sext:
            *Ret = builder.CreateSExt(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
        case GoC::Trunc:
            *Ret = builder.CreateTrunc(AOperand, getType(l.ret, Context), VString(l.ret));
            break;
            //-----------------memory op-----------------------------
        case GoC::And:
            *Ret = builder.CreateAnd(AOperand, BOperand, VString(l.ret));
            break;
        case GoC::Load:
            *Ret = builder.CreateLoad(AOperand, false);
            break;
        case GoC::Store:
            builder.CreateStore(AOperand, BOperand, false);
            break;
        case GoC::Alloca_:
            *Ret = builder.CreateAlloca(getType(l.ret, Context), 0, "alloc.v");
            break;
        case GoC::InBoundsGEP:
            *Ret = builder.CreateInBoundsGEP(AOperand, BOperand, "Var.ptr");
            break;
        case GoC::ICmpULT:
            *Ret = builder.CreateICmpULT(AOperand, BOperand);
            break;
        case GoC::BR:
            builder.CreateBr(BB[l.val1.V]);
            break;
        case GoC::CondBr:
            builder.CreateCondBr(AOperand, BB[l.val2.V],BB[l.val3.V]);
            break;
        case GoC::UpdateBuffer: 
            builder.CreateCall( UpdateBufferF, ConstVars[0]);
            break;
        case GoC::newBlock: 
            builder.SetInsertPoint(BB[l.val1.V]);
            break;
        case GoC::End:;
            builder.SetInsertPoint(BB[header->BasicBlock_N]);
            builder.CreateRet(ConstVars[0]);
            break;
        default:
            break;
        }
    }
    // KnownFunction = (Function *)UpdateBufferF;
    //llvm::outs() << "\n Create Main Function successfull :\n ---------\n";
    return MainF;
    */
    return NULL;
}

extern Function* CreateCallFunction(Module* M, LLVMContext& Context) {
    

    Function* CallFunction =
        Function::Create(
            FunctionType::get(
                Type::getInt32Ty(Context), {
                    Type::getInt32PtrTy(Context)->getPointerTo() }, 
                false),
            Function::ExternalLinkage, 
            "CallFunction",
            M);

    BasicBlock* BB = BasicBlock::Create(Context, "EntryBlock", CallFunction);
    //IRBuilder<> builder(Context);
    IRBuilder<> builder(BB);
    builder.SetInsertPoint(BB);

    auto MainF = M->getFunction("MainF");
    auto MainFSignature = MainF->getFunctionType()->params();



    assert(CallFunction->arg_begin() != CallFunction->arg_end()); 
    Value* ArgI2Ptr = &*CallFunction->arg_begin() ;
    ArgI2Ptr->setName("ArgI2Ptr");
    Value* Var ;

    std::vector<Value*> FArgsList;
    
    for (Type*x : MainFSignature)
    {
        Var =builder.CreateLoad(ArgI2Ptr, true);
        //Var =builder.CreateAddrSpaceCast(Var, x,"VPointer");
        Var = builder.CreateBitCast(Var, x, "VPointer");
        FArgsList.push_back(Var);
        ArgI2Ptr = builder.CreateInBoundsGEP(Type::getInt32PtrTy(Context), ArgI2Ptr, builder.getInt32(1), "Var.2ptr");

    }
    
    CallInst* Add1CallRes = builder.CreateCall(MainF, FArgsList);
    Add1CallRes->setTailCall(true);
    builder.CreateRet(Add1CallRes);
    //llvm::outs() << "\n Create Call Function successfull :\n ---------\n";
    return CallFunction;
}