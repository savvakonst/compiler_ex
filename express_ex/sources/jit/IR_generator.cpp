

#include "IR_generator.h"
#include "table.h"

using namespace llvm;


IRGenerator::IRGenerator(LLVMContext & context,Table * table_):IRBuilder<>(context){
    table=table_;
}

IRGenerator::~IRGenerator(){
    for (auto i : buffer_list_)
        delete i;
}

Value * IRGenerator::CreateFPow(Value *aOperand, Value *bOperand, const std::string &name) {
    if (table == nullptr) 
        return nullptr;

    if (aOperand->getType() == getFloatTy())
        return CreateCall(table->getFloatBIFunc(OpCodeEn::fpow),  { aOperand, bOperand }, name);
    else
        return CreateCall(table->getDoubleBIFunc(OpCodeEn::fpow), { aOperand, bOperand }, name);
}

Value * IRGenerator::CreateConst(uint64_t &binaryValue, TypeEn targetTy, const std::string &name) {
    Value * ret = nullptr;

    auto &context = getContext();
    switch (targetTy)
    {
    case TypeEn::int1_jty:   ret = getInt1(*((bool    *)(&binaryValue)));   break;
    case TypeEn::int8_jty:   ret = getInt8(*((int8_t  *)(&binaryValue)));   break;
    case TypeEn::int16_jty:  ret = getInt16(*((int16_t*)(&binaryValue)));   break;
    case TypeEn::int32_jty:  ret = getInt32(*((int32_t*)(&binaryValue)));   break;
    case TypeEn::int64_jty:  ret = getInt64(*((int64_t*)(&binaryValue)));   break;
    case TypeEn::float_jty:  ret = ConstantFP::get(Type::getFloatTy(context), *((float  *)(&binaryValue)));  break;
    case TypeEn::double_jty: ret = ConstantFP::get(Type::getDoubleTy(context), *((double *)(&binaryValue))); break;
    case TypeEn::unknown_jty:ret = nullptr;                                    break;
    default:                 ret = nullptr;                                    break;
    }

    return ret;
}

