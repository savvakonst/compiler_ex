
#include <sstream>



#include "parser/body.h"
#include "parser/operations.h"
#include "parser/line.h"
#include "parser/call.h"
#include "parser/basic.h"

#include "jit/buffer.h"
#include "jit/table.h"
#include "jit/llvmHdrs.h"
#include "jit/IR_generator.h"
#include "ifs/printer.h"

//using namespace llvm;


SubBlock::SubBlock(Value * var){

    left_length_=var->getLeftBufferLen();
    right_length_=var->getRightBufferLen();
    length_=var->getLength();
    setUint(var);
}

void SubBlock::setUint(Value * var)
{
    left_length_=maxInt(left_length_, var->getLeftBufferLen());
    right_length_=maxInt(right_length_, var->getRightBufferLen());
    uint_list_.push(var);
}

string SubBlock::print()
{
    const size_t max_line_length=90;
    string out=std::string(4, ' ')+"L,R: " + std::to_string(left_length_) + "," + std::to_string(right_length_) + "\n";
    for (auto i : uint_list_) {
        std::string txt       = i->printUint() + ";" + (i->isBuffered() ? " store" : "");
        std::string txtShifts = std::to_string(i->getLeftBufferLen()) + " : " + std::to_string(i->getRightBufferLen());
        std::string txtSkip   = std::string(max_line_length - ((txt.length() > max_line_length) ? 0 : txt.length()), ' ');

        out+=std::string(6, ' ') + txt + txtSkip + txtShifts + "\n";
    }
    return out;
}

