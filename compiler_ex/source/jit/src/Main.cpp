//===-- examples/HowToUseJIT/HowToUseJIT.cpp - An example use of the JIT --===//
//

//..\..\Release\lib\LLVMCore.lib;..\..\Release\lib\LLVMExecutionEngine.lib;..\..\Release\lib\LLVMInterpreter.lib;..\..\Release\lib\LLVMMC.lib;..\..\Release\lib\LLVMMCJIT.lib;..\..\Release\lib\LLVMSupport.lib;..\..\Release\lib\LLVMX86CodeGen.lib;..\..\Release\lib\LLVMX86Desc.lib;..\..\Release\lib\LLVMX86Info.lib;..\..\Release\lib\LLVMExecutionEngine.lib;..\..\Release\lib\LLVMRuntimeDyld.lib;..\..\Release\lib\LLVMMCDisassembler.lib;..\..\Release\lib\LLVMAsmPrinter.lib;..\..\Release\lib\LLVMDebugInfoDWARF.lib;..\..\Release\lib\LLVMGlobalISel.lib;..\..\Release\lib\LLVMSelectionDAG.lib;..\..\Release\lib\LLVMCodeGen.lib;..\..\Release\lib\LLVMTarget.lib;..\..\Release\lib\LLVMBitWriter.lib;..\..\Release\lib\LLVMScalarOpts.lib;..\..\Release\lib\LLVMAggressiveInstCombine.lib;..\..\Release\lib\LLVMInstCombine.lib;..\..\Release\lib\LLVMTransformUtils.lib;..\..\Release\lib\LLVMAnalysis.lib;..\..\Release\lib\LLVMObject.lib;..\..\Release\lib\LLVMBitReader.lib;..\..\Release\lib\LLVMMCParser.lib;..\..\Release\lib\LLVMProfileData.lib;..\..\Release\lib\LLVMCore.lib;..\..\Release\lib\LLVMRemarks.lib;..\..\Release\lib\LLVMX86AsmPrinter.lib;..\..\Release\lib\LLVMMC.lib;..\..\Release\lib\LLVMBinaryFormat.lib;..\..\Release\lib\LLVMDebugInfoCodeView.lib;..\..\Release\lib\LLVMDebugInfoMSF.lib;..\..\Release\lib\LLVMX86Utils.lib;..\..\Release\lib\LLVMSupport.lib;psapi.lib;shell32.lib;ole32.lib;uuid.lib;advapi32.lib;delayimp.lib;-delayload:shell32.dll;-delayload:ole32.dll;..\..\Release\lib\LLVMDemangle.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib
//..\..\Release\lib\LLVMCore.lib;..\..\Release\lib\LLVMExecutionEngine.lib;..\..\Release\lib\LLVMInterpreter.lib;..\..\Release\lib\LLVMSupport.lib;..\..\Release\lib\LLVMX86CodeGen.lib;..\..\Release\lib\LLVMX86Desc.lib;..\..\Release\lib\LLVMX86Info.lib;..\..\Release\lib\LLVMExecutionEngine.lib;..\..\Release\lib\LLVMRuntimeDyld.lib;..\..\Release\lib\LLVMMCDisassembler.lib;..\..\Release\lib\LLVMAsmPrinter.lib;..\..\Release\lib\LLVMDebugInfoDWARF.lib;..\..\Release\lib\LLVMGlobalISel.lib;..\..\Release\lib\LLVMSelectionDAG.lib;..\..\Release\lib\LLVMCodeGen.lib;..\..\Release\lib\LLVMTarget.lib;..\..\Release\lib\LLVMBitWriter.lib;..\..\Release\lib\LLVMScalarOpts.lib;..\..\Release\lib\LLVMAggressiveInstCombine.lib;..\..\Release\lib\LLVMInstCombine.lib;..\..\Release\lib\LLVMTransformUtils.lib;..\..\Release\lib\LLVMAnalysis.lib;..\..\Release\lib\LLVMObject.lib;..\..\Release\lib\LLVMBitReader.lib;..\..\Release\lib\LLVMMCParser.lib;..\..\Release\lib\LLVMProfileData.lib;..\..\Release\lib\LLVMCore.lib;..\..\Release\lib\LLVMRemarks.lib;..\..\Release\lib\LLVMX86AsmPrinter.lib;..\..\Release\lib\LLVMMC.lib;..\..\Release\lib\LLVMBinaryFormat.lib;..\..\Release\lib\LLVMDebugInfoCodeView.lib;..\..\Release\lib\LLVMDebugInfoMSF.lib;..\..\Release\lib\LLVMX86Utils.lib;..\..\Release\lib\LLVMSupport.lib;psapi.lib;shell32.lib;ole32.lib;uuid.lib;advapi32.lib;delayimp.lib;-delayload:shell32.dll;-delayload:ole32.dll;..\..\Release\lib\LLVMDemangle.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib

// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This small program provides an example of how to quickly build a small
//  module with two functions and execute it with the JIT.
//
// Goal:
//  The goal of this snippet is to create in the memory
//  the LLVM module consisting of two functions as follow:
//
// int add1(int x) {
//   return x+1;
// }
//
// int foo() {
//   return add1(10);
// }
//
// then compile the module via JIT, then execute the `foo'
// function and return result to a driver, i.e. to a "host program".
//
// Some remarks and questions:
//
// - could we invoke some code using noname functions too?
//   e.g. evaluate "foo()+foo()" without fears to introduce
//   conflict of temporary function name with some real
//   existing function name?
//
//===----------------------------------------------------------------------===//
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


