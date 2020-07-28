
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
#include "llvm/Support/JSON.h"
#include "ioIfs.h"


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




class KEXParser{
public:

    KEXParser(std::string fileName) {
        std::ifstream ifs(fileName);
        std::string content(
            (std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>())
        );

        //ANTLRInputStream    input(content + "\n");
        
        //EGrammarLexer       lexer(&input);
        //CommonTokenStream   tokens(&lexer);
        //EGrammarParser      parser(&tokens);

        input_  =new ANTLRInputStream(content + "\n");
        lexer_  =new EGrammarLexer(input_);
        tokens_ =new CommonTokenStream(lexer_);
        parser_ =new EGrammarParser(tokens_);

        parser_->removeErrorListeners();
        errorListner_ = new EErrorListener ;
        parser_->addErrorListener(errorListner_);

        tree_ = parser_->start();
    }
    ~KEXParser() {
        //delete tree_;
        delete errorListner_;
        delete parser_;
        delete tokens_;
        delete lexer_;
        delete input_;
    }

    TreeShapeListener* getListener() {
        return &listener_;
    }

    tree::ParseTree* getTree() {
        return tree_;
    }

    void walk() {
        tree::ParseTreeWalker::DEFAULT.walk(&listener_, tree_);
    }

    Body*  getActivBody() {
        return listener_.activBody_;
    }

private:
    ANTLRInputStream   *   input_;
    EGrammarLexer      *   lexer_;
    CommonTokenStream  *   tokens_;
    EGrammarParser     *   parser_;
    EErrorListener     *   errorListner_;

    tree::ParseTree*  tree_;
    TreeShapeListener listener_;
};




int main(int argc, const char* argv[]) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    const std::string delimiter = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    if (argc ==1)  return 1;
    const char* fileName=argv[1];

    
    std::vector<ParameterInfo> parametersList;
    if (argc>2)
        parametersList  = readParametersList(argv[2]);
    else
        parametersList  = readParametersList("A01_3.json");


    Parameters parameters(parametersList,"");

    KEXParser parser(fileName);

    LLVMContext* context = new LLVMContext() ;
    std::unique_ptr<Module> moduleUPtr = std::make_unique<Module>("test", *context);
    std::unique_ptr<legacy::FunctionPassManager> theFPM = std::make_unique<legacy::FunctionPassManager>(moduleUPtr.get());
    configOptimization(theFPM.get());

    try{
        parser.walk();
      
        auto body= parser.getActivBody();
        std::map<std::string, std::string> parameterNameList = body->getParameterLinkNames();

        llvm::outs() << delimiter <<"names list: \n  "<< parameterNameList <<" \n";


        stack<Variable*> args;
        for (auto i : parameterNameList) {
            auto p =parameters[i.second];
            if (!isEmpty(p))
                args.push(new Line(i.first, p));
        }


        llvm::outs() << delimiter << body->print("")<< delimiter<<"\n";
        body=parser.getActivBody()->genBodyByPrototype(args, false);
        llvm::outs() << delimiter << body->print("");
        return 1;

        body=parser.getActivBody()->genBodyByPrototype(args,false);
        body->symplyfy();



        llvm::outs()<< delimiter << body->print("", false, true);
        body->reduce(); //atavism


        Table* table = new Table(moduleUPtr.get());
        TableGenContext context= TableGenContext(table);
        
        body->genTable(&context);

        llvm::outs() << delimiter<< table->print();
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

