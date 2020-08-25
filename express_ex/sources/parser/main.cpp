
#include <iostream>
#include <strstream>
#include <fstream>

#include "ParameterIO.h"
#include "printer.h"

#include "body.h"
#include "treeShapeListener.h"

#include "llvm/Support/CommandLine.h"
#include "express_ex.h"
 
using namespace llvm;




bool         g_ansiEscapeCodes;



void jit_init();


int main2(int argc, const char* argv[]) {

    enum  ShowNames {
        nameList, untypedFSR, activeNameList, allFSR, redusedFSR, tableSSR, outputPrm, llvmIRcode
    };

    static  cl::OptionCategory    mainCategory("Compiler Options", "Main Options ");
    static  cl::opt<std::string>  inputFile("i", cl::desc("input source file"), cl::value_desc("filename"), cl::Required, cl::cat(mainCategory));
    static  cl::opt<bool>         optimizationEnable("opt", cl::desc("optimization enable"), cl::Optional, cl::cat(mainCategory));
    static  cl::opt<bool>         ansiEscapeCodes("ansi", cl::desc("enable ANSI escape codes"), cl::Optional, cl::cat(mainCategory));
    static  cl::list<std::string> libraryPath("dir", cl::desc("list of available directories"), cl::value_desc("filename"), cl::ZeroOrMore, cl::cat(mainCategory));
    static  cl::list<std::string> inputDataBaseFile("db", cl::desc("input data base files"), cl::value_desc("directory"), cl::ZeroOrMore, cl::cat(mainCategory));
    static  cl::opt<bool>         runJit("runJit", cl::desc("run jit"), cl::Optional, cl::cat(mainCategory));
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

    ExColors colorReset = ExColors::RESET;
    ExColors colorRed   = ExColors::RED;
    ExColors colorGreen = ExColors::GREEN;
    Delimiter delimiter = Delimiter::GREEN;



    ParametersDB parameterDB(inputDataBaseFile);

    KEXParser  parser(inputFile);


    try{
        parser.walk();
        Body* body_brototype = parser.getActivBody();

        std::map<std::string, std::string> parameterNameList = body_brototype->getParameterLinkNames();

        if(showBits.isSet(nameList))
            llvm::outs()<< delimiter <<"names list: \n  "<< parameterNameList <<" \n";

        stack<Variable*> args;
        for (auto i : parameterNameList) {
            auto p =parameterDB[i.second];
            if (p!=nullptr)
                args.push(new Line(i.first, p));
            else
                print_error("can not find parameter " + i.second);
        }

        if (showBits.isSet(untypedFSR))
            llvm::outs() << delimiter << body_brototype->print("")<< delimiter <<"\n";


        if (0 == inputDataBaseFile.size()) {
            llvm::outs() << colorRed << "there are no input database file\n" << colorReset;
            return 1;
        }

        Body* body = body_brototype->genBodyByPrototype(args, false);
        body->symplyfy();

        /// print block 
        if (showBits.isSet(activeNameList))
            llvm::outs() << delimiter << "names list: \n  " << body->getParameterLinkNames(true) << " \n";

        if (showBits.isSet(allFSR))
            llvm::outs() << delimiter << body->print("");

        if (showBits.isSet(redusedFSR))
            llvm::outs() << delimiter << body->print("", false, true);

        body->reduce(); //atavism


        jit_init();
        Table*            table = new Table();
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
        table->llvmInit();
        table->generateIR();

        if (optimizationEnable)
            table->runOptimization();

        if (showBits.isSet(llvmIRcode))
            table->printllvmIr();

        if (runJit)
            table->run();

        llvm::outs() << "complete";
    }catch(size_t ){

    }


    return 0;
}



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
    static  cl::opt<bool>         runJit("runJit", cl::desc("run jit"), cl::Optional, cl::cat(mainCategory));
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

    ParametersDB parameterDB(inputDataBaseFile);
    std::string content;
    std::ifstream ifs(inputFile);
    content=    std::string(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));


    jit_init();
    Express_ex* express_ex = new Express_ex();


    express_ex->parseText(content);
    auto map = express_ex->getParameterLinkNamesMap();

    std::map<std::string, SyncParameter*> parameters_map;
    for (auto i : map)
        parameters_map[i.first]=(parameterDB[i.second]);

    express_ex->setParameters(parameters_map);
    express_ex->getOutputParameterVector(); //
    express_ex->genJit();
    express_ex->run();
    llvm::outs() << "complete\n";

    express_ex->parseText(content);
    map = express_ex->getParameterLinkNamesMap();

    std::map<std::string, SyncParameter*> parameters_map2;
    for (auto i : map)
        parameters_map2[i.first]=(parameterDB[i.second]);

    express_ex->setParameters(parameters_map2);

    int index=0;
    for (auto i : express_ex->getOutputParameterVector())
        i->setName("new_" + std::to_string(index++));
    express_ex->genJit();
    express_ex->run();
    llvm::outs() << "complete";

    return 0;
}