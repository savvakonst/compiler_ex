////////////////////////////////////////////////////////////////
/// naming convention
/// if you want declare some variable for debugging purposes, you should name it like this _debug_var_<name of variable>.
/// it will allow you to easily find debug variables before release to remove them.
/// 
////////////////////////////////////////////////////////////////


#include "operations.h"
#include <string>
#include <math.h>


Variable* newInvOperation(GarbageContainer* garbage_container, Variable* arg) {
    print_error("inverse operation is not supported yet");
    return arg;
}

Variable* newTypeConvOp(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg)
{
    const TypeEn arg_type = arg->getType();
    Variable *ret_val = nullptr;

#define OP_LV2(T1,T2)        *((T1*)(&V)) = (T1 ) arg->getConvTypeVal<T2>()
#define CONV_OP(depend,target) case (depend):  target ;  break

#define OP_LV1(T)           switch (arg_type)  \
    {   \
        CONV_OP(TypeEn::double_jty, OP_LV2(T,double)   ); \
        CONV_OP(TypeEn::float_jty,  OP_LV2(T,float)    ); \
        CONV_OP(TypeEn::int64_jty,  OP_LV2(T,int64_t)  ); \
        CONV_OP(TypeEn::int32_jty,  OP_LV2(T,int32_t)  ); \
        CONV_OP(TypeEn::int16_jty,  OP_LV2(T,int16_t)  ); \
        CONV_OP(TypeEn::int8_jty,   OP_LV2(T,int8_t)   ); \
        CONV_OP(TypeEn::int1_jty,   OP_LV2(T,bool)     ); \
    } 
    if (!isUnknownTy(arg_type) && isConst(arg) ) {
        uint64_t V = 0;
        switch (target_type)
        {
            CONV_OP(TypeEn::double_jty, OP_LV1(double));
            CONV_OP(TypeEn::float_jty, OP_LV1(float));
            CONV_OP(TypeEn::int64_jty, OP_LV1(int64_t));
            CONV_OP(TypeEn::int32_jty, OP_LV1(int32_t));
            CONV_OP(TypeEn::int16_jty, OP_LV1(int16_t));
            CONV_OP(TypeEn::int8_jty, OP_LV1(int8_t));
            CONV_OP(TypeEn::int1_jty, OP_LV1(bool));
        }
        ret_val = new Variable(V, target_type);
        return garbage_container->add(ret_val);
    }
#undef CONV_OP
#undef OP

    if ( (target_type == arg_type) || isUnknownTy(arg_type) ) {
        return arg; 
    }
    else if (isFloating(target_type) && isFloating(arg_type)) {
        if (target_type < arg_type)
            ret_val= new Operation(OpCodeEn::fptrunc, arg, target_type);
        else
            ret_val= new Operation(OpCodeEn::fpext, arg, target_type);
    }
    else if (isFloating(target_type) && isInteger(arg_type)) {
        ret_val= new Operation(OpCodeEn::sitofp, arg, target_type);;
    }
    else if (isInteger(target_type) && isFloating(arg_type)) {
        ret_val= new Operation(OpCodeEn::fptosi, arg, target_type);
    }
    else if (isInteger(target_type) && isInteger(arg_type))
    {
        if (target_type < arg_type)
            ret_val= new Operation(OpCodeEn::trunc, arg, target_type);
        else
            ret_val= new Operation(OpCodeEn::sext, arg, target_type);
    }
    else { 
        print_error("newTypeConvOp"); 
    }

    return garbage_container->add(ret_val);
}

Variable* newBuiltInFuncOperation(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg, OpCodeEn type_op) {

    Variable* var = arg;

    if (TypeEn::float_jty > target_type) {
        var = newTypeConvOp(garbage_container,TypeEn::double_jty, arg);
        target_type=var->getType();
    }

#define CONV_OP(depend,target) case (depend):  (target) ;  break
#define OP(T)   calcBuiltInFuncOperation<T> ( var->getConvTypeVal<T>(),type_op )
    if (isConst(var) && !isUnknownTy(target_type)) {
        uint64_t V=0;
        switch (target_type) {
            CONV_OP(TypeEn::double_jty, V =OP(double));
            CONV_OP(TypeEn::float_jty, V =OP(float));
        default: print_error("type is not float");
        }
        return garbage_container->add(new Variable(V, target_type));
    }
#undef CONV_OP
#undef OP

    return garbage_container->add(new Operation(type_op, var, target_type));;
}

