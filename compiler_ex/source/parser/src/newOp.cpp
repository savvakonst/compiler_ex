
#include "operations.h"
#include <string>


void print_error(std::string content);

Variable* newInvOperation(Variable* arg1) {
	print_error("inverse operation is not supported yet");
	return arg1;
}

Variable* newTypeConvOp(TypeEn targetType, Variable* arg1)
{

#define OP_LV2(T1,T2)   	 *((T1*)(&V)) = (T1 )arg1->getConvTypeVal<T2>()
#define CONV_OP(depend,target) case (depend):  target ;  break

#define OP_LV1(T)   		switch (arg1->getType())  \
	{   \
		CONV_OP(TypeEn::Double_jty, OP_LV2(T,double)   ); \
		CONV_OP(TypeEn::Float_jty,  OP_LV2(T,float)    ); \
		CONV_OP(TypeEn::Int64_jty,  OP_LV2(T,int64_t)  ); \
		CONV_OP(TypeEn::Int32_jty,  OP_LV2(T,int32_t)  ); \
		CONV_OP(TypeEn::Int16_jty,  OP_LV2(T,int16_t)  ); \
		CONV_OP(TypeEn::Int8_jty,   OP_LV2(T,int8_t)   ); \
		CONV_OP(TypeEn::Int1_jty,   OP_LV2(T,bool)     ); \
	} 

	if (isConst(arg1) && !isUnknownTy(targetType)) {
		uint64_t V = 0;
		switch (targetType)
		{
			CONV_OP(TypeEn::Double_jty, OP_LV1(double));
			CONV_OP(TypeEn::Float_jty, OP_LV1(float));
			CONV_OP(TypeEn::Int64_jty, OP_LV1(int64_t));
			CONV_OP(TypeEn::Int32_jty, OP_LV1(int32_t));
			CONV_OP(TypeEn::Int16_jty, OP_LV1(int16_t));
			CONV_OP(TypeEn::Int8_jty, OP_LV1(int8_t));
			CONV_OP(TypeEn::Int1_jty, OP_LV1(bool));
		}
		return new Variable(V, targetType);
	}
#undef CONV_OP
#undef OP



	if (targetType == arg1->getType()) {
		return arg1;
	}
	else if (isFloating(targetType) && isFloating(arg1)) {
		if (targetType < arg1)
			return new Operation(opCodeEn::fptrunc, arg1, targetType);
		else
			return new Operation(opCodeEn::fpext, arg1, targetType);
	}
	else if (isFloating(targetType) && isInteger(arg1)) {
		return new Operation(opCodeEn::sitofp, arg1, targetType);;
	}
	else if (isInteger(targetType) && isFloating(arg1)) {
		return new Operation(opCodeEn::fptosi, arg1, targetType);
	}
	else //if (isInteger(targetType) && isFloating(arg1)) 
	{

		if (targetType < arg1)
			return new Operation(opCodeEn::trunc, arg1, targetType);
		else
			return new Operation(opCodeEn::sext, arg1, targetType);
	}
}

Variable* newBuiltInFuncOperation(TypeEn targetType, Variable* arg1, opCodeEn uTypeOp) {
	return new Operation(uTypeOp, arg1, targetType);
}

