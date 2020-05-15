#ifndef CommandLineArgParser_H
#define CommandLineArgParser_H

#include "llvm/Support/CommandLine.h"
#include "IO.h"

extern cl::OptionCategory MainCategory;
FilesInfo* CommandLineArgParser(int argc, char** argv);

#endif