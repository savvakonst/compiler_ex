
#include <iostream>
#include <strstream>
#include <fstream>


#include "antlr4-runtime.h"
#include "EGrammarLexer.h"
#include "EGrammarParser.h"
#include "EGrammarBaseListener.h"

#include "body.h"
#include "Line.h"
#include "parser.h"
#include "llvmHdrs.h"



using namespace llvm;


void jit_init() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
}


//static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

void configOptimization(legacy::FunctionPassManager* TheFPM) {
    TheFPM->add(createPromoteMemoryToRegisterPass());
    TheFPM->add(createInstructionCombiningPass());
    TheFPM->add(createReassociatePass());
    TheFPM->add(createGVNPass());
    TheFPM->add(createCFGSimplificationPass());
    TheFPM->doInitialization();
}


int main(int argc, const char* argv[]) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    if (argc ==1)  return 1;
    const char* fileName=argv[1];


#ifdef PreRelise
    DEBUG_STREAM();
#else
    //fileName
    std::ifstream ifs(fileName);
    std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );

    ANTLRInputStream input(content+"\n");
#endif


    /*
    //raw_ostream* out = &outs();
    LLVMContext Context;

    std::unique_ptr<Module> Owner = std::make_unique<Module>("test", Context);
    Module* M = Owner.get();
    */

    //EErrorListener

    EGrammarLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    EGrammarParser parser(&tokens);
    
    parser.removeErrorListeners();
    EErrorListener errorListner;
    parser.addErrorListener(&errorListner);

    tree::ParseTree*  tree = parser.start();
    TreeShapeListener listener;

    LLVMContext* context = new LLVMContext() ;
    std::unique_ptr<Module> moduleUPtr = std::make_unique<Module>("test", *context);
    
    std::unique_ptr<legacy::FunctionPassManager> theFPM = std::make_unique<legacy::FunctionPassManager>(moduleUPtr.get());
    configOptimization(theFPM.get());

    try
    {
        llvm::outs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
        //listener.activBody->print();
        listener.activBody->symplyfy();
        llvm::outs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        llvm::outs()<<listener.activBody->print("", false, true);
        listener.activBody->reduce();


        Table* table = new Table(moduleUPtr.get());
        TableGenContext context= TableGenContext(table);
        
        listener.activBody->genTable(&context);
        llvm::outs() << "~~print~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        table->print();
        llvm::outs() << "~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        table->calculateBufferLength();

        table->generateIR();
        auto mainF=moduleUPtr->getFunction("main");
        theFPM->run(*mainF);
        llvm::outs() << "\n\n---------\nWe just constructed this LLVM module:\n\n---------\n" << *mainF<<"\n\n";


        std::string errStr;
        ExecutionEngine* EE = EngineBuilder(std::move(moduleUPtr)).setErrorStr(&errStr).create();
        if (!EE) {
            llvm::outs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
            return 1;
        }

        if (verifyFunction(*mainF, &llvm::outs())) {
            llvm::outs() << ": Error constructing FooF function!\n\n";
            return 1;
        }

        if (verifyModule(*moduleUPtr)) {
            llvm::outs() << ": Error constructing module!\n";
            return 1;
        }
        llvm::outs() << "complete";
    }catch(size_t ){

    }


    return 0;
}