Value * IRGenerator::CreateArithmetic(Value *aOperand, Value *bOperand, OpCodeEn opCode, const std::string &name) {
    Value * ret = nullptr;
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

llvm::Value * IRGenerator::CreateComparsion(Value *aOperand, Value *bOperand, OpCodeEn opCode, const std::string & name)
{
    Value * ret = nullptr;
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
Value * IRGenerator::CreateInv(Value *aOperand, OpCodeEn opCode, const std::string &name) {
    Value * ret = nullptr;

    if (opCode == OpCodeEn::neg)
        return CreateNeg(aOperand, name);
    else
        return CreateFNeg(aOperand,name);
}

Value * IRGenerator::CreateTypeConv(llvm::Value * aOperand, OpCodeEn opCode, TypeEn targetTy, const std::string &name)
{
    Type* destTy = getLLVMType(targetTy);

    Value * ret=nullptr;
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
    }
    return ret;
}

llvm::Value * IRGenerator::CreateBuiltInFunc(llvm::Value * aOperand, OpCodeEn opCode, const std::string &name)
{
    if (table == nullptr) return nullptr;
    if (aOperand->getType() == getFloatTy()) {
        return CreateCall(table->getFloatBIFunc(opCode), { aOperand }, name);
    }
    else {
        return CreateCall(table->getDoubleBIFunc(opCode), { aOperand }, name);
    }
}

Value * IRGenerator::CreateConvolve(llvm::Value * aOperand,  char * ptr,int64_t length, int64_t shift, TypeEn type, const std::string &name)
{
    auto function = table->getConvolveFunc(type);
    auto bOperand = CreateIntToPtr(
        ConstantInt::get(getInt64Ty(), uintptr_t(ptr)),
        PointerType::getUnqual(getLLVMType(type)));

    return CreateCall(function, { aOperand, bOperand, getInt64(length) ,getInt64(shift) }, name);
}


Value *IRGenerator::CreateCall_(Value *Callee, ArrayRef<Value *> Args ,const std::string & Name ) {
    MDNode *FPMathTag = nullptr;

    return CreateCall(
        cast<FunctionType>(Callee->getType()->getPointerElementType()), Callee,
        Args, Name, FPMathTag);
}

Value * IRGenerator::CreateConvolve(Value * aOperand, Value * bOperand, const std::string &name)
{
    Value * ret =nullptr, * convolveFunction=nullptr;
    Type * type=aOperand->getType()->getPointerElementType();

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
    auto function = table->getGPUConvolveFunc(type);
    return nullptr;
}

Value * IRGenerator::CreatePositionalAlloca(llvm::Type * ty, int64_t i, const std::string &name)
{
    SetInitInsertPoint();
    Value* ret = CreateAlloca(ty, 0, name);
    SetCalcInsertPoint();
    return ret;
}

Value * IRGenerator::CreatePositionalOffset( std::string name,int64_t startValue)
{
    Value* ret=CreatePositionalOffsetAlloca("", startValue);
    SetLoadInsertPoint();
    current_offset_value_=CreateLoad(ret, "offset" );
    SetCalcInsertPoint();
    return ret;
}


Value * IRGenerator::CreatePositionalOffsetAlloca(std::string name, int64_t startValue)
{
    auto ty=getInt64Ty();
    SetInitInsertPoint();
    Value* ret = CreateAlloca(ty, 0, "offset_alloca_");
    current_offset_value_alloca_=ret;
    CreateStore(getInt64(startValue), ret);
    return ret;
}

llvm::Value * IRGenerator::CreatePositionalInBoundsGEP(llvm::Value * Ptr, llvm::ArrayRef<llvm::Value*> IdxList, const std::string & name)
{
    SetLoadInsertPoint();
    Value* ret = CreateInBoundsGEP(Ptr, IdxList, name);
    SetCalcInsertPoint();
    return ret;
}

Value * IRGenerator::CreatePositionalLoad(llvm::Value * aOperand, const std::string &name)
{
    SetLoadInsertPoint();
    Value* ret = CreateLoad(aOperand, name);
    SetCalcInsertPoint();
    return ret;
}

Value * IRGenerator::CreatePositionalLoad(llvm::Value * aOperand, bool isVolatile, const std::string &name)
{
    SetLoadInsertPoint();
    Value* ret = CreateLoad(aOperand, isVolatile, name);
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

Value * IRGenerator::CreateBufferInit(TypeEn targetTy, const std::string &name)
{
    int numberOfBuffer = buffers_List_.size();
    std::string numberOfBufferTxt=std::to_string(numberOfBuffer);

    SetInitInsertPoint();
    Value* arg              = current_Function_->getArg(0);

    Value* untypedBufferPtr =CreateInBoundsGEP(
        getInt64Ty()->getPointerTo(),
        arg,
        getInt32(numberOfBuffer),
        name + "untyped_buffer_ptr_"+ numberOfBufferTxt);

    Value* untypedBuffer    = CreateLoad(
        untypedBufferPtr,
        true,
        name+"untyped_buffer_"+ numberOfBufferTxt);


    Value* buffer           = CreateBitCast(
        untypedBuffer,
        getLLVMType(targetTy)->getPointerTo(),
        name + "buffer_"+ numberOfBufferTxt);
    
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
    buffer_list_.push_back(s);
    return nullptr;
}

void IRGenerator::SetDeclareConvolve(llvm::Type * type, uintptr_t addr) //atavism
{
    typedef Value * (*lambla_ty)(llvm::Type * type, uintptr_t addr);

    auto  lambda =[this](llvm::Type * type, uintptr_t addr) {
        return CreateIntToPtr(
            ConstantInt::get(getInt64Ty(), uintptr_t(addr)),
            PointerType::getUnqual(
                FunctionType::get(type,
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
    BasicBlock*  tmp=BB;
    CreateStore(getInt64(0), current_offset_value_alloca_);
}

void IRGenerator::SetOffsetTo(int64_t val)
{
    BasicBlock*  tmp=BB;
    CreateStore(getInt64(val), current_offset_value_alloca_);
}

llvm::Type * IRGenerator::getLLVMType(TypeEn targetTy) {
    Type * ret = nullptr;
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