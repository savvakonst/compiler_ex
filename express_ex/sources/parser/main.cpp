
#include <iostream>
#include <strstream>
#include <fstream>

#include "ParameterIO.h"
#include "printer.h"

#include "body.h"
#include "treeShapeListener.h"


#include "defWarningIgnore.h"
#include "llvm/Support/CommandLine.h"
#include "undefWarningIgnore.h"

#include "express_ex.h"
 


bool         g_ansiEscapeCodes;

void clGpuConvolve();
int main(int argc, const char* argv[]) {



    enum  ShowNames {
        nameList, untypedFSR, activeNameList, allFSR, redusedFSR, tableSSR, outputPrm, llvmIRcode
    };

    static  llvm::cl::OptionCategory    mainCategory("Compiler Options", "Main Options ");
    static  llvm::cl::opt<std::string>  inputFile("i", llvm::cl::desc("input source file"), llvm::cl::value_desc("filename"), llvm::cl::Required, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         optimizationEnable("opt", llvm::cl::desc("optimization enable"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         ansiEscapeCodes("ansi", llvm::cl::desc("enable ANSI escape codes"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::list<std::string> libraryPaths("dir", llvm::cl::desc("list of available directories"), llvm::cl::value_desc("filename"), llvm::cl::ZeroOrMore, llvm::cl::cat(mainCategory));
    static  llvm::cl::list<std::string> moduleFiles("lib", llvm::cl::desc("list of available modules"), llvm::cl::value_desc("filename"), llvm::cl::ZeroOrMore, llvm::cl::cat(mainCategory));
    static  llvm::cl::list<std::string> inputDataBaseFile("db", llvm::cl::desc("input data base files"), llvm::cl::value_desc("directory"), llvm::cl::ZeroOrMore, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         runJit("runJit", llvm::cl::desc("run jit"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::bits<ShowNames>   showBits(llvm::cl::desc("show:"), llvm::cl::cat(mainCategory), llvm::cl::ZeroOrMore,
        llvm::cl::values(
            clEnumVal(nameList, "names list"),
            clEnumVal(untypedFSR, "FSR(first stage representation) without type calculation"),
            clEnumVal(activeNameList, "Procedure Integration"),
            clEnumVal(allFSR, "full FSR(first stage representation)"),
            clEnumVal(redusedFSR, "redused FSR code"),
            clEnumVal(tableSSR, "table (second stage) representation"),
            clEnumVal(outputPrm, "output parameters"),
            clEnumVal(llvmIRcode, "llvm IR ")));


    llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {OS << "version - 0.94_6\n"; });
    llvm::cl::HideUnrelatedOptions(mainCategory);
    llvm::cl::ParseCommandLineOptions(argc, argv, "express jit");

    //clGpuConvolve();

    g_ansiEscapeCodes=ansiEscapeCodes;

    ExColors colorReset = ExColors::RESET;
    ExColors colorRed   = ExColors::RED;
    ExColors colorGreen = ExColors::GREEN;
    Delimiter delimiter = Delimiter::GREEN;



    ParametersDB parameterDB(inputDataBaseFile);




    try{ 
        std::map<std::string, bool> modules_map;
        for (auto i : moduleFiles) modules_map[i]=true;

        KEXParser  parser(inputFile,true, modules_map);
        Body* body_brototype = parser.getActivBody();

        std::map<std::string, std::string> parameterNameList = body_brototype->getParameterLinkNames();

        if(showBits.isSet(nameList))
            llvm::outs()<< delimiter <<"names list: \n  "<< parameterNameList <<" \n";

        stack<Variable*> args;
        for (auto i : parameterNameList) {
            auto p =parameterDB[i.second];
            if (p != nullptr) {
                p->setLocal(false);
                args.push(new Line(i.first, p));
            }
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




        jit_init();
        Table*            table = new Table();
        TableGenContext context = TableGenContext(table);
        
        body->genTable(&context);

        int index=0;
        for (auto i : body->getOutputParameterList()) {
            i->setName("out_" + std::to_string(index++));
            i->setLocal(false);
        }
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



int main2(int argc, const char* argv[]) {

    enum  ShowNames {
        nameList, untypedFSR, activeNameList, allFSR, redusedFSR, tableSSR, outputPrm, llvmIRcode 
    };

    static  llvm::cl::OptionCategory    mainCategory("Compiler Options", "Main Options ");
    static  llvm::cl::opt<std::string>  inputFile("i", llvm::cl::desc("input source file"), llvm::cl::value_desc("filename"), llvm::cl::Required, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         optimizationEnable("opt", llvm::cl::desc("optimization enable"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         ansiEscapeCodes("ansi", llvm::cl::desc("enable ANSI escape codes"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::list<std::string> libraryPath("dir", llvm::cl::desc("list of available directories"), llvm::cl::value_desc("filename"), llvm::cl::ZeroOrMore, llvm::cl::cat(mainCategory));
    static  llvm::cl::list<std::string> inputDataBaseFile("db", llvm::cl::desc("input data base files"), llvm::cl::value_desc("directory"), llvm::cl::ZeroOrMore, llvm::cl::cat(mainCategory));
    static  llvm::cl::opt<bool>         runJit("runJit", llvm::cl::desc("run jit"), llvm::cl::Optional, llvm::cl::cat(mainCategory));
    static  llvm::cl::bits<ShowNames>   showBits(llvm::cl::desc("show:"), llvm::cl::cat(mainCategory), llvm::cl::ZeroOrMore,
        llvm::cl::values(
            clEnumVal(nameList, "names list"),
            clEnumVal(untypedFSR, "FSR(first stage representation) without type calculation"),
            clEnumVal(activeNameList, "Procedure Integration"),
            clEnumVal(allFSR, "full FSR(first stage representation)"),
            clEnumVal(redusedFSR, "redused FSR code"),
            clEnumVal(tableSSR, "table (second stage) representation"),
            clEnumVal(outputPrm, "output parameters"),
            clEnumVal(llvmIRcode, "llvm IR ")));


    llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {OS << "version - 0.94_6\n"; });
    llvm::cl::HideUnrelatedOptions(mainCategory);
    llvm::cl::ParseCommandLineOptions(argc, argv, "express jit");

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