
#include "common.h"
#include "variable.h"





void * calcSmallArrayAlloc(TypeEn targetType, int N, void* ptr) {
	void * ret=NULL;
#define OP(T)  ret = (void *) SmallArrayAlloc<T> (N,(T*)ptr)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void * calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int n) {
	ret=calcSmallArrayAlloc(targetType,n,ret);
#define OP(T)   aritheticTemplate<T> (op,(T*)ret,(T*)a ,(T*)b, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,(T*)a ,*((T*)(&b)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, void * ret, int64_t  a, void *  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,*((T*)(&a)),(T*)b , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  invAritheticSmallArray(TypeEn targetType, void * ret, void * a, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   invTemplate<T>((T*)ret,(T*)a , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  builtInFuncSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a,  int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   builtInFuncTemplate<T>(op,(T*)ret,(T*)a , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}


void *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, void * c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,(T*)b,(T*)c, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int64_t c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,(T*)b,*((T*)(&c)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, void * c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,*((T*)(&b)),(T*)c, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, void * ret, void * a, int64_t b, int64_t c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)  selectTemplate<T> (op,(T*)ret,(bool*)a ,*((T*)(&b)),*((T*)(&c)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  calcConvolveSmallArray(TypeEn targetType, void * ret, void * a, void * b, int aN, int bN) {
	ret=calcSmallArrayAlloc(targetType, maxInt(aN,bN), ret);
#define OP(T)  convolveTemplate_<T> ((T*)ret,(T*)a,(T*)b, aN,bN)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

void *  typeConvSmallArray(TypeEn retType, TypeEn argType, void * ret, void* arg, int n)
{
	ret=calcSmallArrayAlloc(retType, n, ret);

#define OP_LV2(T1,T2)   {		\
	T1 * typedRet=(T1 *)ret;	\
	T2 * typedArg=(T2 *)arg;	\
	for (int i=0; i < n; i++){ typedRet[i] = (T1)typedArg[i];} \
    }



#define OP_LV1(T)   		switch (argType)  \
	{   \
		CONV_TYPE_OP(TypeEn::double_jty, OP_LV2(T,double)   ); \
		CONV_TYPE_OP(TypeEn::float_jty,  OP_LV2(T,float)    ); \
		CONV_TYPE_OP(TypeEn::int64_jty,  OP_LV2(T,int64_t)  ); \
		CONV_TYPE_OP(TypeEn::int32_jty,  OP_LV2(T,int32_t)  ); \
		CONV_TYPE_OP(TypeEn::int16_jty,  OP_LV2(T,int16_t)  ); \
		CONV_TYPE_OP(TypeEn::int8_jty,   OP_LV2(T,int8_t)   ); \
		CONV_TYPE_OP(TypeEn::int1_jty,   OP_LV2(T,bool)     ); \
	} 

	if (!isUnknownTy(argType)) {
		uint64_t V = 0;
		switch (retType)
		{
			CONV_TYPE_OP(TypeEn::double_jty, OP_LV1(double));
			CONV_TYPE_OP(TypeEn::float_jty, OP_LV1(float));
			CONV_TYPE_OP(TypeEn::int64_jty, OP_LV1(int64_t));
			CONV_TYPE_OP(TypeEn::int32_jty, OP_LV1(int32_t));
			CONV_TYPE_OP(TypeEn::int16_jty, OP_LV1(int16_t));
			CONV_TYPE_OP(TypeEn::int8_jty, OP_LV1(int8_t));
			CONV_TYPE_OP(TypeEn::int1_jty, OP_LV1(bool));
		}
	}
	else {

	}
#undef OP_LV2
#undef OP_LV1
	return ret;
}

void * calcSmallArrayDef(TypeEn targetType, std::vector<Variable*> &operand) {

	void * ret=calcSmallArrayAlloc(targetType, operand.size());

#define OP(T)	{	\
	T* ptr=(T*)ret;	\
	for (auto &j : operand)  {		\
		uint64_t v =j->getAssignedVal(true)->getBinaryValue(); \
		*ptr++=*((T*)(&v)); }}

	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}


