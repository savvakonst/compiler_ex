
#include <sstream>

#include "buffer.h"
#include "body.h"
#include "table.h"
#include "llvmHdrs.h"
#include "IR_generator.h"
#include "printer.h"

using namespace llvm;


SubBlock::SubBlock(Variable * var)
{
    left_ength_=var->getLeftBufferLen();
    right_length_=var->getRightBufferLen();
    length_=var->getLength();
    setUint(var);
}

void SubBlock::setUint(Variable * var)
{
    left_ength_=maxInt(left_ength_, var->getLeftBufferLen());
    right_length_=maxInt(right_length_, var->getRightBufferLen());
    uint_list_.push(var);
}

string SubBlock::print()
{
    const size_t max_line_length=90;
    string out=std::string(4, ' ')+"L,R: " + std::to_string(left_ength_) + "," + std::to_string(right_length_) + "\n";
    for (auto i : uint_list_) {
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

    BasicBlock* bb_load  = BasicBlock::Create(context, "load_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
    BasicBlock* bb_calc  = BasicBlock::Create(context, "calc_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
    BasicBlock* bb_store = BasicBlock::Create(context, "store_"   + basicBlockPrefix + levelTxt, builder.getCurrentFunction());
    //BasicBlock* bb_terminal_op = BasicBlock::Create(context, "terminal_op_" + basicBlockPrefix + levelTxt, builder.getCurrentFunction());

    BasicBlock* bb_last_store=builder.getStoreBlock();

    if (nullptr != bb_last_store)
    {
        builder.CreateStartBRs();
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_load);
    }

    builder.ClearInitializedVariablesList();

    builder.SetLoadInsertPoint(bb_load);
    builder.SetCalcInsertPoint(bb_calc);
    builder.SetStoreInsertPoint(bb_store);
    //builder.SetTerminalOpInsertPoint(bb_terminal_op);

    builder.SetCalcInsertPoint();


    Value* alloc =builder.CreatePositionalOffset(basicBlockPrefix + levelTxt, -left_ength_ );
    //Value* alloc =builder.CreatePositionalOffset(basicBlockPrefix + level_txt, 0);
    builder.SetStoreInsertPoint();
    Value* nextOffset =builder.CreateAdd(builder.getCurrentOffsetValue(), builder.getInt64(1));
    builder.CreateStore(nextOffset, alloc);
    builder.SetCurrentCMPRes(
        builder.CreateICmpSLT(
            nextOffset,
            builder.getInt64(buffer_length_ + right_length_))); // not true
            //builder.getInt64(buffer_length_ + right_length_)));
    builder.SetCalcInsertPoint();

    for (auto i : uint_list_)
        i->setupIR(builder);

    return true;
}

//Table::Block section 
//
//
//
Block::Block(Variable* var) {
    left_ength_=var->getLeftBufferLen();
    right_length_=var->getRightBufferLen();
    level_=var->getLevel();
    length_=var->getLength();
    setUint(var);
}

void Block::setUint(Variable * var){

    left_ength_=maxInt(left_ength_, var->getLeftBufferLen());
    right_length_=maxInt(right_length_, var->getRightBufferLen());
    uint_list_.push(var);
    setUintToSubtable(var);
}

void Block::setBufferLength(uint64_t buffer_length) {
    buffer_length_=buffer_length;
    for (auto i : sub_block_list_)
        i->setBufferLength(buffer_length_);
    for (auto i : uint_list_) {
        i->setBufferLength(buffer_length_);
    }
}

void Block::setUintToSubtable(Variable * var)
{
    auto  leftBufferLen= var->getLeftBufferLen();
    auto  rightBufferLen= var->getRightBufferLen();
     
    for (auto i : sub_block_list_)
        if ((i->getLeftLength() == leftBufferLen) && (i->getRightLength() == rightBufferLen)) {
            i->setUint(var);
            return;
        }

    sub_block_list_.push(new SubBlock(var));
}

void Block::recalcLeftRightBufferLengths() {
    for (auto i : uint_list_) {
        i->setBufferLength(left_ength_, right_length_);
    }
}

string  Block::print() {
    const size_t max_line_length=90;
    string out=std::string(2, ' ')+"level: " + std::to_string(level_) + "\n";
    for (auto i : sub_block_list_) {
        out+=i->print();
    }
    /*
    for (auto i : uint_list_) {
        std::string txt       = i->printUint() + ";" + (i->isBuffered() ? " store" : "");
        std::string txtShifts = std::to_string(i->getLeftBufferLen()) + " : " + std::to_string(i->getRightBufferLen());
        std::string txtSkip   = std::string(max_line_length - ((txt.length_() > max_line_length) ? 0 : txt.length_()), ' ');

        out+="\t\t" + txt+ txtSkip+ txtShifts+ "\n";
    }
   */
    return out;
}


bool Block::generateIR(IRGenerator &builder, CycleStageEn type, std::string basicBlockPrefix)
{
    LLVMContext & context = builder.getContext();

    std::string  level_txt=std::to_string(level_);

    if ( (type == CycleStageEn::midle) || (type == CycleStageEn::end) ){
        
        if ((type == CycleStageEn::end) & (0 == ( length_ % buffer_length_ )))
            return false;


        BasicBlock* bb_intermediate  = BasicBlock::Create(context, "intermediate_" + basicBlockPrefix + level_txt, builder.getCurrentFunction());
        BasicBlock* bb_load  = BasicBlock::Create(context, "load_"  + basicBlockPrefix + level_txt, builder.getCurrentFunction());
        BasicBlock* bb_calc  = BasicBlock::Create(context, "calc_"  + basicBlockPrefix + level_txt, builder.getCurrentFunction());
        BasicBlock* bb_store = BasicBlock::Create(context, "store_" + basicBlockPrefix + level_txt, builder.getCurrentFunction());
        //BasicBlock* bb_terminal_op = BasicBlock::Create(context, "terminal_op_" + basicBlockPrefix + levelTxt, builder.getCurrentFunction());

        BasicBlock* bb_last_store=builder.getStoreBlock();

        if (nullptr != bb_last_store)
        {
            builder.CreateMidleBRs();
            builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_intermediate);
  
        }
        else {
            builder.SetLoopEnterInsertPoint();
            builder.CreateBr(bb_intermediate);
        }

        builder.ClearInitializedVariablesList();

        builder.SetIntermediateInsertPoint(bb_intermediate);
        //builder.SetOffsetToZero();
        builder.SetOffsetTo(right_length_);

        builder.SetLoadInsertPoint(bb_load);
        builder.SetCalcInsertPoint(bb_calc);
        builder.SetStoreInsertPoint(bb_store);
        
        builder.SetLoadInsertPoint();

        Value* offset_alloc = builder.getCurrentOffsetValueAlloca();
        Value* offset= builder.CreateLoadOffset();

        builder.SetStoreInsertPoint();

        Value* next_offset =builder.CreateAdd(offset, builder.getInt64(1));
        builder.CreateStore(next_offset, offset_alloc);

        int len=(type == CycleStageEn::midle) ? buffer_length_ : length_ % buffer_length_ ;
        builder.SetCurrentCMPRes(
            builder.CreateICmpSLT(
                next_offset,
                builder.getInt64(len + right_length_)));
        builder.SetCalcInsertPoint();

        for (auto i : uint_list_)
            i->setupIR(builder);

    }else if(type == CycleStageEn::start){
        std::string  levelTxt=std::to_string(level_);
        size_t sub_level=0;
        for (auto i : sub_block_list_) {
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
    length_ =var->getLength();
    setUint(var);
}

void    TableColumn::setUint(Variable * var){
    auto  varlevel= var->getLevel();
    for (auto i : blockList_) 
        if (i->getLevel() == varlevel){
            i->setUint(var);
            return;
        }
    blockList_.push(new Block(var));
}

string  TableColumn::print(){

    string out="length: " + std::to_string(length_) + "\n";
    for (auto i : blockList_) {
        out+= i->print();
    }
    return out;
}

bool    TableColumn::generateIR(IRGenerator &builder, CycleStageEn type, std::string basicBlockPrefix) {

    std::stringstream hex;
    bool res=false;
    for (auto i : blockList_) {
        std::stringstream ss;
        ss<< std::hex <<length_;

        res|=i->generateIR(builder, type, basicBlockPrefix + "block_" +"0x" +ss.str() + "_level_");
    }
    return res;
}


bool TableColumn::generateIR(IRGenerator & builder, CycleStageEn type, int64_t level, std::string basicBlockPrefix){

    std::stringstream hex;
    bool res=false;
    for (auto i : blockList_) {
        if (i->getLevel() == level) {
            std::stringstream ss;
            ss << std::hex << length_;
            res |= i->generateIR(builder, type, basicBlockPrefix + "block_" + "0x" + ss.str() + "_level_");
        }
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


std::vector <Buffer* >   *g_buffers=nullptr;


ParametersDB*  g_IO_IFS =nullptr;

bool isInvalidBuffers() {
    return 
        nullptr == g_buffers ;
}

int32_t initBuffers() {
    if (isInvalidBuffers())
        return 1;
    for (auto i : *g_buffers)
        i->init();
    return 0;
}

int32_t updateBuffer() {
    if (isInvalidBuffers()) 
        return 1;
    for (auto i : *g_buffers)
        i->update();
    return 0;
}


///Table:: Table section 
///

Table::~Table(){
    for (auto i : parameterSet_)
        delete i;

    for (auto i : column_list_)
        delete i;

    delete builder_;
}


// add variable to table_ 
void    Table::setUint(Variable * var){
    auto  varLength= var->getLength();
    if (isConst(var)) {
        const_list_.push(var);
        return;
    }
    if (isSmallArr(var)) {
        small_array_list_.push(var);
        return;
    }
    for (auto i : column_list_)
        if (i->getLength() == varLength) {
            i->setUint(var);
            return;
        }
    column_list_.push(new TableColumn(var));
}

llvm::Function * Table::getFloatBIFunc(OpCodeEn op) { 
    //return float_BI_func_map_[op];
    return getBIFunc (float_BI_func_map_, op, fTy_);
}

llvm::Function * Table::getDoubleBIFunc(OpCodeEn op) {
    //return double_BI_func_map_[op]; 
    return getBIFunc (double_BI_func_map_, op, dTy_);
}

llvm::Function*  Table::getBIFunc(BuiltInFuncMap &UBIFMap, OpCodeEn op, Type * Ty) {
    auto pos=float_BI_func_map_.find(op);
    if (pos != float_BI_func_map_.end()) {
        return pos->second;
    }
    else {
        Intrinsic::ID code =BIF2LLVMmap_[op];
        Function* f=Intrinsic::getDeclaration(M_, code, Ty);
        (UBIFMap)[op] = f;
        return f;
    }
}

void    Table::declareFunctions() {
    LLVMContext & context = M_->getContext();
    fTy_ = Type::getFloatTy(context);
    dTy_ = Type::getDoubleTy(context);
    declareBuiltInFunctions(float_BI_func_map_, fTy_);
}

void    Table::declareBuiltInFunctions(BuiltInFuncMap &UBIFMap, Type * Ty) {
    BIF2LLVMmap_ = {
        {OpCodeEn::fpow, Intrinsic::pow},
        {OpCodeEn::cos,  Intrinsic::cos},
        {OpCodeEn::sin,  Intrinsic::sin},
        {OpCodeEn::exp,  Intrinsic::exp},
        {OpCodeEn::log,  Intrinsic::log},
        {OpCodeEn::log2, Intrinsic::log2},
        {OpCodeEn::log10,Intrinsic::log10} 
    };
}


string  Table::print() {
    string out="";

    out+="constant defenition:\n";
    for (auto i : const_list_) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="small array defenition:\n";
    for (auto i : small_array_list_) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="table:\n";
    for (auto i : column_list_) {
        out+= i->print();
    }
    return out;
}

void Table::calculateBufferLength(std::string basicBlockPrefix){

    int maxLength=0;
    int minLength=column_list_[0]->getLength();

    typedef struct {
        uint64_t maxLength;
        uint64_t minLength;
        uint64_t iterations;
        stack<TableColumn *> columnList;
    } group;

    std::vector<group> groupList;

    for (auto  i : column_list_){
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

        int subMaxBufferLength = min_buffer_length_ * j.maxLength / j.minLength;
        
        if (subMaxBufferLength > max_buffer_length_) {
            llvm::outs() << "maxBufferLength = subMaxBufferLength;\n";
            max_buffer_length_ = subMaxBufferLength;
        }
        iterations_ =  j.maxLength / max_buffer_length_;
        j.iterations =  j.maxLength / max_buffer_length_;

        for (auto i : j.columnList){
            i->setBufferLength(max_buffer_length_ / (j.maxLength / i->getLength()));
        }

    }
    if (groupList.size() !=1)
        print_IR_error("non multiple large array lengths is not supported yet\n");

    //iterations=groupList[0].columnList[0]->getLength()/ groupList[0].columnList[0]->get();
}

DLL_EXPORT void  jit_init();
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

bool Table::llvmInit() {
    context_ = new LLVMContext();

    module_U_ptr_  = std::make_unique<Module>("test", *context_);
    the_FPM_      = std::make_unique<legacy::FunctionPassManager>(module_U_ptr_.get());
    M_ = module_U_ptr_.get();

    configOptimization(the_FPM_.get());
    declareFunctions();

    return true;
}


Function* CreateConvolveFunction(Module* M, TypeEn type) {
    LLVMContext & context = M->getContext();


    IRGenerator builder(context, nullptr);
    auto  currentType= builder.getLLVMType(type);

    Function* function =
        Function::Create(
            FunctionType::get(currentType, { currentType->getPointerTo(),currentType->getPointerTo(), builder.getInt64Ty(),builder.getInt64Ty() }, false),
            Function::ExternalLinkage,
            "convolveFunction", M);



    BasicBlock* bb_Entry = BasicBlock::Create(context, "entry_block", function);
    BasicBlock* bb_LoopBody = BasicBlock::Create(context, "loop_body_block", function);
    BasicBlock* bb_exit = BasicBlock::Create(context, "exit_block", function);


    auto offset = function->getArg(3);
    auto num_of_samples = function->getArg(2);
    auto small_arr_base_ptr = function->getArg(1);

    // entry_block
    builder.SetInsertPoint(bb_Entry);
    auto big_arr_base_ptr =  builder.CreateInBoundsGEP(function->getArg(0), offset);
    
    auto index_ptr
        =builder.CreateAlloca(builder.getInt64Ty(),nullptr,"index_ptr");
    auto sum_ptr
        =builder.CreateAlloca(currentType, nullptr,"sum_ptr");
    uint64_t zero = 0;
    double one = 0;
    uint64_t oneint= *((uint64_t*)&one);
    builder.CreateStore(builder.CreateConst(zero, type), sum_ptr);
    builder.CreateStore(builder.getInt64(0), index_ptr);
    builder.CreateBr(bb_LoopBody);

    // loop_body_block
    builder.SetInsertPoint(bb_LoopBody);

    auto index
        =builder.CreateLoad(index_ptr,"index");
    auto next_index
        =builder.CreateAdd(index, builder.getInt64(1),"next_index");
    auto inv_index
        =builder.CreateSub(num_of_samples, next_index,"inv_index");

    auto big_arr_ptr = builder.CreateInBoundsGEP(big_arr_base_ptr, index);
    auto small_arr_ptr = builder.CreateInBoundsGEP(small_arr_base_ptr, inv_index);

    auto a
        = builder.CreateLoad(big_arr_ptr,"a");
    auto b
        = builder.CreateLoad(small_arr_ptr,"b");
    auto sum_v
        = builder.CreateLoad(sum_ptr, "sum_v");

    llvm::Value *next_sum;
    if (isInteger(type)) {
        auto mull = builder.CreateMul(a, b,"mull");
        next_sum = builder.CreateAdd(mull, sum_v,"next_sum");
    }
    else {
        auto mull = builder.CreateFMul(a, b, "mull");
        next_sum = builder.CreateFAdd(mull, sum_v, "next_sum");
    }

    builder.CreateStore(next_sum, sum_ptr);
    builder.CreateStore(next_index, index_ptr);
    auto cond=builder.CreateICmpSLT(next_index, num_of_samples);
    builder.CreateCondBr(cond, bb_LoopBody, bb_exit);

    //exit_block
    builder.SetInsertPoint(bb_exit);

    auto return_val=builder.CreateLoad(sum_ptr);
    builder.CreateRet(return_val);


    return function;
}


Function * Table::getConvolveFunc(TypeEn type){
    llvm::Function * function = convolve_map_[type];
    if (function == nullptr)
        function=CreateConvolveFunction(M_, type);
        convolve_map_[type] = function;
    return function;
}

Function * Table::getGPUConvolveFunc(TypeEn type) {
    llvm::Function * function = convolve_map_[type];
    if (function == nullptr)
        function=CreateConvolveFunction(M_, type);
    convolve_map_[type] = function;
    return function;
}


bool Table::generateIR(std::string basicBlockPrefix) {


    LLVMContext & context = M_->getContext();
    if(builder_==nullptr)
        builder_=new IRGenerator(context, this);
    IRGenerator &builder= *builder_;

    mainFunction_ = Function::Create(
        FunctionType::get(Type::getInt32Ty(context), {Type::getInt64PtrTy(context)->getPointerTo()}, false),
        Function::ExternalLinkage,
        "main", 
        M_
    );

    Value* bufferUpdateFunction=builder.CreateIntToPtr(
        ConstantInt::get(builder.getInt64Ty(), uintptr_t(updateBuffer)),
        PointerType::getUnqual(
            FunctionType::get(builder.getInt32Ty(),
                { },
                false)));

    bufferUpdateFunction->setName("updateBuffer");

    for (auto i : const_list_)
        i->setupIR(builder);

    for (auto i : small_array_list_)
        i->calculate();

    ///the main "For loop"
    ///creating jump commands for init cycle
    builder.SetBufferUpdateFunction(bufferUpdateFunction);
    builder.SetCurrentFunction(mainFunction_);
    BasicBlock* bb= BasicBlock::Create(context, "init_block", mainFunction_);
    builder.SetInitInsertPoint(bb);

    auto max_level =getMaxLevel()+1;
    for (uint32_t j=0;j < max_level;j++ )
        for (auto i : column_list_) 
            i->generateIR(builder,CycleStageEn::start, j);

    ///creating jump commands for init cycle
    builder.CreateStartBRs();
    //cond jump to Main Cycle Enter
    
    BasicBlock* bbUpdateBuffer = BasicBlock::Create(context, "update_buffer_block", builder.getCurrentFunction());
    BasicBlock* bbLoopEnter = BasicBlock::Create(context, "loop_enter_block", builder.getCurrentFunction());

    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbUpdateBuffer);

    builder.SetInsertPoint(bbUpdateBuffer);
    builder.CreateCall(bufferUpdateFunction); //call buffer update
    builder.CreateBr(bbLoopEnter);

    builder.SetLoopEnterInsertPoint(bbLoopEnter);
    builder.DropBaseInsertPoint();

    Value* globIndexAlloca=builder.CreatePositionalOffsetAlloca("glob_index_alloca",0);
    builder.CreatePositionalOffsetAlloca("common_offset_alloca", 0);


    builder.SetLoopEnterInsertPoint(bbLoopEnter);
    Value* globIndex=builder.CreateLoad(globIndexAlloca, "glob_index");


    for (uint32_t j=0; j < max_level; j++)
        for (auto i : column_list_)
            i->generateIR(builder, CycleStageEn::midle,j);
   
    builder.CreateMidleBRs();
    BasicBlock* bbCycleExit = BasicBlock::Create(context, "cycle_exit_block", builder.getCurrentFunction());
    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbCycleExit);
    builder.SetCycleExitInsertPoint(bbCycleExit);

    ///global index increment, and index comparsion with max iteration number
    ///increasing the global index and comparing the index with the max iteration number
    Value* nextGlobIndex = builder.CreateAdd(globIndex, builder.getInt64(1));
    builder.CreateStore(nextGlobIndex, globIndexAlloca);
    builder.SetCurrentCMPRes(
        builder.CreateICmpSLT(
            nextGlobIndex,
            builder.getInt64( ( iterations_ - 1 ) )));


    BasicBlock* bbTerminalLoopEnter = BasicBlock::Create(context, "terminal_loop_enter", builder.getCurrentFunction());
    builder.CreateCall(bufferUpdateFunction); //call buffer update
    builder.CreateCondBr(builder.getCurrentCMPRes(), bbLoopEnter, bbTerminalLoopEnter);
    builder.SetLoopEnterInsertPoint(bbTerminalLoopEnter);
    builder.DropBaseInsertPoint();
    
    bool  isNotIdle = false;
    for (uint32_t j=0; j < max_level; j++)
        for (auto i : column_list_)
            isNotIdle |= i->generateIR(builder, CycleStageEn::end,j);
   
    if (isNotIdle) {
        builder.CreateMidleBRs();
        BasicBlock* bbExit = BasicBlock::Create(context, "exit_block", builder.getCurrentFunction());
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbExit);
        builder.SetExitInsertPoint(bbExit);
        builder.CreateCall(bufferUpdateFunction); //call buffer update
    }
    
    builder.CreateRet(builder.getInt32(1));
    ///create jump from init block to the next
    builder.SetInitInsertPoint();
    builder.CreateBr(builder.getBlock(1));

    //
    g_buffers = builder.getBufferList();
    
    //for (auto i : *g_buffers) 
    //    if(i)
    //        llvm::outs() << Delimiter::RED << *i;
    
    return true;
}

bool Table::runOptimization() {
    the_FPM_->run(*mainFunction_);
    for (auto i : convolve_map_)
        the_FPM_->run(*i.second);
    return true;
}

bool Table::run() {

    //auto mainF=M_->getFunction("main");
    std::string errStr;
    ExecutionEngine* EE = EngineBuilder(std::move(module_U_ptr_)) .setErrorStr(&errStr).create();
    if (!EE) {
        llvm::outs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
        return false;
    }

    if (verifyFunction(*mainFunction_, &llvm::outs())) {
        llvm::outs() << ": Error constructing main function!\n\n";
        return false;
    }

    for (auto i : convolve_map_)
        if (verifyFunction( *i.second, &llvm::outs())) {
            llvm::outs() << ": Error constructing main function!\n\n";
            return false;
        }
        

    if (verifyModule(*M_)) {
        llvm::outs() << ": Error constructing module!\n";
        return false;
    }

    EE->finalizeObject();

    Jit_Call_t Call = (Jit_Call_t)EE->getPointerToFunction(mainFunction_);


    char ** buffers_array = new char*[g_buffers->size()];
    char ** buffers_array_temp = buffers_array;
    for (auto &i : *g_buffers)
        *(buffers_array_temp++)=i->getPtr();

    //llvm::outs()<<buffers <<

    initBuffers();
    Call(buffers_array);

    //updateBuffer();

    for (auto &i : *g_buffers)
        i->close();

    delete [] buffers_array;
    return  true;
}

std::string Table::printllvmIr() {
    llvm::outs() << ExColors::GREEN << "\n\n---------We just constructed this LLVM module:--------- \n" << ExColors::RESET << *M_ << "\n\n";
    return "";
}





/// Implementation of Value, Operation, Line and Call  members, 
/// which provide llvm IR generation.
///
///
Value* Variable::getIRValue(IRGenerator & builder, int64_t parentLevel) {
    Value * ret=nullptr;
    if (isBuffered() & (parentLevel != level_) ) {
        if (!builder.CheckExistence(IRBufferBasePtr_)) {
            IRBufferPtr_=builder.CreatePositionalInBoundsGEP(IRBufferBasePtr_, builder.getCurrentOffsetValue(), "offset_incr");
            IRLoadedBufferValue_=builder.CreatePositionalLoad(IRBufferPtr_, "buffer_");
            builder.AddInitializedVariable(IRBufferBasePtr_);
        }
        ret=IRLoadedBufferValue_;
    }
    else ret=IRValue_;

    if (ret == nullptr) print_IR_error("IRValue - is nullptr :" + getUniqueName());
    return ret;
}

Value* Variable::getIRValueBasePtr(IRGenerator & builder, int64_t parentLevel) {
    auto ret =isBuffered () ? IRBufferBasePtr_ : nullptr;
    if (ret == nullptr) print_IR_error("getIRValueBasePtr - is nullptr :" + getUniqueName());
    return ret;
}

Value* Variable::getIRValuePtr(IRGenerator & builder, int64_t parentLevel) {
    auto ret =isBuffered () ? IRBufferPtr_ : nullptr;
    if (ret == nullptr) print_IR_error("getIRValuePtr - is nullptr :" + getUniqueName());
    return ret;
}

void  Variable::setupIR(IRGenerator & builder) {
    IRValue_=builder.CreateConst(binaryValue_, type_, "");
}

void  Operation::setupIR(IRGenerator & builder){

#define OP(i)     (operand[(i)]->getAssignedVal(true)->getIRValue(builder,level_) )
#define OP_PTR(i) (operand[(i)]->getAssignedVal(true)->getIRValuePtr(builder,level_) )

    if (isArithetic(opCode)) {
        IRValue_ =builder.CreateArithmetic(OP(0), OP(1), opCode, getUniqueName());
    }
    else if (isComparsion(opCode)) {
        IRValue_ =builder.CreateComparsion(OP(0), OP(1), opCode, getUniqueName());
    }
    else if (isInv(opCode)) {
        IRValue_ =builder.CreateInv(OP(0), opCode, getUniqueName());
    }
    else if (isTypeConv(opCode)) {
        IRValue_ =builder.CreateTypeConv(OP(0), opCode, type_, getUniqueName());
    }
    else if (isBuiltInFunc(opCode)) {
        IRValue_ =builder.CreateBuiltInFunc(OP(0), opCode, getUniqueName());
    }
    else if (isSelect(opCode)) {
        IRValue_ =builder.CreateSelect(OP(0), OP(1), OP(2),  getUniqueName());
    }
    else if (isConvolve(opCode)) {
        //outs()<<operand[1]->getAssignedVal(true)->printSmallArray()<<"\n";
        auto second_op=operand[1]->getAssignedVal(true);
        auto length = second_op->getLength();
        auto f = OP(0);
        IRValue_ =builder.CreateConvolve(OP_PTR(0), second_op->getBufferPtr(), length, -(length / 2 + shiftParameter),type_, getUniqueName());
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
            BufferTypeEn bufferType=isReturned()? BufferTypeEn::output: BufferTypeEn::internal;
            if (isReturned()) {
                builder.AddBufferAlloca(new OutputBuffer(this));
            }
            else 
                builder.AddBufferAlloca(new Buffer(this));
            
            IRBufferBasePtr_=builder.CreateBufferInit(type_,"internal_");
            is_initialized=true;
        }
        builder.SetStoreInsertPoint();
        IRBufferPtr_ = builder.CreateInBoundsGEP(IRBufferBasePtr_, builder.getCurrentOffsetValue(), "offset_incr");
        builder.CreatePositionalStore(IRValue_, IRBufferPtr_);
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
        Type * volatile t= builder.getLLVMType(type_);
        if (!is_initialized) {
            builder.AddBufferAlloca(new InputBuffer(this));
            IRBufferBasePtr_ = builder.CreateBufferInit(type_, "external_");
            is_initialized  = true;
        }
        IRBufferPtr_   = builder.CreatePositionalInBoundsGEP(IRBufferBasePtr_, builder.getCurrentOffsetValue(), "offset_arg_incr");
        IRValue_       = builder.CreatePositionalLoad(IRBufferPtr_, "arg_buffer_");
    }
}

void  Call::setupIR(IRGenerator & builder){
    //pass
}
