
#include "table.h"
#include "variable.h"
#include "Body.h"
#include "llvmHdrs.h"


using namespace llvm;


//Table::Block section 
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
    const size_t max_line_length=90;
    string out="\tlevel: " + std::to_string(level) + "\n";
    for (auto i : unitList) {
        std::string txt       = i->printUint() + ";" + (i->isBuffered() ? " store" : "");
        std::string txtShifts = std::to_string(i->getLeftBufferLen()) + " : " + std::to_string(i->getRightBufferLen());
        std::string txtSkip   = std::string(max_line_length - ((txt.length() > max_line_length) ? 0 : txt.length()), ' ');

        out+="\t\t" + txt+ txtSkip+ txtShifts+ "\n";
    }
    return out;
}


void Block::generateIR(IRGenerator &builder) {

    LLVMContext & context = builder.getContext();
    BasicBlock* bbLoad= BasicBlock::Create(context, "load_values", builder.getCurrentFunction());
    BasicBlock* bbCalc= BasicBlock::Create(context, "calc_values", builder.getCurrentFunction());
    builder.SetLoadInsertPoint(bbLoad);
    builder.SetCalcInsertPoint(bbCalc);
    for (auto i : unitList)
        i->setupIR(builder);
}


//TableColumn:: column section 
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

    llvm:outs() << out;
    return out;
}


void    Table::declareBuiltInFunctions(BuiltInFuncMap &UBIFMap, Type * Ty) {
    BIF2LLVMmap ={
{opCodeEn::FPOW, Intrinsic::pow},
{opCodeEn::COS, Intrinsic::cos},
{opCodeEn::SIN, Intrinsic::sin},
{opCodeEn::EXP, Intrinsic::exp},
{opCodeEn::LOG, Intrinsic::log},
{opCodeEn::LOG2, Intrinsic::log2},
{opCodeEn::LOG10,Intrinsic::log10} };

}

llvm::Function*  Table::getBIFunc( BuiltInFuncMap &UBIFMap, opCodeEn op, Type * Ty) {
    auto pos=floatBIFuncMap.find(op);
    if (pos != floatBIFuncMap.end()) {
        return pos->second;
    }
    else {
        Intrinsic::ID code =BIF2LLVMmap[op];
        Function* f=Intrinsic::getDeclaration(M, code, Ty);
        (UBIFMap)[op] = f;
        return f;
    }
}


void    Table::declareFunctions() {
    LLVMContext & context = M->getContext();
    fTy = Type::getFloatTy(context);
    dTy = Type::getDoubleTy(context);
    declareBuiltInFunctions(floatBIFuncMap, fTy);
}

llvm::Function * Table::getFloatBIFunc(opCodeEn op) { 
    //return floatBIFuncMap[op];
    return getBIFunc (floatBIFuncMap, op, fTy);
}

llvm::Function * Table::getDoubleBIFunc(opCodeEn op) {
    //return doubleBIFuncMap[op]; 
    return getBIFunc (doubleBIFuncMap, op, dTy);
}


template <typename T>
T convolveTemplate(T * i, T * j)
{
    return (T)100;
}



//convolveTemplate<int64_t>
int64_t convolveI64(int64_t * i, int64_t * j) {
    return 100;
}

int32_t convolveI32(int32_t * i, int32_t * j) {
    return 100;
}

int16_t convolveI16(int16_t * i, int16_t * j) {
    return 100;
}

double convolveDouble(double * i,double * j) {
    return 100.0;
}

float convolveFloat(float * i, float * j) {
    return 100.0;
}

Value * genConvolve(IRGenerator & builder,llvm::Type* type , uintptr_t addr) {

    Value *convolveFunction = builder.CreateIntToPtr(ConstantInt::get(builder.getInt64Ty(),
        uintptr_t(addr)),
        PointerType::getUnqual(
            FunctionType::get(builder.getInt64Ty(),
                { type->getPointerTo(), type->getPointerTo() },
                false
            )
        ));
    //convolveFunction->setName("convolve");
    return  convolveFunction;
}


