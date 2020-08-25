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
extern bool  g_ansiEscapeCodes;



int sub_main(int argc, const char* argv[]) {
    std::string error_str_ = "";

    bool optimizationEnable = true;

    ParametersDB parameterDB({ "inputDataBaseFile" });
    KEXParser    parser("inputFile");


    try {
        parser.walk();
        Body* body_brototype = parser.getActivBody();
        body_brototype->getParameterLinkNames();


        stack<Variable*> args;

        auto p =new SyncParameter();
        if (p != nullptr)
            args.push(new Line("as", p));
        else
            error_str_="can not find parameter ";


        Body* body = body_brototype->genBodyByPrototype(args, false);
        body->symplyfy();

        body->getParameterLinkNames(true);
        body->reduce(); //atavism

        jit_init();

        Table*          table   = new Table();
        TableGenContext context = TableGenContext(table);

        body->genTable(&context);

        int index=0;
        for (auto i : body->getOutputParameterList())
            i->setName("out_" + std::to_string(index++));

        table->calculateBufferLength();
        table->llvmInit();
        table->generateIR();

        if (optimizationEnable)
            table->runOptimization();

        //table->printllvmIr();
        //run jit

        table->run();
    }
    catch (size_t) {

    }


    return 0;
}

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

bool Express_ex::parseText(std::string str, bool is_file_name ) {

    //
    bool status=false;


    REMOVE_MEMBER(table_);
    REMOVE_MEMBER(body_);
    //REMOVE_MEMBER(body_prototype_);
    REMOVE_MEMBER(kex_parser_);


    kex_parser_ =new KEXParser(str, is_file_name);
    try {
        kex_parser_->walk();
        Body* body_brototype_ = kex_parser_->getActivBody();
        body_brototype_->getParameterLinkNames();
        status=true;
    }
    catch (size_t) {
        error_str_ = g_error_str;
    }

    return status;
}


bool Express_ex::setParameters(std::vector<SyncParameter*> parameters_vector)
{
    bool status=false;


    stack<Variable*> args;
    for (auto p : parameters_vector)
        if (p != nullptr)
            args.push(new Line("as", p));
        else {
            error_str_="null_ptr in parameters_vector";
            return false;
        }

    try {

        REMOVE_MEMBER(table_);
        REMOVE_MEMBER(body_);

        body_ = body_prototype_->genBodyByPrototype(args, false);
        body_->symplyfy();

        body_->getParameterLinkNames(true);
        body_->reduce(); //atavism

        Table*          table   = new Table();
        TableGenContext context = TableGenContext(table);

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
