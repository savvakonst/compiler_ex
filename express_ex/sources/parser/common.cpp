
#include "common.h"
#include "variable.h"


char * calcSmallArrayAlloc(TypeEn targetType, int N, char* ptr) {
	char * ret=nullptr;
#define OP(T)  ret = (char *) SmallArrayAlloc<T> (N,(T*)ptr)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char * calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, char * b, int n) {
	ret=calcSmallArrayAlloc(targetType,n,ret);
#define OP(T)   aritheticTemplate<T> (op,(T*)ret,(T*)a ,(T*)b, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, int64_t  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,(T*)a ,*((T*)(&b)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcAritheticSmallArray(OpCodeEn op, TypeEn targetType, char * ret, int64_t  a, char *  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,*((T*)(&a)),(T*)b , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char * calcComparsionSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, char * b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   comparsionTemplate<T> (op,(T*)ret,(T*)a ,(T*)b, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcComparsionSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, int64_t  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   comparsionConstTemplate<T>(op,(T*)ret,(T*)a ,*((T*)(&b)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcComparsionSmallArray(OpCodeEn op, TypeEn targetType, char * ret, int64_t  a, char *  b, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   comparsionConstTemplate<T>(op,(T*)ret,*((T*)(&a)),(T*)b , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}


char *  invAritheticSmallArray(TypeEn targetType, char * ret, char * a, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   invTemplate<T>((T*)ret,(T*)a , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  builtInFuncSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a,  int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   builtInFuncTemplate<T>(op,(T*)ret,(T*)a , n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}


char *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, char * b, char * c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,(T*)b,(T*)c, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, char * b, int64_t c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,(T*)b,*((T*)(&c)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, int64_t  b, char * c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)   selectTemplate<T> (op,(T*)ret,(bool*)a ,*((T*)(&b)),(T*)c, n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcSelectSmallArray(OpCodeEn op, TypeEn targetType, char * ret, char * a, int64_t b, int64_t c, int n) {
	ret=calcSmallArrayAlloc(targetType, n, ret);
#define OP(T)  selectTemplate<T> (op,(T*)ret,(bool*)a ,*((T*)(&b)),*((T*)(&c)), n)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  calcConvolveSmallArray(TypeEn targetType, char * ret, char * a, char * b, int aN, int bN) {
	ret=calcSmallArrayAlloc(targetType, maxInt(aN,bN), ret);
#define OP(T)  convolveTemplate_<T> ((T*)ret,(T*)a,(T*)b, aN,bN)
	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}

char *  typeConvSmallArray(TypeEn retType, TypeEn argType, char * ret, char* arg, int n)
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

char * calcSmallArrayDef(TypeEn targetType, std::vector<Variable*> &operand) {

	char * ret=calcSmallArrayAlloc(targetType,(int)operand.size());

#define OP(T)	{	\
	T* ptr=(T*)ret;	\
	for (auto &j : operand)  { \
		int64_t v =j->getAssignedVal(true)->getBinaryValue(); \
		*ptr++ = *((T*)(&v)); }}

	SWITCH_TYPE_OP(targetType, ;);
#undef OP
	return ret;
}


