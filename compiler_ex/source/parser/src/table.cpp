
#include "table.h"
#include "variable.h"
#include "Body.h"
#include "llvmHdrs.h"


using namespace llvm;


//Table::Block section 
//
//
//
//
//
Block::Block (Variable* var) {
    level=var->getLevel();
    setUint(var);
}

void    Block::setUint(Variable * var){
    unitList.push(var);
}

string  Block::print() {
    string out="\tlevel: " + std::to_string(level) + "\n";
    for (auto i : unitList) {
        out+="\t\t" + i->printUint() + ";" + (i->isBuffered()?" store\n" :"\n");
    }
    return out;
}


void Block::generateIR(IRGenerator &builder) {

    LLVMContext & context = builder.getContext();

    BasicBlock* bb= BasicBlock::Create(context, "calc_values");
    //builder.CreateAlloca(getType(l.ret, Context), 0, "buffer");
    builder.SetInsertPoint(bb);
    for (auto i : unitList)
        i->setupIR(builder);
}

//TableColumn:: column section 
//
//
//
//
//
TableColumn::TableColumn (Variable* var) {
    length =var->getLength();
    setUint(var);
}

void    TableColumn::setUint(Variable * var){
    auto  varlevel= var->getLevel();
    for (auto i : blockList) 
        if (i->getLevel() == varlevel) {
            i->setUint(var);
            return;
        }
    blockList.push(new Block(var));
}

string  TableColumn::print(){
    string out="length: " + std::to_string(length) + "\n";
    for (auto i : blockList) {
        out+= i->print();
    }
    return out;
}

void    TableColumn::generateIR(IRGenerator &builder) {
    for (auto i : blockList)
        i->generateIR(builder);
}


//Table:: Table section 
//
//
//
//
//
void    Table::setUint(Variable * var)
{
    auto  varLength= var->getLength();
    if (isConst(var)) {
        constList.push(var);
        return;
    }
    if (isSmallArr(var)) {
        smallArrayList.push(var);
        return;
    }
    for (auto i : columnList)
        if (i->getLength() == varLength) {
            i->setUint(var);
            return;
        }
    columnList.push(new TableColumn(var));
}


string  Table::print() {
    string out="";

    out+="constant defenition:\n";
    for (auto i : constList) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="small array defenition:\n";
    for (auto i : smallArrayList) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="table:\n";
    for (auto i : columnList) {
        out+= i->print();
    }

    std::cout << out;
    return out;
}


void    declareUnaryBuiltInFunctions(Module * M, BuiltInFuncMap *UBIFMap, Type * Ty) {
    (*UBIFMap)[opCodeEn::POW] =      getDeclaration(M, Intrinsic::pow, Ty);
    (*UBIFMap)[opCodeEn::COS] =      getDeclaration(M, Intrinsic::cos, Ty);
    (*UBIFMap)[opCodeEn::SUB] =      getDeclaration(M, Intrinsic::sin, Ty);
    (*UBIFMap)[opCodeEn::EXP] =      getDeclaration(M, Intrinsic::exp, Ty);
    (*UBIFMap)[opCodeEn::LOG] =      getDeclaration(M, Intrinsic::log, Ty);
    (*UBIFMap)[opCodeEn::LOG2] =     getDeclaration(M, Intrinsic::log2, Ty);
    (*UBIFMap)[opCodeEn::LOG10] =    getDeclaration(M, Intrinsic::log10, Ty);
}


void    Table::declareFunctions() {

    LLVMContext & context = M->getContext();
    fTy = Type::getFloatTy(context);
    dTy = Type::getDoubleTy(context);
    //std::cout << "fTy = Type::getFloatTy(context);  " << (dTy== Type::getDoubleTy(context) )<<"\n" ;
    declareUnaryBuiltInFunctions(M, &floatBIFuncMap, fTy);
    declareUnaryBuiltInFunctions(M, &doubleBIFuncMap, dTy);

}

  
void    Table::generateIR(){
    LLVMContext & context = M->getContext();
    IRGenerator builder(context,this);

     currentFunction = Function::Create(
        FunctionType::get(Type::getInt32Ty(context), {Type::getInt32PtrTy(context)->getPointerTo()}, false),
        Function::ExternalLinkage,
        "main", 
        M
     );

    BasicBlock* bb= BasicBlock::Create(context, "calc_block", currentFunction);
    builder.SetInsertPoint(bb);

    for (auto i : constList)
       i->setupIR(builder);

    //for (auto i : smallArrayList)
    //   i->setupIR(builder);

    for (auto i : columnList) 
       i->generateIR(builder);
    llvm::outs() << "We just constructed this LLVM module:\n";
    llvm::outs() << "We just constructed this LLVM module:\n\n---------\n" << *M;
}

void  Variable::setupIR(IRGenerator & builder){
    IRValue=builder.CreateConst(binaryValue, type, "");
}

void  Operation::setupIR(IRGenerator & builder){
    is_visited = false;
    std::string txtOperation = "";
    std::string uName = getUniqueName();
#define OP(i) (operand[(i)]->getIRValue() )

    if (isArithetic(opCode)) {
        IRValue =builder.CreateArithmetic(OP(0), OP(1), opCode, printUint());
    }
    else if (isInv(opCode)) {
        IRValue =builder.CreateInv(OP(0), opCode, printUint());
    }
    else if (isTypeConv(opCode)) {
        IRValue =builder.CreateTypeConv(OP(0), opCode, type, printUint());
    }
    else if (isBuiltInFunc(opCode)) {
        IRValue =builder.CreateBuiltInFunc(OP(0), opCode, printUint());
    }
    else if (isSelect(opCode)) {

    }
    else if (isConvolve(opCode)) {

    }
    else if (isSlice(opCode)) {

    }
    else if (isStoreToBuffer(opCode)) {
        print_error("visitExitTxt StoreToBuffer unknown command .");
    }
    else {
        print_error("visitExitTxt unknown command .");
    }

    if (isBuffered ()) {// it may be unsafe and dangerous 
        IRValueBuffer=builder.CreateAlloca(IRValue->getType(), 0, "buffer");
    }
#undef OP
}

void  Line::setupIR(IRGenerator & builder) {
    if (!is_arg) {
        IRValue=getAssignedVal()->getIRValue();
        if (isBuffered ()) {// it may be unsafe and dangerous 
            std::string name ="buffer" + getAssignedVal()->getUniqueName();
            IRValueBuffer=builder.CreateAlloca(IRValue->getType(), 0, name);
        }
    }
    else {
        setBuffered();
        IRValueBuffer=builder.CreateAlloca(IRValue->getType(), 0, "buffer");
    }


}

void  Call::setupIR(IRGenerator & builder) {
    IRValue =body->getRet()[0]->getIRValue();
    if (isBuffered ()) {// it may be unsafe and dangerous 
        IRValueBuffer=builder.CreateAlloca(IRValue->getType(), 0, "buffer");
    }
}
