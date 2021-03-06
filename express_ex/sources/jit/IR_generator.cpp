

#include "jit/IR_generator.h"
#include "jit/table.h"

//using namespace llvm;


IRGenerator::IRGenerator(llvm::LLVMContext & context,Table * table,bool is_pure_function)
    :IRBuilder<>(context),
    is_pure_function_(is_pure_function ),
    table_(table)
{
    nextBufferGeoup();
}

IRGenerator::~IRGenerator(){
    for(auto buffer_group : buffer_list_){
        for(auto i: *buffer_group)
            delete i;
        delete buffer_group;
    }
}

llvm::Value * IRGenerator::CreateFPow(llvm::Value *aOperand, llvm::Value *bOperand, const std::string &name) {
    if (table_ == nullptr) 
        return nullptr;

    if (aOperand->getType() == getFloatTy())
        return CreateCall(table_->getFloatBIFunc(OpCodeEn::fpow),  { aOperand, bOperand }, name);
    else
        return CreateCall(table_->getDoubleBIFunc(OpCodeEn::fpow), { aOperand, bOperand }, name);
}

llvm::Value * IRGenerator::CreateConst(int64_t &binaryValue, TypeEn targetTy, const std::string &name) {
    llvm::Value * ret = nullptr;

    auto &context = getContext();
    switch (targetTy)
    {
    case TypeEn::int1_jty:   ret = getInt1(*((bool    *)(&binaryValue)));   break;
    case TypeEn::int8_jty:   ret = getInt8(*((int8_t  *)(&binaryValue)));   break;
    case TypeEn::int16_jty:  ret = getInt16(*((int16_t*)(&binaryValue)));   break;
    case TypeEn::int32_jty:  ret = getInt32(*((int32_t*)(&binaryValue)));   break;
    case TypeEn::int64_jty:  ret = getInt64(*((int64_t*)(&binaryValue)));   break;
    case TypeEn::float_jty:  ret = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), *((float  *)(&binaryValue)));  break;
    case TypeEn::double_jty: ret = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), *((double *)(&binaryValue))); break;
    case TypeEn::unknown_jty:ret = nullptr;                                    break;
    default:                 ret = nullptr;                                    break;
    }

    return ret;
}

llvm::Value * IRGenerator::CreateArithmetic(llvm::Value *aOperand, llvm::Value *bOperand, OpCodeEn opCode, const std::string &name) {
    llvm::Value * ret = nullptr;
    switch (opCode)
    {
    case OpCodeEn::add:  ret = CreateAdd(aOperand, bOperand, name);  break;
    case OpCodeEn::sub:  ret = CreateSub(aOperand, bOperand, name);  break;
    case OpCodeEn::mul:  ret = CreateMul(aOperand, bOperand, name);  break;
    case OpCodeEn::sdiv: ret = CreateSDiv(aOperand, bOperand, name); break;
    case OpCodeEn::srem: ret = CreateSRem(aOperand, bOperand, name); break;
    case OpCodeEn::pow:  ret = nullptr; break;

    case OpCodeEn::fadd: ret = CreateFAdd(aOperand, bOperand, name); break;
    case OpCodeEn::fsub: ret = CreateFSub(aOperand, bOperand, name); break;
    case OpCodeEn::fmul: ret = CreateFMul(aOperand, bOperand, name); break;
    case OpCodeEn::fdiv: ret = CreateFDiv(aOperand, bOperand, name); break;
    case OpCodeEn::frem: ret = CreateFRem(aOperand, bOperand, name); break;
    case OpCodeEn::fpow: ret = CreateFPow(aOperand, bOperand, name); break;
    }
    return ret;
}
/*
sgt,//  signed greater than
sge,//  signed greater or equal
slt,//  signed less than
sle,//  signed less or equal
*/

