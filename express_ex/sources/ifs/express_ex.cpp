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


//using namespace llvm;
extern bool  g_ansiEscapeCodes;

#define REMOVE_MEMBER(X)    if (X != nullptr) {\
                                delete X;\
                                X=nullptr;\
                            }\


extern std::string g_error_str;
Express_ex::~Express_ex()
{
    REMOVE_MEMBER(table_);
    REMOVE_MEMBER(body_);
    REMOVE_MEMBER(kex_parser_);
}

bool Express_ex::parseText(std::string str, bool is_file_name , std::map<std::string, bool/*is_file_name*/> lib_str_map ) {

    //
    bool status=false;


    REMOVE_MEMBER(table_);
    REMOVE_MEMBER(body_);
    //REMOVE_MEMBER(body_prototype_);
    REMOVE_MEMBER(kex_parser_);


    try {
        kex_parser_ =new KEXParser(str, is_file_name, lib_str_map);

        body_prototype_ = kex_parser_->getActivBody();
        body_prototype_->getParameterLinkNames();
        status=true;
    }
    catch (size_t) {
        error_str_ = g_error_str;
    }

    return status;
}


bool Express_ex::setParameters(const std::map<std::string ,SyncParameter*> &parameters_map)
{
    bool status=false;


    stack<Value*> args;
    for (auto p : parameters_map)
        if (p.second != nullptr)
            args.push(new Line(p.first, p.second));
        else {
            error_str_="null_ptr in parameters_map";
            return false;
        }

    try {

        REMOVE_MEMBER(table_);
        REMOVE_MEMBER(body_);

        body_ = body_prototype_->genBodyByPrototype(args, false);
        body_->symplyfy();

        body_->getParameterLinkNames(true);


        table_   = new Table();
        TableGenContext context = TableGenContext(table_);

        body_->genTable(&context);

        int index=0;
        for (auto i : body_->getOutputParameterList())
            i->setName("out_" + std::to_string(index++));

        status=true;
    }
    catch (size_t) {
        error_str_ = g_error_str;
    }

    return status;
}

std::map<std::string, std::string> Express_ex::getParameterLinkNamesMap(bool hide_unused ) {
    if (body_prototype_)
        return body_prototype_->getParameterLinkNames(hide_unused);
    else
        return std::map<std::string, std::string>();
}

std::vector<ParameterIfs*> Express_ex::getOutputParameterVector()
{
    if (body_)
        return body_->getOutputParameterList();
    else
        return std::vector<ParameterIfs*>();
}

bool Express_ex::genJit()
{
    bool status=false;
    try {
        table_->calculateBufferLength();
        table_->llvmInit();
        table_->generateIR();
        table_->runOptimization();
        status = true;
    }
    catch (size_t) {
        error_str_ = g_error_str;
    }

    return status;
}

bool Express_ex::run()
{
    table_->run();
    return true;
}