Variable* newArithmeticOperation(TypeEn targetType, Variable* arg1, Variable* arg2, opCodeEn uTypeOp) {

	opCodeEn opType = opCodeEn::ADD;

	if (!isÑompatible(arg1, arg2))
		print_error("uncompatible values");


#define CONV_OP(depend,target) case (depend):  (target) ;  break
#define OP(T)   calcArithmeticOperation<T> ( arg1->getConvTypeVal<T>(),arg2->getConvTypeVal<T>(),uTypeOp )
	if (isConst(arg1) && isConst(arg2) && !isUnknownTy(targetType)) {
		uint64_t V=0;
		switch (targetType)
		{
			CONV_OP(TypeEn::Double_jty, V =OP(double));
			CONV_OP(TypeEn::Float_jty, V =OP(float));
			CONV_OP(TypeEn::Int64_jty, V =OP(int64_t));
			CONV_OP(TypeEn::Int32_jty, V =OP(int32_t));
			CONV_OP(TypeEn::Int16_jty, V =OP(int16_t));
			CONV_OP(TypeEn::Int8_jty, V =OP(int8_t));
			CONV_OP(TypeEn::Int1_jty, print_error(" Int1_jty "));
		}
		return new Variable(V, targetType);
	}
#undef CONV_OP
#undef OP

#define CONV_OP(depend,target) case (depend):  opType=(target) ;  break
	if (isInteger(targetType) || isUnknownTy(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(opCodeEn::ADD, opCodeEn::ADD);
			CONV_OP(opCodeEn::SUB, opCodeEn::SUB);
			CONV_OP(opCodeEn::MUL, opCodeEn::MUL);
			CONV_OP(opCodeEn::SDIV, opCodeEn::SDIV);
			CONV_OP(opCodeEn::SREM, opCodeEn::SREM);
			CONV_OP(opCodeEn::POW, opCodeEn::POW);
		}
	}
	else if (isFloating(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(opCodeEn::ADD, opCodeEn::FADD);
			CONV_OP(opCodeEn::SUB, opCodeEn::FSUB);
			CONV_OP(opCodeEn::MUL, opCodeEn::FMUL);
			CONV_OP(opCodeEn::SDIV, opCodeEn::FDIV);
			CONV_OP(opCodeEn::SREM, opCodeEn::FREM);
			CONV_OP(opCodeEn::POW, opCodeEn::FPOW);
		}
	}
	else {
		print_error("newArithmeticOperation - unsigned is not supported yet");

	}
#undef CONV_OP
	return new Operation(opType, arg1, arg2);
}


Variable* newConvolveOperation(TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift, opCodeEn uTypeOp) {


	if (uTypeOp != opCodeEn::convolve)
		print_error("convolve_f operation is not supported yet");

	if (isConst(arg1) || isConst(arg2)) {
		return newArithmeticOperation(targetType, arg1, arg2, opCodeEn::MUL);
	}
	else if (isSmallArr(arg1) && isSmallArr(arg2)) {
		print_error("convolve(SmallArr_t,SmallArr_t) - is not supported yet");
	}
	else if (isSmallArr(arg1) || isSmallArr(arg2)) {
		if (isSmallArr(arg2))
			return new Operation(uTypeOp, arg1, arg2, shift);
		else
			return new Operation(uTypeOp, arg2, arg1, shift);
	}
	else if (isLargeArr(arg1) && isLargeArr(arg2)) {
		print_error("convolve(LargeArr_t,LargeArr_t) - is not supported");
	}


	return new Operation(uTypeOp, arg1, arg2);
}


Variable* newSelectOp(TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3)
{
	if (!isÑompatible(arg2, arg3) || !isÑompatible(arg1, arg2))
		print_error("uncompatible values");

	auto i1=newTypeConvOp(TypeEn::Int1_jty, arg1);
	if (isConst(i1))
		if (i1->getConvTypeVal<bool>())
			return arg2;
		else
			return arg3;

	return new Operation(opCodeEn::select, i1, arg2, arg3, targetType);
}

Variable* newSliceOp(Variable* arg1, Variable* arg2, opCodeEn uTypeOp) {
	if (!(isConst(arg2) && isInteger(arg2)))
		print_error("(arr,int) - second argument must be integer consant");
	int64_t intVal =arg2->getBinaryValue();
	return new Operation(uTypeOp, arg1, arg1->getType(), intVal);
}

Variable* newSliceOp(Variable* arg1, int64_t intVal, opCodeEn uTypeOp) {
	return new Operation(uTypeOp, arg1, arg1->getType(), intVal);
}