llvm::Value * IRGenerator::CreateComparsion(llvm::Value *aOperand, llvm::Value *bOperand, OpCodeEn opCode, const std::string & name)
{
    llvm::Value * ret = nullptr;
    switch (opCode)
    {
    case OpCodeEn::eq:  ret = CreateICmpEQ(aOperand, bOperand, name);  break;
    case OpCodeEn::ne:  ret = CreateICmpNE(aOperand, bOperand, name);  break;
    case OpCodeEn::sgt: ret = CreateICmpSGT(aOperand, bOperand, name);  break;
    case OpCodeEn::sge: ret = CreateICmpSGE(aOperand, bOperand, name); break;
    case OpCodeEn::slt: ret = CreateICmpSLT(aOperand, bOperand, name); break;
    case OpCodeEn::sle: ret = CreateICmpSLT(aOperand, bOperand, name); break;

    case OpCodeEn::oeq: ret = CreateFCmpOEQ(aOperand, bOperand, name); break;
    case OpCodeEn::one: ret = CreateFCmpONE(aOperand, bOperand, name); break;
    case OpCodeEn::ogt: ret = CreateFCmpOGT(aOperand, bOperand, name); break;
    case OpCodeEn::oge: ret = CreateFCmpOGE(aOperand, bOperand, name); break;
    case OpCodeEn::olt: ret = CreateFCmpOLT(aOperand, bOperand, name); break;
    case OpCodeEn::ole: ret = CreateFCmpOLE(aOperand, bOperand, name); break;
    }
    return ret;
}


    //builder.CreateCall(UpdateBufferF, ConstVars[0]);
llvm::Value * IRGenerator::CreateInv(llvm::Value *aOperand, OpCodeEn opCode, const std::string &name) {
    llvm::Value * ret = nullptr;

    if (opCode == OpCodeEn::neg)
        return CreateNeg(aOperand, name);
    else
        return CreateFNeg(aOperand,name);
}

llvm::Value * IRGenerator::CreateTypeConv(llvm::Value * aOperand, OpCodeEn opCode, TypeEn targetTy, const std::string &name)
{
    llvm::Type* destTy = getLLVMType(targetTy);

    llvm::Value * ret=nullptr;
    switch (opCode)
    {
    case OpCodeEn::trunc:   ret = CreateTrunc(aOperand, destTy, name);    break;
    case OpCodeEn::fptrunc: ret = CreateFPTrunc(aOperand, destTy, name);  break;
    case OpCodeEn::uitofp:  ret = CreateUIToFP(aOperand, destTy, name);   break;
    case OpCodeEn::sitofp:  ret = CreateSIToFP(aOperand, destTy, name);   break;
    case OpCodeEn::fptoi:   ret = CreateFPToUI(aOperand, destTy, name);   break;
    case OpCodeEn::fptosi:  ret = CreateFPToSI(aOperand, destTy, name);   break;
    case OpCodeEn::sext:    ret = CreateSExt(aOperand, destTy, name);     break;
    case OpCodeEn::zext:    ret = CreateZExt(aOperand, destTy, name);     break;
    case OpCodeEn::fpext:   ret = CreateFPExt(aOperand, destTy, name);    break;
    default: ret = aOperand;
    }
    return ret;
}

llvm::Value * IRGenerator::CreateBuiltInFunc(llvm::Value * aOperand, OpCodeEn opCode, const std::string &name)
{
    if (table_ == nullptr) return nullptr;
    if (aOperand->getType() == getFloatTy()) {
        return CreateCall(table_->getFloatBIFunc(opCode), { aOperand }, name);
    }
    else {
        return CreateCall(table_->getDoubleBIFunc(opCode), { aOperand }, name);
    }
}

llvm::Value * IRGenerator::CreateConvolve(llvm::Value * aOperand,  char * ptr,int64_t length, int64_t shift, TypeEn type, const std::string &name)
{
    auto function = table_->getConvolveFunc(type);
    auto bOperand = CreateIntToPtr(
        llvm::ConstantInt::get(getInt64Ty(), uintptr_t(ptr)),
        llvm::PointerType::getUnqual(getLLVMType(type)));

    return CreateCall(function, { aOperand, bOperand, getInt64(length) ,getInt64(shift) }, name);
}


