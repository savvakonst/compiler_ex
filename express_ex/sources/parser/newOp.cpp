
#include "operations.h"
#include <string>
#include <math.h>


//void print_error(std::string content);

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
		CONV_OP(TypeEn::double_jty, OP_LV2(T,double)   ); \
		CONV_OP(TypeEn::float_jty,  OP_LV2(T,float)    ); \
		CONV_OP(TypeEn::int64_jty,  OP_LV2(T,int64_t)  ); \
		CONV_OP(TypeEn::int32_jty,  OP_LV2(T,int32_t)  ); \
		CONV_OP(TypeEn::int16_jty,  OP_LV2(T,int16_t)  ); \
		CONV_OP(TypeEn::int8_jty,   OP_LV2(T,int8_t)   ); \
		CONV_OP(TypeEn::int1_jty,   OP_LV2(T,bool)     ); \
	} 

	if (!isUnknownTy(arg1) && isConst(arg1)  ) {
		uint64_t V = 0;
		switch (targetType)
		{
			CONV_OP(TypeEn::double_jty, OP_LV1(double));
			CONV_OP(TypeEn::float_jty, OP_LV1(float));
			CONV_OP(TypeEn::int64_jty, OP_LV1(int64_t));
			CONV_OP(TypeEn::int32_jty, OP_LV1(int32_t));
			CONV_OP(TypeEn::int16_jty, OP_LV1(int16_t));
			CONV_OP(TypeEn::int8_jty, OP_LV1(int8_t));
			CONV_OP(TypeEn::int1_jty, OP_LV1(bool));
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
			return new Operation(OpCodeEn::fptrunc, arg1, targetType);
		else
			return new Operation(OpCodeEn::fpext, arg1, targetType);
	}
	else if (isFloating(targetType) && isInteger(arg1)) {
		return new Operation(OpCodeEn::sitofp, arg1, targetType);;
	}
	else if (isInteger(targetType) && isFloating(arg1)) {
		return new Operation(OpCodeEn::fptosi, arg1, targetType);
	}
	else if (isInteger(targetType) && isInteger(arg1))
	{
		if (targetType < arg1)
			return new Operation(OpCodeEn::trunc, arg1, targetType);
		else
			return new Operation(OpCodeEn::sext, arg1, targetType);
	}
	else print_error("newTypeConvOp");
}

//#include "llvm/Support/raw_ostream.h"

Variable* newBuiltInFuncOperation(TypeEn targetType, Variable* arg, OpCodeEn uTypeOp) {

	Variable* var=arg;

	if (TypeEn::float_jty > targetType) {
		var = newTypeConvOp(TypeEn::double_jty, arg);
		targetType=var->getType();
	}


	#define CONV_OP(depend,target) case (depend):  (target) ;  break
	#define OP(T)   calcBuiltInFuncOperation<T> ( var->getConvTypeVal<T>(),uTypeOp )
	if (isConst(var) && !isUnknownTy(targetType)) {
		uint64_t V=0;
		switch (targetType) {
			CONV_OP(TypeEn::double_jty, V =OP(double));
			CONV_OP(TypeEn::float_jty, V =OP(float));
		default: print_error("type is not float");
		}
		return new Variable(V, targetType);
	}
#undef CONV_OP
#undef OP
	return new Operation(uTypeOp, var, targetType);
}