#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <stddef.h>
#include <stdio.h>
#include <windows.h>

#include "Types.h"
#include "IO.h"
#include "LibInterface.h"
#include "CommandLineArgParser.h"


using namespace llvm;


Function *KnownFunction;


#define DLLEXPORT __declspec(dllexport)

extern "C" DLLEXPORT float powe(float x, float y) { return 400.0; }

uint32_t getFunctionRetSize(Header_ptr* header);


extern IOInterface* IO;
uint32_t update_buffer(uint32_t i) {
    return IO->update_buffer(i);
}

static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

void ConfigOptimization() {
    TheFPM->add(createPromoteMemoryToRegisterPass());
    TheFPM->add(createInstructionCombiningPass());
    TheFPM->add(createReassociatePass());
    TheFPM->add(createGVNPass());
    TheFPM->add(createCFGSimplificationPass());
    TheFPM->doInitialization();
}




int main2(int argc, char** argv) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    auto FInfo=CommandLineArgParser(argc,argv);
    if (FInfo==NULL)
        return 1;

    std::string errStr;
    raw_ostream* out = &outs();
    LLVMContext Context;

    LibInterface* Lib = new LibInterface(FInfo->ExprORFilename, FInfo->FileExprSource);
    IO = new IOInterface(FInfo);

    std::unique_ptr<Module> Owner = std::make_unique<Module>("test", Context);
    Module* M = Owner.get();
    TheFPM = std::make_unique<legacy::FunctionPassManager>(M);
    ConfigOptimization();

    Header_ptr* header = Lib->CallCalcFunction(IO->BufferSize, FInfo->dataSize, FInfo->iTypes);

    IO->OTypeSize= getFunctionRetSize(header);
    Function* FooF = CreateMainFunction(M, Context, header);
    Function* CallF = CreateCallFunction(M, Context);

    TheFPM->run(*CallF);

    llvm::outs() << "We just constructed this LLVM module:\n\n---------\n" << *M;

    ExecutionEngine* EE = EngineBuilder(std::move(Owner)).setErrorStr(&errStr).create();

    if (!EE) {
        errs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
        return 1;
    }

    if (verifyFunction(*CallF, out)) {
        errs() << ": Error constructing CallF function!\n\n";
        return 1;
    }

    if (verifyFunction(*FooF, out)) {
        errs() << ": Error constructing FooF function!\n\n";
        return 1;
    }

    if (verifyModule(*M)) {
        errs() << ": Error constructing module!\n";
        return 1;
    }

    EE->finalizeObject();
    llvm::outs() << "finalize Object successful:\n\n---------\n";


    Jit_Call_t Call = (Jit_Call_t)EE->getPointerToFunction(CallF);


    int** arg = new int* [IO->ICounter + 1];
    for (int i = 0; i < IO->ICounter; i++) arg[i]=(int*)IO->Buffers.IBuffer_array[i];
    arg[IO->ICounter] = (int*)IO->Buffers.OBuffer;
    int res = Call(arg);
    IO->close_file();
    delete EE;
    llvm_shutdown();

    return 1;
}


int main3(int argc, char** argv) {


    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    auto FInfo = CommandLineArgParser(argc, argv);
    if (FInfo == NULL)
        return 1;

    std::string errStr;
    raw_ostream* out = &outs();
    LLVMContext Context;

    LibInterface* Lib = new LibInterface(FInfo->ExprORFilename, FInfo->FileExprSource);
    IO = new IOInterface(FInfo);

    std::unique_ptr<Module> Owner = std::make_unique<Module>("test", Context);

    Module* M = Owner.get();
    /* optimization block start*/
    TheFPM = std::make_unique<legacy::FunctionPassManager>(M); 
    ConfigOptimization();
    /* optimization block end*/

    Header_ptr* header = Lib->CallCalcFunction(IO->BufferSize, FInfo->dataSize, FInfo->iTypes);

    IO->OTypeSize = getFunctionRetSize(header);
    Function* FooF = CreateMainFunction(M, Context, header);
    Function* CallF = CreateCallFunction(M, Context);

    TheFPM->run(*CallF);

    llvm::outs() << "We just constructed this LLVM module:\n\n---------\n" << *M;

    ExecutionEngine* EE = EngineBuilder(std::move(Owner)).setErrorStr(&errStr).create();

    if (!EE) {
        errs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
        return 1;
    }

    if (verifyFunction(*CallF, out)) {
        errs() << ": Error constructing CallF function!\n\n";
        return 1;
    }

    if (verifyFunction(*FooF, out)) {
        errs() << ": Error constructing FooF function!\n\n";
        return 1;
    }

    if (verifyModule(*M)) {
        errs() << ": Error constructing module!\n";
        return 1;
    }

    EE->finalizeObject();
    llvm::outs() << "finalize Object successful:\n\n---------\n";


    Jit_Call_t Call = (Jit_Call_t)EE->getPointerToFunction(CallF);


    int** arg = new int* [IO->ICounter + 1];
    for (int i = 0; i < IO->ICounter; i++) arg[i] = (int*)IO->Buffers.IBuffer_array[i];
    arg[IO->ICounter] = (int*)IO->Buffers.OBuffer;
    int res = Call(arg);
    IO->close_file();
    delete EE;
    llvm_shutdown();

    return 1;
}