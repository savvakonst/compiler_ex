

#include "CommandLineArgParser.h"
#include "Types.h"
#include "IO.h"
using namespace llvm;

cl::OptionCategory MainCategory("Compiler Options", "Main Options ");

/*
enum Opts {
    // 'inline' is a C++ keyword, so name it 'inlining'
    dce, constprop, inlining, strip
};

cl::list<Opts> OptimizationList("l",cl::desc("Available Optimizations:"),
        cl::values(
        clEnumVal(dce, "Dead Code Elimination"),
        clEnumVal(constprop, "Constant Propagation"),
        clEnumValN(inlining, "inline", "Procedure Integration"),
        clEnumVal(strip, "Strip Symbols")) , cl::cat(MainCategory));

*/
 

static cl::opt<bool>         FileExprSource("f_expr_source", cl::desc("enable file based expression source"), cl::cat(MainCategory));
static cl::opt<std::string>  ExprORFilename("expr", cl::desc("script filename or str expr"), cl::value_desc("filename/expression"), cl::Required, cl::cat(MainCategory));
static cl::opt<uint32_t>     DataSize("s", cl::desc("Data   size"), cl::value_desc("len in bytes"), cl::Required, cl::cat(MainCategory));
static cl::list<std::string> OutputFile("o", cl::desc("output filename"), cl::value_desc("filename"), cl::multi_val(1), cl::Required, cl::cat(MainCategory));
static cl::list<std::string> InputFile("i", cl::desc("Input  files"), cl::value_desc("filename type"), cl::multi_val(2), cl::OneOrMore, cl::cat(MainCategory)); //, cl::OneOrMore cl::CommaSeparated ,

//cl::list<std::string, cl::list<std::string>> List("list",cl::desc("option list -- This option turns on options a when 'foo' is included in list"), cl::CommaSeparated,  cl::callback([&](const std::string& Str) {if (Str == "foo") OptA = true; }), cl::cat(MainCategory));
//static cl::opt<std::string>InputSize      ("s", cl::desc("Data   size")     , cl::value_desc("filenames"), cl::Required  , cl::cat(MainCategory));
//static cl::list<std::string> InputFiles(cl::Positional, cl::OneOrMore, cl::desc("input files"));
//static cl::list<std::string> InputArgv("args", cl::Positional, cl::desc("<program arguments>..."), cl::ZeroOrMore, cl::PositionalEatsArgs);


FilesInfo* CommandLineArgParser(int argc, char** argv) {
    cl::HideUnrelatedOptions(MainCategory);
    cl::OptionCategory AnotherCategory("Some options");
    cl::ParseCommandLineOptions(argc, argv, "Express_jit");

    uint32_t IFcounter = InputFile.size() / 2;


    #ifndef Print_M
    llvm::outs() << DataSize;
    llvm::outs() << ExprORFilename;
    for (unsigned int i = 0; i < IFcounter; i++)
        llvm::outs() << "\nIFile\n\tFile path:" << InputFile[i * 2] << "\n\tFile type:" << InputFile[i * 2 + 1] << "\n";
    llvm::outs() << "\nOFile\n\tFile path:" << OutputFile[0] << "\n";
    #endif

    FilesInfo* FInfo = new FilesInfo(IFcounter, DataSize);

    for (unsigned int i = 0; i < IFcounter; i++)
        FInfo->setIfileParam(i, InputFile[i * 2], InputFile[i * 2 + 1]);
    FInfo->setOfileParam(OutputFile[0]);
    FInfo->FileExprSource = FileExprSource;
    FInfo->ExprORFilename = ExprORFilename;

    return FInfo;
}