void    Table::generateIR() {
    LLVMContext & context = M->getContext();
    IRGenerator builder(context, this);


    currentFunction = Function::Create(
        FunctionType::get(Type::getInt32Ty(context), {Type::getInt64PtrTy(context)->getPointerTo()}, false),
        Function::ExternalLinkage,
        "main", 
        M
    );



    builder.SetDeclareConvolve(builder.getInt16Ty(), uintptr_t(convolveTemplate<int16_t>));
    builder.SetDeclareConvolve(builder.getInt32Ty(), uintptr_t(convolveTemplate<int32_t>));
    builder.SetDeclareConvolve(builder.getInt64Ty(),  uintptr_t(convolveTemplate<int64_t>));
    builder.SetDeclareConvolve(builder.getDoubleTy(), uintptr_t(convolveTemplate<double>));
    builder.SetDeclareConvolve(builder.getFloatTy(),  uintptr_t(convolveTemplate<float>));



    builder.SetCurrentFunction(currentFunction);
    BasicBlock* bb= BasicBlock::Create(context, "init_block", currentFunction);
    builder.SetInitInsertPoint(bb);

    for (auto i : constList)
       i->setupIR(builder);

    //for (auto i : smallArrayList)
    //   i->setupIR(builder);

    for (auto i : columnList) 
       i->generateIR(builder);

    llvm::outs() << "\n\n---------\nWe just constructed this LLVM module:\n\n---------\n" << *M;
}

void  Variable::setupIR(IRGenerator & builder){
    IRValue=builder.CreateConst(binaryValue, type, "");
}

void  Operation::setupIR(IRGenerator & builder){

#define OP(i) (operand[(i)]->getAssignedVal(true)->getIRValue() )
#define OP_PTR(i) (operand[(i)]->getAssignedVal(true)->getIRValuePtr() )
    if (isArithetic(opCode)) {
        IRValue =builder.CreateArithmetic(OP(0), OP(1), opCode, getUniqueName());
    }
    else if (isInv(opCode)) {
        IRValue =builder.CreateInv(OP(0), opCode, getUniqueName());
    }
    else if (isTypeConv(opCode)) {
        IRValue =builder.CreateTypeConv(OP(0), opCode, type, getUniqueName());
    }
    else if (isBuiltInFunc(opCode)) {
        IRValue =builder.CreateBuiltInFunc(OP(0), opCode, getUniqueName());
    }
    else if (isSelect(opCode)) {
         
    }
    else if (isConvolve(opCode)) {
        IRValue =builder.CreateConvolve(OP_PTR(0), OP_PTR(0), getUniqueName());
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
        IRValueBufferPtr=builder.CreatePositionalAlloca(IRValue->getType(), 0, "alloca_");
        IRValueBuffer=builder.CreatePositionalLoad(IRValueBufferPtr, "buffer_");
    }
#undef OP_PTR
#undef OP
}

void  Line::setupIR(IRGenerator & builder) {
    if (!is_arg) {
        IRValue=getAssignedVal()->getIRValue();
        if (isBuffered ()) {// it may be unsafe and dangerous 
            std::string name ="line_alloc_" + getAssignedVal(true)->getUniqueName();
            IRValueBufferPtr=builder.CreatePositionalAlloca(IRValue->getType(), 0, name);
            IRValueBuffer=builder.CreatePositionalLoad(IRValueBufferPtr, "line_buffer_");
        }
    }
    else {
        setBuffered();
        Type * volatile t= builder.getLLVMType(type);
        IRValueBufferPtr =builder.CreatePositionalAlloca(t,0, "arg_alloca_");
        IRValue=builder.CreatePositionalLoad(IRValueBufferPtr, "arg_buffer_");
        IRValueBuffer=IRValue;
    }
}

void  Call::setupIR(IRGenerator & builder) {
    /*
    IRValue =body->getRet()[0]->getAssignedVal(true)->getIRValue();
    if (isBuffered ()) {// it may be unsafe and dangerous 

        IRValueBufferPtr = builder.CreatePositionalAlloca(IRValue->getType(), 0, "call_alloca_");
        IRValueBuffer=builder.CreatePositionalLoad(IRValueBufferPtr, "call_buffer_");
    }
    */
}
