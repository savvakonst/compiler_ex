
#include <iostream>
#include <strstream>
#include <fstream>

#include "ParameterIO.h"
#include "printer.h"

#include "body.h"
#include "treeShapeListener.h"
#include "llvmHdrs.h"
#include "llvm/Support/CommandLine.h"

 
using namespace llvm;


void jit_init() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
}


//static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

void configOptimization(legacy::FunctionPassManager* TheFPM) {
    TheFPM->add(createPartiallyInlineLibCallsPass());
    TheFPM->add(createPromoteMemoryToRegisterPass());
    TheFPM->add(createInstructionCombiningPass());
    TheFPM->add(createReassociatePass());
    TheFPM->add(createGVNPass());
    TheFPM->add(createCFGSimplificationPass());
    TheFPM->doInitialization();
}

bool         g_ansiEscapeCodes;






int main(int argc, const char* argv[]) {

    enum  ShowNames {
        nameList, untypedFSR, activeNameList, allFSR, redusedFSR, tableSSR, outputPrm, llvmIRcode
    };

    static  cl::OptionCategory    mainCategory("Compiler Options", "Main Options ");
    static  cl::opt<std::string>  inputFile("i", cl::desc("input source file"), cl::value_desc("filename"), cl::Required, cl::cat(mainCategory));
    static  cl::opt<bool>         optimizationEnable("opt", cl::desc("optimization enable"), cl::Optional, cl::cat(mainCategory));
    static  cl::opt<bool>         ansiEscapeCodes("ansi", cl::desc("enable ANSI escape codes"), cl::Optional, cl::cat(mainCategory));
    static  cl::list<std::string> libraryPath("dir", cl::desc("list of available directories"), cl::value_desc("filename"), cl::ZeroOrMore, cl::cat(mainCategory));
    static  cl::list<std::string> inputDataBaseFile("db", cl::desc("input data base files"), cl::value_desc("directory"), cl::ZeroOrMore, cl::cat(mainCategory));
    static  cl::bits<ShowNames>   showBits(cl::desc("show:"), cl::cat(mainCategory), cl::ZeroOrMore,
        cl::values(
            clEnumVal(nameList, "names list"),
            clEnumVal(untypedFSR, "FSR(first stage representation) without type calculation"),
            clEnumVal(activeNameList, "Procedure Integration"),
            clEnumVal(allFSR, "full FSR(first stage representation)"),
            clEnumVal(redusedFSR, "redused FSR code"),
            clEnumVal(tableSSR, "table (second stage) representation"),
            clEnumVal(outputPrm, "output parameters"),
            clEnumVal(llvmIRcode, "llvm IR ")));


    cl::SetVersionPrinter([](llvm::raw_ostream &OS) {OS << "version - 0.94_6\n"; });
    cl::HideUnrelatedOptions(mainCategory);
    cl::ParseCommandLineOptions(argc, argv, "express jit");


    g_ansiEscapeCodes=ansiEscapeCodes;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();


    ExColors colorReset = ExColors::RESET;
    ExColors colorRed   = ExColors::RED;
    ExColors colorGreen = ExColors::GREEN;
    Delimiter delimiter = Delimiter::GREEN;



    ParametersDB parameterDB(inputDataBaseFile);

    KEXParser  parser(inputFile);

    LLVMContext* context = new LLVMContext() ;
    std::unique_ptr<Module> moduleUPtr                  = std::make_unique<Module>("test", *context);
    std::unique_ptr<legacy::FunctionPassManager> theFPM = std::make_unique<legacy::FunctionPassManager>(moduleUPtr.get());

    configOptimization(theFPM.get());

    try{
        parser.walk();
        auto body= parser.getActivBody();

        std::map<std::string, std::string> parameterNameList = body->getParameterLinkNames();

        if(showBits.isSet(nameList))
            llvm::outs()<< delimiter <<"names list: \n  "<< parameterNameList <<" \n";

        stack<Variable*> args;
        for (auto i : parameterNameList) {
            auto p =parameterDB[i.second];
            if (p!=NULL)
                args.push(new Line(i.first, p));
            else
                print_error("can not find parameter " + i.second);
        }

        if (showBits.isSet(untypedFSR))
            llvm::outs() << delimiter << body->print("")<< delimiter <<"\n";


        if (0 == inputDataBaseFile.size()) {
            llvm::outs() << colorRed << "there are no input database file\n" << colorReset;
            return 1;
        }

        body=parser.getActivBody()->genBodyByPrototype(args, false);
        body->symplyfy();

        /// print block 
        ///
        ///
        if (showBits.isSet(activeNameList))
            llvm::outs() << delimiter << "names list: \n  " << body->getParameterLinkNames(true) << " \n";

        if (showBits.isSet(allFSR))
            llvm::outs() << delimiter << body->print("");

        if (showBits.isSet(redusedFSR))
            llvm::outs() << delimiter << body->print("", false, true);

        body->reduce(); //atavism

        Module *M = moduleUPtr.get();

        Table*            table = new Table(M);
        TableGenContext context = TableGenContext(table);
        
        body->genTable(&context);

        

        int index=0;
        for (auto i : body->getOutputParameterList())
            i->setName("out_" + std::to_string(index++));

        if (showBits.isSet(outputPrm)) {
            index=0;
            for (auto i : body->getOutputParameterList()) {
                llvm::outs() << delimiter << *i;
            }
        }


        if (showBits.isSet(tableSSR))
            llvm::outs() << delimiter << table->print();


        table->calculateBufferLength();
        table->generateIR();

        auto mainF=M->getFunction("main");

        if(optimizationEnable)
            theFPM->run(*mainF);

        if (showBits.isSet(llvmIRcode))
            llvm::outs() << colorGreen << "\n\n---------We just constructed this LLVM module:--------- \n" << colorReset << *mainF<<"\n\n";

        
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
        
        if (verifyModule(*M)) {
            llvm::outs() << ": Error constructing module!\n";
            return 1;
        }

        Jit_Call_t Call = (Jit_Call_t)EE->getPointerToFunction(mainF);

        llvm::outs() << "complete";
    }catch(size_t ){

    }


    return 0;
}

