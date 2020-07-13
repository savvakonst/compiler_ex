
#include "table.h"
#include "variable.h"
#include "Body.h"
#include "llvmHdrs.h"
#include <sstream>


using namespace llvm;
void print_IR_error(std::string content);



SubBlock::SubBlock(Variable * var)
{
    leftLength=var->getLeftBufferLen();
    rightLength=var->getRightBufferLen();
    length=var->getLength();
    setUint(var);
}

void SubBlock::setUint(Variable * var)
{
    unitList.push(var);
}

string SubBlock::print()
{
    const size_t max_line_length=90;
    string out=std::string(4, ' ')+"L,R: " + std::to_string(leftLength) + "," + std::to_string(rightLength) + "\n";
    for (auto i : unitList) {
        std::string txt       = i->printUint() + ";" + (i->isBuffered() ? " store" : "");
        std::string txtShifts = std::to_string(i->getLeftBufferLen()) + " : " + std::to_string(i->getRightBufferLen());
        std::string txtSkip   = std::string(max_line_length - ((txt.length() > max_line_length) ? 0 : txt.length()), ' ');

        out+=std::string(6, ' ') + txt + txtSkip + txtShifts + "\n";
    }
    return out;
}

bool SubBlock::generateIR(IRGenerator & builder, CycleStageEn type, std::string basicBlockPrefix, std::string basicBlockPostfix)
{
    LLVMContext & context = builder.getContext();

    std::string  levelTxt=basicBlockPostfix;

    BasicBlock* bbLoad  = BasicBlock::Create(context, "load_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
    BasicBlock* bbCalc  = BasicBlock::Create(context, "calc_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
    BasicBlock* bbStore = BasicBlock::Create(context, "store_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());

    BasicBlock* bbLastStore=builder.getStoreBlock();

    if (NULL != bbLastStore)
    {
        builder.CreateStartBRs();
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbLoad);
    }

    builder.ClearInitializedVariablesList();

    builder.SetLoadInsertPoint(bbLoad);
    builder.SetCalcInsertPoint(bbCalc);
    builder.SetStoreInsertPoint(bbStore);
        
    builder.SetCalcInsertPoint();


    Value* alloc =builder.CreatePositionalOffset(basicBlockPrefix + levelTxt, -leftLength);
    builder.SetStoreInsertPoint();
    Value* nextOffset =builder.CreateAdd(builder.getCurrentOffsetValue(), builder.getInt64(1));
    builder.CreateStore(nextOffset, alloc);
    builder.SetCurrentCMPRes(
        builder.CreateICmpULT(
            nextOffset,
            builder.getInt64(bufferLength + rightLength)));
    builder.SetCalcInsertPoint();

    for (auto i : unitList)
        i->setupIR(builder);


    return true;
}

//Table::Block section 
//
//
//
Block::Block(Variable* var) {
    level=var->getLevel();
    length=var->getLength();
    setUint(var);
}

void Block::setUint(Variable * var){
    unitList.push(var);
    setUintToSubtable(var);
}

void Block::setUintToSubtable(Variable * var)
{
    auto  leftBufferLen= var->getLeftBufferLen();
    auto  rightBufferLen= var->getRightBufferLen();
     
    for (auto i : subBlockList)
        if ((i->getLeftLength() == leftBufferLen) && (i->getRightLength() == rightBufferLen)) {
            i->setUint(var);
            return;
        }

    subBlockList.push(new SubBlock(var));
}

string  Block::print() {
    const size_t max_line_length=90;
    string out=std::string(2, ' ')+"level: " + std::to_string(level) + "\n";
    for (auto i : subBlockList) {
        out+=i->print();
    }
    /*
    for (auto i : unitList) {
        std::string txt       = i->printUint() + ";" + (i->isBuffered() ? " store" : "");
        std::string txtShifts = std::to_string(i->getLeftBufferLen()) + " : " + std::to_string(i->getRightBufferLen());
        std::string txtSkip   = std::string(max_line_length - ((txt.length() > max_line_length) ? 0 : txt.length()), ' ');

        out+="\t\t" + txt+ txtSkip+ txtShifts+ "\n";
    }
   */
    return out;
}


bool Block::generateIR(IRGenerator &builder, CycleStageEn type, std::string basicBlockPrefix)
{
    LLVMContext & context = builder.getContext();

    std::string  levelTxt=std::to_string(level);

    if ( (type == CycleStageEn::midle) || (type == CycleStageEn::end) ){
        
        if ((type == CycleStageEn::end) & (0 == ( length % bufferLength )))
            return false;


        BasicBlock* bbIntermediate  = BasicBlock::Create(context, "intermediate_" + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
        BasicBlock* bbLoad  = BasicBlock::Create(context, "load_" + basicBlockPrefix+ levelTxt, builder.getCurrentFunction());
        BasicBlock* bbCalc  = BasicBlock::Create(context, "calc_" + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
        BasicBlock* bbStore = BasicBlock::Create(context, "store_" + basicBlockPrefix +  levelTxt, builder.getCurrentFunction());


        BasicBlock* bbLastStore=builder.getStoreBlock();

        if (NULL != bbLastStore)
        {
            builder.CreateMidleBRs();
            builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbIntermediate);
  
        }
        else {
            builder.SetLoopEnterInsertPoint();
            builder.CreateBr(bbIntermediate);
        }

        builder.ClearInitializedVariablesList();

        builder.SetIntermediateInsertPoint(bbIntermediate);
        builder.SetOffsetToZero();

        builder.SetLoadInsertPoint(bbLoad);
        builder.SetCalcInsertPoint(bbCalc);
        builder.SetStoreInsertPoint(bbStore);


        
        builder.SetLoadInsertPoint();

        Value* offsetAlloc = builder.getCurrentOffsetValueAlloca();
        Value* offset= builder.CreateLoadOffset();

        builder.SetStoreInsertPoint();

        Value* nextOffset =builder.CreateAdd(offset, builder.getInt64(1));
        builder.CreateStore(nextOffset, offsetAlloc);

        int len=(type == CycleStageEn::midle) ? bufferLength : length % bufferLength ;
        builder.SetCurrentCMPRes(
            builder.CreateICmpULT(
                nextOffset,
                builder.getInt64(len)));
        builder.SetCalcInsertPoint();


        for (auto i : unitList)
            i->setupIR(builder);

    }else if(type == CycleStageEn::start){
        std::string  levelTxt=std::to_string(level);
        size_t sub_level=0;
        for (auto i : subBlockList) {
            i->generateIR(builder, type, basicBlockPrefix, levelTxt+"_"+std::to_string(sub_level));
            sub_level++;
        }

    }
    return true;
}


//TableColumn:: column section 
//
//
TableColumn::TableColumn (Variable* var) {
    length =var->getLength();
    setUint(var);
}

void    TableColumn::setUint(Variable * var){
    auto  varlevel= var->getLevel();
    for (auto i : blockList) 
        if (i->getLevel() == varlevel){
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

bool    TableColumn::generateIR(IRGenerator &builder, CycleStageEn type, std::string basicBlockPrefix) {
    std::stringstream hex();
    bool res=false;
    for (auto i : blockList) {
        std::stringstream ss;
        ss<< std::hex <<length;

        res|=i->generateIR(builder, type, basicBlockPrefix + "block_" +"0x" +ss.str() + "_level_");
    }
    return res;
}



template <typename T>
T convolveTemplate(T * i, T * j){
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
double convolveDouble(double * i, double * j) {
    return 100.0;
}
float convolveFloat(float * i, float * j) {
    return 100.0;
}

Value * genConvolve(IRGenerator & builder, llvm::Type* type, uintptr_t addr) {

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


//Table:: Table section 
//
//
//
void    Table::setUint(Variable * var){

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

llvm::Function * Table::getFloatBIFunc(opCodeEn op) { 
    //return floatBIFuncMap[op];
    return getBIFunc (floatBIFuncMap, op, fTy);
}

llvm::Function * Table::getDoubleBIFunc(opCodeEn op) {
    //return doubleBIFuncMap[op]; 
    return getBIFunc (doubleBIFuncMap, op, dTy);
}

llvm::Function*  Table::getBIFunc(BuiltInFuncMap &UBIFMap, opCodeEn op, Type * Ty) {
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

void Table::calculateBufferLength(std::string basicBlockPrefix){

    int maxLength=0;
    int minLength=columnList[0]->getLength();

    typedef struct {
        int maxLength;
        int minLength;
        int iterations;
        stack<TableColumn *> columnList;
    } group;

    std::vector<group> groupList;

    for (auto  i : columnList){
        uint64_t length=i->getLength();
        bool exist=false;
       
        for (auto &j : groupList) {
            auto remainder =(j.maxLength > length) ? j.maxLength % length : length % j.maxLength;
            if (remainder == 0) {
                exist=true;
                j.maxLength=length > j.maxLength ? length : j.maxLength;
                j.minLength=length < j.minLength ? length : j.minLength;
                j.columnList.push_back(i);
                break;
            }
        }


        if (!exist) {
            group g={ length ,length };
            g.columnList.push_back(i);
            groupList.push_back(g);
        }
    }

    for (auto &j : groupList) {

        int subMaxBufferLength = minBufferLength * j.maxLength / j.minLength;
        
        if (subMaxBufferLength > maxBufferLength) {
            llvm::outs() << "maxBufferLength = subMaxBufferLength;\n";
            maxBufferLength = subMaxBufferLength;
        }
        
        iterations =  j.maxLength / maxBufferLength;

        j.iterations =  j.maxLength / maxBufferLength;

        for (auto i : j.columnList){
            i->setBufferLength(
                maxBufferLength / (j.maxLength / i->getLength())
            );
        }
    }
    if (groupList.size() !=1)
        print_IR_error("non multiple large array lengths is not supported yet\n");

    //iterations=groupList[0].columnList[0]->getLength()/ groupList[0].columnList[0]->get();
}




bool    Table::generateIR(std::string basicBlockPrefix) {
    LLVMContext & context = M->getContext();
    IRGenerator builder(context, this);

    currentFunction = Function::Create(
        FunctionType::get(Type::getInt32Ty(context), {Type::getInt64PtrTy(context)->getPointerTo()}, false),
        Function::ExternalLinkage,
        "main", 
        M
    );

    builder.SetDeclareConvolve(builder.getInt16Ty(),  uintptr_t(convolveTemplate<int16_t>));
    builder.SetDeclareConvolve(builder.getInt32Ty(),  uintptr_t(convolveTemplate<int32_t>));
    builder.SetDeclareConvolve(builder.getInt64Ty(),  uintptr_t(convolveTemplate<int64_t>));
    builder.SetDeclareConvolve(builder.getDoubleTy(), uintptr_t(convolveTemplate<double>));
    builder.SetDeclareConvolve(builder.getFloatTy(),  uintptr_t(convolveTemplate<float>));

    for (auto i : constList)
       i->setupIR(builder);

    for (auto i : smallArrayList)
        //i.
       //i->setupIR(builder);




    //the main "For loop"
    // creating jump commands for init cycle
    builder.SetCurrentFunction(currentFunction);
    BasicBlock* bb= BasicBlock::Create(context, "init_block", currentFunction);
    builder.SetInitInsertPoint(bb);

    for (auto i : columnList) 
       i->generateIR(builder,CycleStageEn::start);

    // creating jump commands for init cycle
    builder.CreateStartBRs();
    //cond jump to Main Cycle Enter
    BasicBlock* bbLoopEnter = BasicBlock::Create(context, "loop_enter_block", builder.getCurrentFunction());
    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbLoopEnter);
    builder.SetLoopEnterInsertPoint(bbLoopEnter);
    builder.DropBaseInsertPoint();

    Value* globIndexAlloca=builder.CreatePositionalOffsetAlloca("glob_index_alloca",0);
    builder.CreatePositionalOffsetAlloca("common_offset_alloca", 0);


    builder.SetLoopEnterInsertPoint(bbLoopEnter);
    Value* globIndex=builder.CreateLoad(globIndexAlloca, "glob_index");

    for (auto i : columnList)
        i->generateIR(builder, CycleStageEn::midle);
   
    builder.CreateMidleBRs();
    BasicBlock* bbCycleExit = BasicBlock::Create(context, "cycle_exit_block", builder.getCurrentFunction());
    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbCycleExit);
    builder.SetCycleExitInsertPoint(bbCycleExit);

    //global index increment, and index comparsion with max iteration number
    //increasing the global index and comparing the index with the max iteration number
    Value* nextGlobIndex =builder.CreateAdd(globIndex, builder.getInt64(1));
    builder.CreateStore(nextGlobIndex, globIndexAlloca);
    builder.SetCurrentCMPRes(
        builder.CreateICmpULT(
            nextGlobIndex,
            builder.getInt64( (iterations-1) )));


    BasicBlock* bbTerminalLoopEnter = BasicBlock::Create(context, "terminal_loop_enter", builder.getCurrentFunction());
    builder.CreateCondBr(builder.getCurrentCMPRes(), bbLoopEnter, bbTerminalLoopEnter);
    builder.SetLoopEnterInsertPoint(bbTerminalLoopEnter);
    builder.DropBaseInsertPoint();
    
    bool  isNotIdle = false;
    for (auto i : columnList)
        isNotIdle |= i->generateIR(builder, CycleStageEn::end);
   
    if (isNotIdle) {
        builder.CreateMidleBRs();
        BasicBlock* bbExit = BasicBlock::Create(context, "exit_block", builder.getCurrentFunction());
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbExit);
        builder.SetExitInsertPoint(bbExit);
    }
   
    builder.CreateRet(builder.getInt32(1));

    //create jump from init block to the next
    builder.SetInitInsertPoint();
    builder.CreateBr(builder.getBlock(1));
}



// Implementation of Value, Operation, Line and Call  members, 
// which provide llvm IR generation.
//
//
Value* Variable::getIRValue(IRGenerator & builder, int64_t parentLevel) {
    Value * ret=NULL;
    if (isBuffered() & (parentLevel != level) ) {
        if (!builder.CheckExistence(IRBufferRefPtr)) {
            auto valPtr=builder.CreatePositionalInBoundsGEP(IRBufferRefPtr, builder.getCurrentOffsetValue(), "offset_incr");
            IRLoadedBufferValue=builder.CreatePositionalLoad(valPtr, "buffer_");
            builder.AddInitializedVariable(IRBufferRefPtr);
        }
        ret=IRLoadedBufferValue;
    }
    else ret=IRValue;

    if (ret == NULL) print_IR_error("IRValue - is NULL :" + getUniqueName());
    return ret;
}

Value* Variable::getIRValuePtr(IRGenerator & builder, int64_t parentLevel) {
    auto ret =isBuffered () ? IRBufferRefPtr : NULL;
    if (ret == NULL) print_IR_error("getIRValuePtr - is NULL :" + getUniqueName());
    return ret;
}

void  Variable::setupIR(IRGenerator & builder) {
    IRValue=builder.CreateConst(binaryValue, type, "");
}

void  Operation::setupIR(IRGenerator & builder){

#define OP(i)     (operand[(i)]->getAssignedVal(true)->getIRValue(builder,level) )
#define OP_PTR(i) (operand[(i)]->getAssignedVal(true)->getIRValuePtr(builder,level) )

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
        print_IR_error("visitExitTxt StoreToBuffer unknown command .");
    }
    else {
        print_IR_error("visitExitTxt unknown command .");
    }


    if (isBuffered()|isReturned()) {
        if (!is_initialized) {
            IRBufferRefPtr=builder.CreateBufferInit(type,"internal_");
            is_initialized=true;
        }
        builder.SetStoreInsertPoint();
        auto valPtr=builder.CreateInBoundsGEP(IRBufferRefPtr, builder.getCurrentOffsetValue(), "offset_incr");
        builder.CreatePositionalStore(IRValue, valPtr);
    }

#undef OP_PTR
#undef OP

}

void  Line::setupIR(IRGenerator & builder) {
    if (!is_arg) {
        //pass
    }
    else {
        //setBuffered();
        Type * volatile t= builder.getLLVMType(type);
        if (!is_initialized) {
            IRBufferRefPtr =builder.CreateBufferInit(type, "external_");
            is_initialized=true;
        }
        auto valPtr    =builder.CreatePositionalInBoundsGEP(IRBufferRefPtr, builder.getCurrentOffsetValue(), "offset_arg_incr");
        IRValue        =builder.CreatePositionalLoad(valPtr, "arg_buffer_");
    }
}

void  Call::setupIR(IRGenerator & builder){
    //pass
}