llvm::Value *IRGenerator::CreateCall_(llvm::Value *Calle, llvm::ArrayRef<llvm::Value *> Args ,const std::string & Name ) {
    llvm::MDNode *FPMathTag = nullptr;

    return CreateCall(
        llvm::cast<llvm::FunctionType>(Calle->getType()->getPointerElementType()), Calle,
        Args, Name, FPMathTag);
}

llvm::Value * IRGenerator::CreateConvolve(llvm::Value * aOperand, llvm::Value * bOperand, const std::string &name)
{
    llvm::Value * ret =nullptr, * convolveFunction=nullptr;
    llvm::Type * type=aOperand->getType()->getPointerElementType();

    if (type == getDoubleTy())
        convolveFunction=convolve_double_function_;
    else if (type == getFloatTy())
        convolveFunction=convolve_float_function_;
    else if (type == getInt64Ty())
        convolveFunction=convolve_I64_function_;
    else if (type == getInt32Ty())
        convolveFunction=convolve_I32_function_;
    else
        return ret;
    //if (ret == nullptr) print_error("CreateConvolve :" );


    return CreateCall_(convolveFunction, { aOperand ,bOperand }, name);
}

llvm::Value * IRGenerator::CreateGPUConvolve(llvm::Value * aOperand, char * ptr, int64_t length, int64_t shift, TypeEn type, const std::string &name)
{
    print_IR_error("CreateGPUConvolve is not supported yet");
    auto function = table_->getGPUConvolveFunc(type);
    return nullptr;
}

