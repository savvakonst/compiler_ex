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


//void buffer_update(bufferTable);


//#define PreRelise
int main(int argc, const char* argv[]) {
    
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
    std::unique_ptr<Module> Owner = std::make_unique<Module>("test", *context);
    Module * M =  Owner.get();

    try
    {
        llvm::outs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
        //listener.activBody->print();
        listener.activBody->symplyfy();
        llvm::outs() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        llvm::outs()<<listener.activBody->print("", false, true);
        listener.activBody->reduce();


        Table* table = new Table(M);
        TableGenContext context= TableGenContext(table);
        listener.activBody->genTable(&context);
        llvm::outs() << "~~print~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        table->print();
        llvm::outs() << "~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

        table->generateIR();
    }catch(size_t )
    {
        
    }



    return 0;
}