bool SubBlock::generateIR(IRGenerator & builder, CycleStageEn type, std::string basic_block_prefix, std::string basic_block_postfix)
{
    llvm::LLVMContext & context = builder.getContext();

    std::string  level_txt=basic_block_postfix;


    llvm::BasicBlock* bb_intermediate  = llvm::BasicBlock::Create(context, "intermediate_" + basic_block_prefix + level_txt, builder.getCurrentFunction());
    llvm::BasicBlock* bb_load  = llvm::BasicBlock::Create(context, "load_"   + basic_block_prefix + level_txt, builder.getCurrentFunction());
    llvm::BasicBlock* bb_calc  = llvm::BasicBlock::Create(context, "calc_"   + basic_block_prefix + level_txt, builder.getCurrentFunction());
    llvm::BasicBlock* bb_store = llvm::BasicBlock::Create(context, "store_"   + basic_block_prefix + level_txt, builder.getCurrentFunction());
    //BasicBlock* bb_terminal_op = BasicBlock::Create(context_, "terminal_op_" + basic_block_prefix + level_txt, builder.getCurrentFunction());

    llvm::BasicBlock* bb_last_store=builder.getStoreBlock();

    if (nullptr != bb_last_store)
    {
        builder.CreateStartBRs();
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_intermediate);
    }

    builder.ClearInitializedValueList();

    builder.SetIntermediateInsertPoint(bb_intermediate);
    builder.SetLoadInsertPoint(bb_load);
    builder.SetCalcInsertPoint(bb_calc);
    builder.SetStoreInsertPoint(bb_store);
    //builder.SetTerminalOpInsertPoint(bb_terminal_op);

    builder.SetCalcInsertPoint();


    llvm::Value* alloc =builder.CreatePositionalOffset(basic_block_prefix + level_txt, - (int64_t)left_length_ );
    //Value* alloc =builder.CreatePositionalOffset(basic_block_prefix + level_txt, 0);
    builder.SetStoreInsertPoint();
    llvm::Value* next_offset =builder.CreateAdd(builder.getCurrentOffsetValue(), builder.getInt64(1));
    builder.CreateStore(next_offset, alloc);
    builder.SetCurrentCMPRes(
        builder.CreateICmpSLT(
            next_offset,
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
Block::Block(Value* var) {
    left_ength_=var->getLeftBufferLen();
    right_length_=var->getRightBufferLen();
    level_=var->getLevel();
    length_=var->getLength();
    setUint(var);
}

void Block::setUint(Value * var){

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

void Block::setUintToSubtable(Value * var)
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


bool Block::generateIR(IRGenerator &builder, CycleStageEn type, std::string basic_block_prefix)
{
    llvm::LLVMContext & context = builder.getContext();

    std::string  level_txt=std::to_string(level_);

    if ( (type == CycleStageEn::midle) || (type == CycleStageEn::end) ){
        
        if ((type == CycleStageEn::end) & (0 == ( length_ % buffer_length_ )))
            return false;


        llvm::BasicBlock* bb_intermediate = llvm::BasicBlock::Create(context, "intermediate_" + basic_block_prefix + level_txt, builder.getCurrentFunction());
        llvm::BasicBlock* bb_load  = llvm::BasicBlock::Create(context, "load_"  + basic_block_prefix + level_txt, builder.getCurrentFunction());
        llvm::BasicBlock* bb_calc  = llvm::BasicBlock::Create(context, "calc_"  + basic_block_prefix + level_txt, builder.getCurrentFunction());
        llvm::BasicBlock* bb_store = llvm::BasicBlock::Create(context, "store_" + basic_block_prefix + level_txt, builder.getCurrentFunction());
        //BasicBlock* bb_terminal_op = BasicBlock::Create(context_, "terminal_op_" + basic_block_prefix + level_txt, builder.getCurrentFunction());

        llvm::BasicBlock* bb_last_store=builder.getStoreBlock();

        if (nullptr != bb_last_store) {
            builder.CreateMidleBRs();
            builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_intermediate);
        } else {
            builder.SetLoopEnterInsertPoint();
            builder.CreateBr(bb_intermediate);
        }

        builder.ClearInitializedValueList();

        builder.SetIntermediateInsertPoint(bb_intermediate);
        //builder.SetOffsetToZero();
        builder.SetOffsetTo(right_length_);

        builder.SetLoadInsertPoint(bb_load);
        builder.SetCalcInsertPoint(bb_calc);
        builder.SetStoreInsertPoint(bb_store);
        
        builder.SetLoadInsertPoint();

        llvm::Value* offset_alloc = builder.getCurrentOffsetValueAlloca();
        llvm::Value* offset= builder.CreateLoadOffset();

        builder.SetStoreInsertPoint();

        llvm::Value* next_offset =builder.CreateAdd(offset, builder.getInt64(1));
        builder.CreateStore(next_offset, offset_alloc);

        uint64_t len = (type == CycleStageEn::midle) ? buffer_length_ : length_ % buffer_length_ ;
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
            i->generateIR(builder, type, basic_block_prefix, levelTxt+"_"+std::to_string(sub_level));
            sub_level++;
        }

    }
    return true;
}




//TableColumn:: column section 
//
//
TableColumn::TableColumn (Value* var) {
    length_ =var->getLength();
    setUint(var);
}

void    TableColumn::setUint(Value * var){
    auto  varlevel= var->getLevel();
    for (Block* i : blockList_) 
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

llvm::Value* genConvolve(IRGenerator& builder, llvm::Type* type, uintptr_t addr){

    llvm::Value* convolveFunction =
        builder.CreateIntToPtr(
            llvm::ConstantInt::get(builder.getInt64Ty(), uintptr_t(addr)),
            llvm::PointerType::getUnqual(
                llvm::FunctionType::get(builder.getInt64Ty(),
                                        {type->getPointerTo(), type->getPointerTo()},
                                        false
                )
            ));
    //convolveFunction->setName("convolve");
    return  convolveFunction;
}



std::vector<std::list<Buffer*>*>* g_list_of_buffer_groups=nullptr;


ParametersDB*  g_IO_IFS =nullptr;

bool isInvalidBuffers() {
    return 
        nullptr == g_list_of_buffer_groups ;
}

int32_t initBuffers() {
    if (isInvalidBuffers())
        return 1;
    for (auto group : *g_list_of_buffer_groups)
        for(auto i : *group)
            i->init();
    return 0;
}
/*
int32_t updateBuffer() {
    if (isInvalidBuffers()) 
        return 1;
    for(auto group : *g_list_of_buffer_groups)
        for(auto i : *group)
            i->update();
    return 0;
}
*/
int32_t updateBuffer(int32_t group_number){
    if(isInvalidBuffers())
        return 1;
    for(auto i : *((*g_list_of_buffer_groups)[group_number]))
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


// add Value to table_ 
void    Table::setUint(Value * var){
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

llvm::Function*  Table::getBIFunc(BuiltInFuncMap &UBIFMap, OpCodeEn op, llvm::Type * Ty) {
    auto pos=float_BI_func_map_.find(op);
    if (pos != float_BI_func_map_.end()) {
        return pos->second;
    }
    else {
        llvm::Intrinsic::ID code =BIF2LLVMmap_[op];
        llvm::Function* f=llvm::Intrinsic::getDeclaration(M_, code, Ty);
        (UBIFMap)[op] = f;
        return f;
    }
}

void    Table::declareFunctions() {
    llvm::LLVMContext & context = M_->getContext();
    fTy_ = llvm::Type::getFloatTy(context);
    dTy_ = llvm::Type::getDoubleTy(context);
    declareBuiltInFunctions(float_BI_func_map_, fTy_);
}

void    Table::declareBuiltInFunctions(BuiltInFuncMap &UBIFMap, llvm::Type * Ty) {
    BIF2LLVMmap_ = {
        {OpCodeEn::fpow, llvm::Intrinsic::pow},
        {OpCodeEn::cos,  llvm::Intrinsic::cos},
        {OpCodeEn::sin,  llvm::Intrinsic::sin},
        {OpCodeEn::exp,  llvm::Intrinsic::exp},
        {OpCodeEn::log,  llvm::Intrinsic::log},
        {OpCodeEn::log2, llvm::Intrinsic::log2},
        {OpCodeEn::log10,llvm::Intrinsic::log10}
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

    for (auto  i : column_list_){
        uint64_t length = i->getLength();
        bool exist = false;
       
        for (auto &j : group_list_) {
            auto remainder =(j.max_length > length) ? j.max_length % length : length % j.max_length;
            if (remainder == 0) {
                exist=true;
                j.max_length = length > j.max_length ? length : j.max_length;
                j.min_length = length < j.min_length ? length : j.min_length;
                j.column_list.push_back(i);
                break;
            }
        }

        if (!exist) {
            Group g={length, length};
            g.column_list.push_back(i);
            group_list_.push_back(g);
        }
    }

    for (auto &j : group_list_) {

        if(j.max_length < max_buffer_length_)
            max_buffer_length_ = j.max_length/4;

        int64_t sub_max_buffer_length = min_buffer_length_ * j.max_length / j.min_length;
        
        if (sub_max_buffer_length > max_buffer_length_) {
            llvm::outs() << "max_buffer_length_ = sub_max_buffer_length;\n";
            max_buffer_length_ = sub_max_buffer_length;
        }
        
        iterations_ =  j.max_length / max_buffer_length_;
        j.iterations =  iterations_;

        for (auto i : j.column_list){
            i->setBufferLength(max_buffer_length_ / (j.max_length / i->getLength()));
        }
    }

    if (group_list_.size() !=1)
        print_IR_error("non multiple large array lengths is not supported yet\n");

    //iterations=group_list[0].column_list[0]->get_length()/ group_list[0].column_list[0]->get();
}

DLL_EXPORT void  jit_init();
void jit_init() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}


//static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

void configOptimization(llvm::legacy::FunctionPassManager* TheFPM) {
    TheFPM->add(llvm::createPartiallyInlineLibCallsPass());
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->doInitialization();
}

bool Table::llvmInit() {
    context_ = new llvm::LLVMContext();

    module_u_ptr_ = std::make_unique<llvm::Module>("test", *context_);
    the_FPM_ = std::make_unique<llvm::legacy::FunctionPassManager>(module_u_ptr_.get());
    M_ = module_u_ptr_.get();

    configOptimization(the_FPM_.get());
    declareFunctions();

    return true;
}


llvm::Function* CreateConvolveFunction(llvm::Module* M, TypeEn type) {
    llvm::LLVMContext & context = M->getContext();

    IRGenerator builder(context, nullptr);

    auto currentType = builder.getLLVMType(type);
    
    llvm::Function* function =
        llvm::Function::Create(
            llvm::FunctionType::get(currentType, {currentType->getPointerTo(), currentType->getPointerTo(), builder.getInt64Ty(), builder.getInt64Ty() }, false),
            llvm::Function::ExternalLinkage, "convolveFunction", M);


    llvm::BasicBlock* bb_Entry = llvm::BasicBlock::Create(context, "entry_block", function);
    llvm::BasicBlock* bb_LoopBody = llvm::BasicBlock::Create(context, "loop_body_block", function);
    llvm::BasicBlock* bb_exit = llvm::BasicBlock::Create(context, "exit_block", function);


    auto offset = function->getArg(3);
    auto num_of_samples = function->getArg(2);
    auto small_arr_base_ptr = function->getArg(1);

    // entry_block
    builder.SetInsertPoint(bb_Entry);
    auto big_arr_base_ptr =  builder.CreateInBoundsGEP(function->getArg(0), offset);
    
    auto index_ptr = builder.CreateAlloca(builder.getInt64Ty(),nullptr,"index_ptr");
    auto sum_ptr = builder.CreateAlloca(currentType, nullptr,"sum_ptr");

    int64_t zero = 0;
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

    auto a = builder.CreateLoad(big_arr_ptr, "a");
    auto b = builder.CreateLoad(small_arr_ptr, "b");
    auto sum_v = builder.CreateLoad(sum_ptr, "sum_v");

    llvm::Value *next_sum;
    if (isInteger(type)) {
        auto mull = builder.CreateMul(a, b, "mull");
        next_sum = builder.CreateAdd(mull, sum_v, "next_sum");
    }
    else {
        auto mull = builder.CreateFMul(a, b, "mull");
        next_sum = builder.CreateFAdd(mull, sum_v, "next_sum");
    }

    builder.CreateStore(next_sum, sum_ptr);
    builder.CreateStore(next_index, index_ptr);
    auto cond = builder.CreateICmpSLT(next_index, num_of_samples);
    builder.CreateCondBr(cond, bb_LoopBody, bb_exit);

    //exit_block
    builder.SetInsertPoint(bb_exit);

    auto return_val=builder.CreateLoad(sum_ptr);
    builder.CreateRet(return_val);

    return function;
}


llvm::Function * Table::getConvolveFunc(TypeEn type){
    llvm::Function * function = convolve_map_[type];
    if (function == nullptr)
        function=CreateConvolveFunction(M_, type);
        convolve_map_[type] = function;
    return function;
}

llvm::Function * Table::getGPUConvolveFunc(TypeEn type) {
    llvm::Function * function = convolve_map_[type];
    if (function == nullptr)
        function=CreateConvolveFunction(M_, type);
    convolve_map_[type] = function;
    return function;
}


bool Table::generateIR(std::string basicBlockPrefix) {


    llvm::LLVMContext & context = M_->getContext();
    if(builder_==nullptr)
        builder_=new IRGenerator(context, this);
    IRGenerator &builder = *builder_;


    buffer_update_function_ =
        llvm::Function::Create(
            llvm::FunctionType::get(builder.getInt32Ty(), {}, false),
            llvm::Function::ExternalLinkage,
            "buffer_update_function",
            M_
        );
    builder.SetBufferUpdateFunction(buffer_update_function_);

    main_function_ = llvm::Function::Create(
        llvm::FunctionType::get(llvm::Type::getInt32Ty(context), {llvm::Type::getInt64PtrTy(context)->getPointerTo()->getPointerTo()}, false),
        llvm::Function::ExternalLinkage,
        "main", 
        M_
    );
    llvm::BasicBlock* bb= llvm::BasicBlock::Create(context, "init_block", main_function_);
    

    for(auto i: const_list_)
        i->setupIR(builder);

    for(auto i: small_array_list_)
        i->calculate();


    uint32_t group_index = 0;
    for(auto& i : group_list_){
        generateIRInGroup(i, group_index);
        group_index++;
    }
    builder.SetInsertPoint(&(main_function_->back()));
    builder.CreateRet(builder.getInt32(1));

    g_list_of_buffer_groups = builder.getBufferList();
    
    return true;
}


bool Table::generateIRInGroup(Group &group, uint32_t index){

    llvm::LLVMContext& context = M_->getContext();
    IRGenerator& builder = *builder_;

    llvm::Function *sub_function = llvm::Function::Create(
        llvm::FunctionType::get(llvm::Type::getInt32Ty(context), {llvm::Type::getInt64PtrTy(context)->getPointerTo()}, false),
        llvm::Function::ExternalLinkage,
        "main",
        M_
    );

    llvm::Constant * group_index = builder.getInt32(index);

    ///the main "For loop"
    ///creating jump commands for init cycle
    builder.SetCurrentFunction(sub_function);
    llvm::BasicBlock* bb= llvm::BasicBlock::Create(context, "init_block", sub_function);
    builder.SetInitInsertPoint(bb);

    auto max_level = getMaxLevel() + 1;
    for(uint32_t j=0; j < max_level; j++)
        for(auto i : column_list_)
            i->generateIR(builder, CycleStageEn::start, j);

    ///creating jump commands for init cycle
    builder.CreateStartBRs();
    //cond jump to Main Cycle Enter

    llvm::BasicBlock* bb_update_buffer = llvm::BasicBlock::Create(context, "update_buffer_block", builder.getCurrentFunction());
    llvm::BasicBlock* bb_loop_enter = llvm::BasicBlock::Create(context, "loop_enter_block", builder.getCurrentFunction());

    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_update_buffer);

    builder.SetInsertPoint(bb_update_buffer);
    builder.CreateCall_(buffer_update_function_, group_index); //call buffer update
    builder.CreateBr(bb_loop_enter);

    builder.SetLoopEnterInsertPoint(bb_loop_enter);
    builder.DropBaseInsertPoint();

    llvm::Value* glob_index_alloca=builder.CreatePositionalOffsetAlloca("glob_index_alloca", 0);
    builder.CreatePositionalOffsetAlloca("common_offset_alloca", 0);


    builder.SetLoopEnterInsertPoint(bb_loop_enter);
    llvm::Value* glob_index=builder.CreateLoad(glob_index_alloca, "glob_index");


    for(uint32_t j=0; j < max_level; j++)
        for(auto i : column_list_)
            i->generateIR(builder, CycleStageEn::midle, j);

    builder.CreateMidleBRs();
    llvm::BasicBlock* bb_cycle_exit = llvm::BasicBlock::Create(context, "cycle_exit_block", builder.getCurrentFunction());
    builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bb_cycle_exit);
    builder.SetCycleExitInsertPoint(bb_cycle_exit);

    ///global index increment, and index comparsion with maxTypeVar iteration number
    ///increasing the global index and comparing the index with the maxTypeVar iteration number
    llvm::Value* next_glob_index = builder.CreateAdd(glob_index, builder.getInt64(1));
    builder.CreateStore(next_glob_index, glob_index_alloca);
    builder.SetCurrentCMPRes(
        builder.CreateICmpSLT(
            next_glob_index,
            builder.getInt64((group.iterations - 1))));


    llvm::BasicBlock* bb_terminal_loop_enter = llvm::BasicBlock::Create(context, "terminal_loop_enter", builder.getCurrentFunction());
    builder.CreateCall_(buffer_update_function_, group_index); //call buffer update
    builder.CreateCondBr(builder.getCurrentCMPRes(), bb_loop_enter, bb_terminal_loop_enter);
    builder.SetLoopEnterInsertPoint(bb_terminal_loop_enter);
    builder.DropBaseInsertPoint();

    bool  is_not_idle = false;
    for(uint32_t j=0; j < max_level; j++)
        for(auto i : column_list_)
            is_not_idle |= i->generateIR(builder, CycleStageEn::end, j);

    if(is_not_idle){
        builder.CreateMidleBRs();
        llvm::BasicBlock* bbExit = llvm::BasicBlock::Create(context, "exit_block", builder.getCurrentFunction());
        builder.CreateCondBr(builder.getCurrentCMPRes(), builder.getLoadBlock(), bbExit);
        builder.SetExitInsertPoint(bbExit);
        builder.CreateCall_(buffer_update_function_, group_index); //call buffer update
    }

    builder.CreateRet(builder.getInt32(1));
    ///create jump from init block to the next
    builder.SetInitInsertPoint();
    builder.CreateBr(builder.getBlock(1));

    builder.nextBufferGeoup();


    builder.SetInsertPoint(&(main_function_->back()));
    auto buffer_group_ptr = builder.CreateInBoundsGEP(main_function_->getArg(0), builder.getInt32(index), "offset_incr");
    builder.CreateCall_(sub_function, buffer_group_ptr);

}




bool Table::runOptimization() {
    the_FPM_->run(*main_function_);
    for (auto i : convolve_map_)
        the_FPM_->run(*i.second);
    return true;
}

bool Table::run() {

    //auto mainF=M_->getFunction("main");
    std::string errStr;
    llvm::ExecutionEngine* EE = llvm::EngineBuilder(std::move(module_u_ptr_)) .setErrorStr(&errStr).create();
    EE->addGlobalMapping("buffer_update_function", uintptr_t(updateBuffer));

    if (!EE) {
        llvm::outs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
        return false;
    }

    if (verifyFunction(*main_function_, &llvm::outs())) {
        llvm::outs() << ": Error constructing main function!\n\n";
        return false;
    }

    for (auto i : convolve_map_)
        if (verifyFunction( *i.second, &llvm::outs())) {
            llvm::outs() << ": Error constructing main function!\n\n";
            return false;
        }
        

    if (verifyModule(*M_, &llvm::outs())) {
        llvm::outs() << ": Error constructing module!\n";
        return false;
    }

    EE->finalizeObject();

    Jit_Call_t Call = (Jit_Call_t)EE->getPointerToFunction(main_function_);

    initBuffers();

    char*** buffer_groups_array = new char** [g_list_of_buffer_groups->size()];
    char*** buffer_groups_array_temp =buffer_groups_array;
    for(auto& buffer_group : *g_list_of_buffer_groups){
        char** buffers_array = new char* [buffer_group->size()];
        *(buffer_groups_array_temp++)=buffers_array;

        for(auto& i : *buffer_group)
            *(buffers_array++)=i->getPtr();
    }

    Call(buffer_groups_array);

    buffer_groups_array_temp=buffer_groups_array;
    for(auto& buffer_group : *g_list_of_buffer_groups){
        for(auto& i : *buffer_group)
            i->close();
        delete[] * (buffer_groups_array_temp++);
    }
    delete[] buffer_groups_array;
    
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
llvm::Value* Value::getIRValue(IRGenerator & builder, int64_t parentLevel) {
    llvm::Value * ret = nullptr;
    if (isBuffered() & (parentLevel != level_) ) {
        if (!builder.CheckExistence(IR_buffer_base_ptr_)) {
            IR_buffer_ptr_=builder.CreatePositionalInBoundsGEP(IR_buffer_base_ptr_, builder.getCurrentOffsetValue(), "offset_incr");
            IR_loaded_buffer_value_=builder.CreatePositionalLoad(IR_buffer_ptr_, "buffer_");
            builder.AddInitializedValue(IR_buffer_base_ptr_);
        }
        ret = IR_loaded_buffer_value_;
    }
    else 
        ret = IR_value_;

    if ((ret == nullptr) && !builder.is_pure_function_)
        print_IR_error("IRValue - is nullptr :" + getUniqueName());

    return ret;
}

llvm::Value* Value::getIRValueBasePtr(IRGenerator & builder, int64_t parentLevel) {
    auto ret =isBuffered () ? IR_buffer_base_ptr_ : nullptr;
    if (ret == nullptr) 
        print_IR_error("getIRValueBasePtr - is nullptr :" + getUniqueName());
    return ret;
}

llvm::Value* Value::getIRValuePtr(IRGenerator & builder, int64_t parentLevel) {
    auto ret =isBuffered () ? IR_buffer_ptr_ : nullptr;
    if (ret == nullptr) 
        print_IR_error("getIRValuePtr - is nullptr :" + getUniqueName());
    return ret;
}

void  Value::setupIR(IRGenerator & builder) {
    IR_value_ = builder.CreateConst(binary_value_, type_, "");
}

void  Operation::setupIR(IRGenerator & builder){

#define OP(i)     (operand_[(i)]->getAssignedVal(true)->getIRValue(builder,level_) )
#define OP_PTR(i) (operand_[(i)]->getAssignedVal(true)->getIRValuePtr(builder,level_) )

    if (isArithetic(op_code_)) {
        IR_value_ =builder.CreateArithmetic(OP(0), OP(1), op_code_, getUniqueName());
    }
    else if (isComparsion(op_code_)) {
        IR_value_ =builder.CreateComparsion(OP(0), OP(1), op_code_, getUniqueName());
    }
    else if (isInv(op_code_)) {
        IR_value_ =builder.CreateInv(OP(0), op_code_, getUniqueName());
    }
    else if (isTypeConv(op_code_)) {
        IR_value_ =builder.CreateTypeConv(OP(0), op_code_, type_, getUniqueName());
    }
    else if (isBuiltInFunc(op_code_)) {
        IR_value_ =builder.CreateBuiltInFunc(OP(0), op_code_, getUniqueName());
    }
    else if (isSelect(op_code_)) {
        if(contain_rec_call_){
            llvm::Value* ret_val = nullptr;
            if(operand_[1]->getAssignedVal(true)->getNodeType() == NodeTypeEn::kTailCall){
                builder.CreateCondBr(OP(0), builder.getCurrentBlock(), builder.getExitBlock());
                ret_val = OP(2);
            }
            else{
                builder.CreateCondBr(OP(0), builder.getExitBlock(), builder.getCurrentBlock());
                ret_val = OP(1);
            }
            builder.SetExitInsertPoint();
            builder.CreateRet(ret_val);
            builder.SetCalcInsertPoint();
        }
        else
            IR_value_ =builder.CreateSelect(OP(0), OP(1), OP(2),  getUniqueName());
    }
    else if (isConvolve(op_code_)) {
        auto second_op=operand_[1]->getAssignedVal(true);
        auto length = second_op->getLength();
        auto f = OP(0);
        IR_value_ = builder.CreateConvolve(OP_PTR(0), second_op->getBufferPtr(), length, -(length / 2 + shift_parameter_),type_, getUniqueName());
    }
    else if (isSlice(op_code_)) {

    }
    else if (isStoreToBuffer(op_code_)) {
        print_IR_error("visitExitTxt StoreToBuffer unknown command .");
    }
    else {
        print_IR_error("visitExitTxt unknown command .");
    }


    if (isBuffered()|isReturned()) {
        if (!is_initialized_) {
            BufferTypeEn bufferType=isReturned()? BufferTypeEn::output: BufferTypeEn::internal;
            if (isReturned()) {
                builder.AddBufferAlloca(new OutputBuffer(this));
            }
            else 
                builder.AddBufferAlloca(new Buffer(this));
            
            IR_buffer_base_ptr_=builder.CreateBufferInit(type_,"internal_");
            is_initialized_=true;
        }
        builder.SetStoreInsertPoint();
        IR_buffer_ptr_ = builder.CreateInBoundsGEP(IR_buffer_base_ptr_, builder.getCurrentOffsetValue(), "offset_incr");
        builder.CreatePositionalStore(IR_value_, IR_buffer_ptr_);
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
        if(isVariable(this)){
            builder.SetInitInsertPoint();
            llvm::Function* function = builder.getCurrentFunction();
            IR_buffer_ptr_ = builder.CreateAlloca(builder.getLLVMType(type_));
            size_t arg_number = builder.arg_ptr_list_.size();
            builder.CreateStore(function->getArg((uint32_t)arg_number), IR_buffer_ptr_);
            builder.arg_ptr_list_.push_back(IR_buffer_ptr_);

            //insert to  loop_block
            builder.SetCalcInsertPoint();
            IR_value_ = builder.CreateLoad(IR_buffer_ptr_);
            
        }
        else{

            if(!is_initialized_){
                builder.AddBufferAlloca(new InputBuffer(this));
                IR_buffer_base_ptr_ = builder.CreateBufferInit(type_, "external_");
                is_initialized_  = true;
            }
            IR_buffer_ptr_ = builder.CreatePositionalInBoundsGEP(IR_buffer_base_ptr_, builder.getCurrentOffsetValue(), "offset_arg_incr");
            IR_value_ = builder.CreatePositionalLoad(IR_buffer_ptr_, "arg_buffer_");
        }
    }
}

void  Call::setupIR(IRGenerator & builder){
    if(builder.is_pure_function_){
        llvm::Function* function = body_->getOrGenIRPureFunction(builder);
        std::vector<llvm::Value*> arg_list;
        for(auto i : args_){
            arg_list.push_back(
                i->getAssignedVal(true)->getIRValue(builder, level_));

        }
        IR_value_ = builder.CreateCall(function, arg_list, "call_" + body_->getName());
    }
}

void CallRecursiveFunction::setupIR(IRGenerator& builder){
    
    llvm::Function* function = body_->getOrGenIRPureFunction(builder);
    std::vector<llvm::Value*> arg_list;
    for(auto i : args_) {
        arg_list.push_back(
            i->getAssignedVal(true)->getIRValue(builder, level_));
        
    }
    IR_value_ = builder.CreateCall(function, arg_list, "call_" + body_->getName());


    if(isBuffered() | isReturned()){ // replace to new function 
        if(!is_initialized_){
            BufferTypeEn bufferType = isReturned() ? BufferTypeEn::output : BufferTypeEn::internal;
            if(isReturned()){
                builder.AddBufferAlloca(new OutputBuffer(this));
            }
            else
                builder.AddBufferAlloca(new Buffer(this));

            IR_buffer_base_ptr_=builder.CreateBufferInit(type_, "internal_");
            is_initialized_=true;
        }
        builder.SetStoreInsertPoint();
        IR_buffer_ptr_ = builder.CreateInBoundsGEP(IR_buffer_base_ptr_, builder.getCurrentOffsetValue(), "offset_incr");
        builder.CreatePositionalStore(IR_value_, IR_buffer_ptr_);
    }


}

void TailCallDirective::setupIR(IRGenerator& builder){
    size_t size = builder.arg_ptr_list_.size();
    for(size_t i = 0; i < size; i++){
        llvm::Value* arg = args_[i]->getAssignedVal(true)->getIRValue(builder, level_);
        builder.CreateStore(arg, builder.arg_ptr_list_[i]);
    }
}


llvm::Function* Body::getOrGenIRPureFunction(IRGenerator& builder){

    if(function_)
        return function_;

    llvm::LLVMContext& context = builder.getContext();
    IRGenerator local_builder(context, nullptr, true);

    std::vector<llvm::Type*> params;
    const std::vector<TypeEn> signature = getSignature().getList();
    for(auto i : signature)
       params.push_back(local_builder.getLLVMType(i));

    function_ =
        llvm::Function::Create(
            llvm::FunctionType::get(
                local_builder.getLLVMType(return_stack_.front()->getType()),
                params, 
                false),
            llvm::Function::ExternalLinkage, getName() , builder.getCurrentModule());

    local_builder.SetCurrentFunction(function_);

    local_builder.SetInitInsertPoint(llvm::BasicBlock::Create(context, "init_block", function_));
    local_builder.SetCalcInsertPoint(llvm::BasicBlock::Create(context, "loop_block", function_));
    local_builder.SetExitInsertPoint(llvm::BasicBlock::Create(context, "exit_block", function_));
    local_builder.SetCalcInsertPoint();

    stack<Value*> visitor_stack;
    RecursiveGenContext gen_context(is_tail_callable_, false);
    
    for(auto& line : lines_){
        if(line->isArg()){        
            gen_context.addArg(line);
        }
        else{
            visitor_stack.push(line->getAssignedVal());
            do{
                auto var = visitor_stack.pop();
                if(var->isVisited())
                    var->genRecursiveVisitExit(&gen_context);
                else
                    var->visitEnter(&visitor_stack);
            } while(!visitor_stack.empty());
            gen_context.setUint(line);
        }
    }

    for(auto& value : return_stack_){
        visitor_stack.push(value->getAssignedVal());
        do{
            auto var = visitor_stack.pop();
            if(var->isVisited())
                var->genRecursiveVisitExit(&gen_context);
            else
                var->visitEnter(&visitor_stack);
        } while(!visitor_stack.empty());
        gen_context.setUint(value);
    }
    
    size_t size = gen_context.instructions_list_.size();
    for(size_t index = 0; index < size; index++)
        gen_context.instructions_list_[index]->setupIR(local_builder);
    
    if(!local_builder.getCurrentBlock()->back().isTerminator()){
        local_builder.SetCalcInsertPoint();
        local_builder.CreateBr(builder.getExitBlock());
        local_builder.SetExitInsertPoint();
        local_builder.CreateRet(
            return_stack_.front()->getAssignedVal(true)->getIRValue(builder, 0));
    }
    
    local_builder.SetInitInsertPoint();
    local_builder.CreateBr(local_builder.getCalcBlock());

    return function_;
}