Variable* newArithmeticOperation(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg_a, Variable* arg_b, OpCodeEn type_op) {

    OpCodeEn opType = OpCodeEn::add;
    
    if (!is—ompatible(arg_a, arg_b))
        print_error("uncompatible values");

#define CONV_OP(depend,target) case (depend):  (target) ;  break
#define OP(T)   calcArithmeticOperation<T> ( arg_a->getConvTypeVal<T>(),arg_b->getConvTypeVal<T>(),type_op )
    if (isConst(arg_a) && isConst(arg_b) && !isUnknownTy(target_type)) {
        uint64_t V=0;
        switch (target_type)
        {
            CONV_OP(TypeEn::double_jty, V =OP(double));
            CONV_OP(TypeEn::float_jty, V =OP(float));
            CONV_OP(TypeEn::int64_jty, V =OP(int64_t));
            CONV_OP(TypeEn::int32_jty, V =OP(int32_t));
            CONV_OP(TypeEn::int16_jty, V =OP(int16_t));
            CONV_OP(TypeEn::int8_jty, V =OP(int8_t));
            CONV_OP(TypeEn::int1_jty, print_error("invalid type: Int1_jty "));
        }
        return garbage_container->add(new Variable(V, target_type));
    }
#undef CONV_OP
#undef OP
    
#define CONV_OP(depend,target) case (depend):  opType=(target) ;  break
    if (isInteger(target_type) || isUnknownTy(target_type)) {
        opType=type_op;
    }
    else if (isFloating(target_type)) {
        switch (type_op)
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
    return garbage_container->add(new Operation(opType, arg_a, arg_b));
}


Variable* newComparsionOperation(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg_a, Variable* arg_b, OpCodeEn type_op) {

    OpCodeEn opType = OpCodeEn::add;

    if (!is—ompatible(arg_a, arg_b))
        print_error("uncompatible values");

#define CONV_OP(depend,target) case (depend):  (target) ;  break
#define OP(T)   calcComparsionOperation<T> ( arg_a->getConvTypeVal<T>(),arg_b->getConvTypeVal<T>(),type_op )
    if (isConst(arg_a) && isConst(arg_b) && !isUnknownTy(target_type)) {
        uint64_t V = 0;
        switch (target_type)
        {
            CONV_OP(TypeEn::double_jty, V =OP(double));
            CONV_OP(TypeEn::float_jty, V =OP(float));
            CONV_OP(TypeEn::int64_jty, V =OP(int64_t));
            CONV_OP(TypeEn::int32_jty, V =OP(int32_t));
            CONV_OP(TypeEn::int16_jty, V =OP(int16_t));
            CONV_OP(TypeEn::int8_jty, V =OP(int8_t));
            CONV_OP(TypeEn::int1_jty, print_error(" Int1_jty "));
        }
        return garbage_container->add(new Variable(V, TypeEn::int1_jty));
    }
#undef CONV_OP
#undef OP

#define CONV_OP(depend,target) case (depend):  opType=(target) ;  break
    if (isInteger(target_type) || isUnknownTy(target_type)) {
        opType=type_op;
    }
    else if (isFloating(target_type)) {
        switch (type_op)
        {
            CONV_OP(OpCodeEn::eq,  OpCodeEn::oeq);
            CONV_OP(OpCodeEn::ne,  OpCodeEn::one);
            CONV_OP(OpCodeEn::sgt, OpCodeEn::ogt);
            CONV_OP(OpCodeEn::sge, OpCodeEn::oge);
            CONV_OP(OpCodeEn::slt, OpCodeEn::olt);
            CONV_OP(OpCodeEn::sle, OpCodeEn::ole);
        }
    }
    else {
        print_error("newComparsionOperation - unsigned is not supported yet");
    }
#undef CONV_OP
    return garbage_container->add(new Operation(opType, arg_a, arg_b));
}


Variable* newConvolveOperation(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg_a, Variable* arg_b, uint32_t shift, OpCodeEn type_op) {

    if (type_op != OpCodeEn::convolve)
        print_error("convolve_f operation is not supported yet");

    if ((isConst(arg_a) || isConst(arg_b) )&& !(isUnknownTy(arg_a) || isUnknownTy(arg_b))) {
        return garbage_container->add(newArithmeticOperation(garbage_container,target_type, arg_a, arg_b, OpCodeEn::mul));
    }
    else if (isSmallArr(arg_a) && isSmallArr(arg_b)) {
        return garbage_container->add(new Operation(type_op, arg_a, arg_b, shift));
        //print_error("convolve(SmallArr_t,SmallArr_t) - is not supported yet");
    }
    else if (isSmallArr(arg_a) || isSmallArr(arg_b)) {
        if (isSmallArr(arg_b))
            return garbage_container->add(new Operation(type_op, arg_a, arg_b, shift));
        else
            return garbage_container->add(new Operation(type_op, arg_b, arg_a, shift));
    }
    else if (isLargeArr(arg_a) && isLargeArr(arg_b)) {
        print_error("convolve(LargeArr_t,LargeArr_t) - is not supported");
    }

    return garbage_container->add(new Operation(type_op, arg_a, arg_b));
}


Variable* newSelectOp(GarbageContainer* garbage_container, TypeEn target_type, Variable* arg_a, Variable* arg_b, Variable* arg_c ,bool rec_call){

    if (!is—ompatible(arg_b, arg_c) || !is—ompatible(arg_a, arg_b))
        print_error("uncompatible values");

    auto i1=newTypeConvOp(garbage_container,TypeEn::int1_jty, arg_a);
    
    if (isConst(i1) && !isUnknownTy(i1))
        if (i1->getConvTypeVal<bool>())
            return arg_b;
        else
            return arg_c;

    return garbage_container->add(new Operation(OpCodeEn::select, i1, arg_b, arg_c, target_type, rec_call));
}

Variable* newSliceOp(GarbageContainer* garbage_container, Variable* arg_a, Variable* arg_b, OpCodeEn type_op) {

    if (!(isConst(arg_b) && isInteger(arg_b)))
        print_error("(arr,int) - second argument must be integer consant");
    int64_t intVal =arg_b->getBinaryValue();

    return garbage_container->add(new Operation(type_op, arg_a, arg_a->getType(), intVal));
}

Variable* newSliceOp(GarbageContainer* garbage_container, Variable* arg_a, int64_t intVal, OpCodeEn uTypeOp) {

    return garbage_container->add(new Operation(uTypeOp, arg_a, arg_a->getType(), intVal));
}

Variable* newSmallArrayDefOp(GarbageContainer* garbage_container, stack<Variable*> &args, OpCodeEn type_op, bool is_prototype) {

    if (args.empty())
        print_error("SmallArray is empty");

    Variable* var=args[0];
    
    bool  allIsConst =true;
    for (auto i : args) {
        var=maxTypeVar(i, var);
        allIsConst &= isConst(var);
    }

    if (!allIsConst)
        print_error("array concatenation is not supported yet");

    TypeEn targertType =var->getType();
    
    if (isUnknownTy(targertType) || type_op == OpCodeEn::smallArrayRange)
        return garbage_container->add(new Operation(type_op, args, targertType));

    if (is_prototype)
        return garbage_container->add(new Operation(OpCodeEn::smallArrayDef, args, targertType));

    stack<Variable*> typedArgs;
    for (auto i : args)
        typedArgs.push(newTypeConvOp(garbage_container,targertType, i));

    return garbage_container->add(new Operation(OpCodeEn::smallArrayDef, typedArgs, targertType));
}