llvm::Value * IRGenerator::CreatePositionalAlloca(llvm::Type * ty, int64_t i, const std::string &name)
{
    SetInitInsertPoint();
    llvm::Value* ret = CreateAlloca(ty, 0, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalOffset( std::string name,int64_t startValue)
{
    llvm::Value* ret=CreatePositionalOffsetAlloca("", startValue);
    SetLoadInsertPoint();
    current_offset_value_=CreateLoad(ret, "offset" );
    SetCalcInsertPoint();
    return ret;
}


llvm::Value * IRGenerator::CreatePositionalOffsetAlloca(std::string name, int64_t startValue)
{
    auto ty=getInt64Ty();
    SetInitInsertPoint();
    llvm::Value* ret = CreateAlloca(ty, 0, "offset_alloca_");
    current_offset_value_alloca_=ret;
    CreateStore(getInt64(startValue), ret);
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalInBoundsGEP(llvm::Value * Ptr, llvm::ArrayRef<llvm::Value*> IdxList, const std::string & name)
{
    SetLoadInsertPoint();
    llvm::Value* ret = CreateInBoundsGEP(Ptr, IdxList, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalLoad(llvm::Value * aOperand, const std::string &name)
{
    SetLoadInsertPoint();
    llvm::Value* ret = CreateLoad(aOperand, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalLoad(llvm::Value * aOperand, bool isVolatile, const std::string &name)
{
    SetLoadInsertPoint();
    llvm::Value* ret = CreateLoad(aOperand, isVolatile, name);
    SetCalcInsertPoint();
    return ret;
}

llvm::Value * IRGenerator::CreateLoadOffset( const std::string & name)
{
    current_offset_value_ =CreateLoad(current_offset_value_alloca_, name);
    return current_offset_value_;
}

void    IRGenerator::CreatePositionalStore(llvm::Value * value, llvm::Value * ptr, bool isVolatile )
{
    SetStoreInsertPoint();
    CreateStore(value, ptr, isVolatile);
    SetCalcInsertPoint();
}

llvm::Value * IRGenerator::CreateBufferInit(TypeEn targetTy, const std::string &name)
{
    int number_of_buffer = (int) buffers_List_.size();
    std::string number_of_buffer_txt=std::to_string(number_of_buffer);

    SetInitInsertPoint();
    llvm::Value* arg              = current_Function_->getArg(0);

    llvm::Value* untyped_buffer_ptr =CreateInBoundsGEP(
        getInt64Ty()->getPointerTo(),
        arg,
        getInt32(number_of_buffer),
        name + "untyped_buffer_ptr_"+ number_of_buffer_txt);

    llvm::Value* untyped_buffer    = CreateLoad(
        untyped_buffer_ptr,
        true,
        name+"untyped_buffer_"+ number_of_buffer_txt);


    llvm::Value* buffer = CreateBitCast(
        untyped_buffer,
        getLLVMType(targetTy)->getPointerTo(),
        name + "buffer_"+ number_of_buffer_txt);
    
    buffers_List_.push_back(buffer);

    SetCalcInsertPoint();
    return buffer;
}

void IRGenerator::CreateStartBRs()
{
    CreateMidleBRs();
}

void IRGenerator::CreateMidleBRs()
{

    SetIntermediateInsertPoint();
    CreateBr(getLoadBlock());
    SetLoadInsertPoint();
    CreateBr(getCalcBlock());
    SetCalcInsertPoint(); 
    CreateBr(getStoreBlock());
    //SetStoreInsertPoint();
    SetLastInsertPoint();
}

void * IRGenerator::AddBufferAlloca(Buffer *s)
{
    buffer_list_.back()->push_back(s);
    return nullptr;
}





void IRGenerator::SetDeclareConvolve(llvm::Type * type, uintptr_t addr) //atavism
{
    typedef llvm::Value * (*lambla_ty)(llvm::Type * type, uintptr_t addr);

    auto  lambda =[this](llvm::Type * type, uintptr_t addr) {
        return CreateIntToPtr(
            llvm::ConstantInt::get(getInt64Ty(), uintptr_t(addr)),
            llvm::PointerType::getUnqual(
                llvm::FunctionType::get(type,
                    { type->getPointerTo(), type->getPointerTo() },
                    false)));
    };

    if (type == getDoubleTy())
        convolve_double_function_=lambda(type, addr);
    else if (type == getFloatTy())
        convolve_float_function_=lambda(type, addr);
    else if (type == getInt64Ty())
        convolve_I64_function_=lambda(type, addr);
    else if (type == getInt32Ty())
        convolve_I32_function_=lambda(type, addr);
    
}

void IRGenerator::SetOffsetToZero()
{   
    llvm::BasicBlock* tmp=BB;
    CreateStore(getInt64(0), current_offset_value_alloca_);
}

void IRGenerator::SetOffsetTo(int64_t val)
{
    llvm::BasicBlock* tmp=BB;
    CreateStore(getInt64(val), current_offset_value_alloca_);
}

llvm::Type * IRGenerator::getLLVMType(TypeEn targetTy) {
    llvm::Type * ret = nullptr;
    //this->SetInsertPoint
    switch (targetTy)
    {
    case TypeEn::int1_jty:   ret = getInt1Ty();   break;
    case TypeEn::int8_jty:   ret = getInt8Ty();   break;
    case TypeEn::int16_jty:  ret = getInt16Ty();  break;
    case TypeEn::int32_jty:  ret = getInt32Ty();  break;
    case TypeEn::int64_jty:  ret = getInt64Ty();  break;
    case TypeEn::float_jty:  ret = getFloatTy();  break;
    case TypeEn::double_jty: ret = getDoubleTy(); break;
    case TypeEn::unknown_jty:ret = nullptr;          break;
    default:                 ret = nullptr;          break;
    }
    
    return ret;
}

llvm::Module* IRGenerator::getCurrentModule(){
    return table_->getModule();
}

llvm::BasicBlock* IRGenerator::CreateNewBB(BasicBlockPtr& prot, const std::string& name){
    prot = llvm::BasicBlock::Create(Context, name, getCurrentFunction());
    bb_List_.push_back(prot);
    return prot;
}