Variable* newArithmeticOperation(TypeEn targetType, Variable* arg1, Variable* arg2, OpCodeEn uTypeOp) {

	OpCodeEn opType = OpCodeEn::add;
	

	if (!is—ompatible(arg1, arg2))
		print_error("uncompatible values");

#define CONV_OP(depend,target) case (depend):  (target) ;  break
#define OP(T)   calcArithmeticOperation<T> ( arg1->getConvTypeVal<T>(),arg2->getConvTypeVal<T>(),uTypeOp )
	if (isConst(arg1) && isConst(arg2) && !isUnknownTy(targetType)) {
		uint64_t V=0;
		switch (targetType)
		{
			CONV_OP(TypeEn::double_jty, V =OP(double));
			CONV_OP(TypeEn::float_jty, V =OP(float));
			CONV_OP(TypeEn::int64_jty, V =OP(int64_t));
			CONV_OP(TypeEn::int32_jty, V =OP(int32_t));
			CONV_OP(TypeEn::int16_jty, V =OP(int16_t));
			CONV_OP(TypeEn::int8_jty, V =OP(int8_t));
			CONV_OP(TypeEn::int1_jty, print_error(" Int1_jty "));
		}
		return new Variable(V, targetType);
	}
#undef CONV_OP
#undef OP
	
#define CONV_OP(depend,target) case (depend):  opType=(target) ;  break
	if (isInteger(targetType) || isUnknownTy(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(OpCodeEn::add, OpCodeEn::add);
			CONV_OP(OpCodeEn::sub, OpCodeEn::sub);
			CONV_OP(OpCodeEn::mul, OpCodeEn::mul);
			CONV_OP(OpCodeEn::sdiv, OpCodeEn::sdiv);
			CONV_OP(OpCodeEn::srem, OpCodeEn::srem);
			CONV_OP(OpCodeEn::pow, OpCodeEn::pow);
		}
	}
	else if (isFloating(targetType)) {
		switch (uTypeOp)
		{
			CONV_OP(OpCodeEn::add, OpCodeEn::fadd);
			CONV_OP(OpCodeEn::sub, OpCodeEn::fsub);
			CONV_OP(OpCodeEn::mul, OpCodeEn::fmul);
			CONV_OP(OpCodeEn::sdiv, OpCodeEn::fdiv);
			CONV_OP(OpCodeEn::srem, OpCodeEn::frem);
			CONV_OP(OpCodeEn::pow, OpCodeEn::fpow);
		}
	}
	else {
		print_error("newArithmeticOperation - unsigned is not supported yet");

	}
#undef CONV_OP
	return new Operation(opType, arg1, arg2);
}


Variable* newConvolveOperation(TypeEn targetType, Variable* arg1, Variable* arg2, uint32_t shift, OpCodeEn uTypeOp) {

	if (uTypeOp != OpCodeEn::convolve)
		print_error("convolve_f operation is not supported yet");

	if (isConst(arg1) || isConst(arg2)) {
		return newArithmeticOperation(targetType, arg1, arg2, OpCodeEn::mul);
	}
	else if (isSmallArr(arg1) && isSmallArr(arg2)) {
		return new Operation(uTypeOp, arg1, arg2, shift);
		//print_error("convolve(SmallArr_t,SmallArr_t) - is not supported yet");
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


Variable* newSelectOp(TypeEn targetType, Variable* arg1, Variable* arg2, Variable* arg3){

	if (!is—ompatible(arg2, arg3) || !is—ompatible(arg1, arg2))
		print_error("uncompatible values");

	auto i1=newTypeConvOp(TypeEn::int1_jty, arg1);
	if (isConst(i1))
		if (i1->getConvTypeVal<bool>())
			return arg2;
		else
			return arg3;

	return new Operation(OpCodeEn::select, i1, arg2, arg3, targetType);
}

Variable* newSliceOp(Variable* arg1, Variable* arg2, OpCodeEn uTypeOp) {
	if (!(isConst(arg2) && isInteger(arg2)))
		print_error("(arr,int) - second argument must be integer consant");
	int64_t intVal =arg2->getBinaryValue();
	return new Operation(uTypeOp, arg1, arg1->getType(), intVal);
}

Variable* newSliceOp(Variable* arg1, int64_t intVal, OpCodeEn uTypeOp) {
	return new Operation(uTypeOp, arg1, arg1->getType(), intVal);
}

Variable* newSmallArrayDefOp(stack<Variable*> &args, OpCodeEn uTypeOp) {
	if (args.empty())
		print_error("SmallArray is empty");

	Variable* var=args[0];
	
	bool  allIsConst =true;
	for (auto i : args) {
		var=max(i, var);
		allIsConst &= isConst(var);
	}

	if (!allIsConst)
		print_error("array concatenation is not supported yet");

	TypeEn targertType =var->getType();
	
	if (isUnknownTy(targertType) || uTypeOp == OpCodeEn::smallArrayRange)
		return new Operation(uTypeOp, args, targertType);

	stack<Variable*> typedArgs; 
	for (auto i : args)
		typedArgs.push(newTypeConvOp(targertType, i));

	return new Operation(OpCodeEn::smallArrayDef, typedArgs, targertType);
}

