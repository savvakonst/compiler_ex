
#include "common.h"
#include "variable.h"


#define CONV_OP(depend,target) case (depend):  target ;  break
#define  SWITCH_OP(TYPE) switch(TYPE){\
    CONV_OP(TypeEn::Double_jty, OP(double));\
    CONV_OP(TypeEn::Float_jty, OP(float));\
    CONV_OP(TypeEn::Int64_jty, OP(int64_t));\
    CONV_OP(TypeEn::Int32_jty, OP(int32_t));\
    CONV_OP(TypeEn::Int16_jty, OP(int16_t));\
    CONV_OP(TypeEn::Int8_jty, OP(int8_t));\
    CONV_OP(TypeEn::Int1_jty, print_error(" Int1_jty "));\
    }


void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, void * a, void * b, int n) {
#define OP(T)   aritheticTemplate<T> (op,(T*)ret,(T*)a ,(T*)b, n)
    SWITCH_OP(targetType);
#undef OP
}

void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, void * a, int64_t  b, int n) {
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,(T*)a ,*((T*)(&b)), n)
    SWITCH_OP(targetType);
#undef OP
}

void calcAritheticSmallArray(opCodeEn op, TypeEn targetType, void * ret, int64_t  a, void *  b, int n) {
#define OP(T)   aritheticConstTemplate<T>(op,(T*)ret,*((T*)(&a)),(T*)b , n)
    SWITCH_OP(targetType);
#undef OP
}

void invAritheticSmallArray(TypeEn targetType, void * ret, void * a, int n) {
#define OP(T)   invTemplate<T>((T*)ret,(T*)a , n)
    SWITCH_OP(targetType);
#undef OP
}

void typeConvSmallArray(TypeEn retType, TypeEn argType, void * ret, void* arg, int n)
{

#define OP_LV2(T1,T2)   { \
	T1 * typedRet=(T1 *)ret;\
	T2 * typedArg=(T2 *)arg;\
	for (int i=0; i < n; i++){ typedRet[i] = (T1)typedArg[i];} \
    }

#define CONV_OP(depend,target) case (depend):  target ;  break

#define OP_LV1(T)   		switch (argType)  \
	{   \
		CONV_OP(TypeEn::Double_jty, OP_LV2(T,double)   ); \
		CONV_OP(TypeEn::Float_jty,  OP_LV2(T,float)    ); \
		CONV_OP(TypeEn::Int64_jty,  OP_LV2(T,int64_t)  ); \
		CONV_OP(TypeEn::Int32_jty,  OP_LV2(T,int32_t)  ); \
		CONV_OP(TypeEn::Int16_jty,  OP_LV2(T,int16_t)  ); \
		CONV_OP(TypeEn::Int8_jty,   OP_LV2(T,int8_t)   ); \
		CONV_OP(TypeEn::Int1_jty,   OP_LV2(T,bool)     ); \
	} 

	if (!isUnknownTy(argType)) {
		uint64_t V = 0;
		switch (retType)
		{
			CONV_OP(TypeEn::Double_jty, OP_LV1(double));
			CONV_OP(TypeEn::Float_jty, OP_LV1(float));
			CONV_OP(TypeEn::Int64_jty, OP_LV1(int64_t));
			CONV_OP(TypeEn::Int32_jty, OP_LV1(int32_t));
			CONV_OP(TypeEn::Int16_jty, OP_LV1(int16_t));
			CONV_OP(TypeEn::Int8_jty, OP_LV1(int8_t));
			CONV_OP(TypeEn::Int1_jty, OP_LV1(bool));
		}
	}
	else {

	}
#undef CONV_OP
#undef OP
}



#undef CONV_OP
#undef SWITCH_OP