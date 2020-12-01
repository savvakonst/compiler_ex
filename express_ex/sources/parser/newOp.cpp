////////////////////////////////////////////////////////////////
/// naming convention
/// if you want declare some value for debugging purposes, you should name it like this _debug_var_<name of value>.
/// it will allow you to easily find debug values before release to remove them.
/// 
////////////////////////////////////////////////////////////////


#include "operations.h"
#include <string>
#include <math.h>


Value* newInvOperation(GarbageContainer* garbage_container, Value* arg) {
    print_error("inverse operation is not supported yet");
    return arg;
}

Value* newTypeConvOp(GarbageContainer* garbage_container, TypeEn target_type, Value* arg)
{
    const TypeEn arg_type = arg->getType();
    Value *ret_val = nullptr;

    if (!isUnknownTy(arg_type) && isConst(arg) ) {
        ret_val = new Value(calcTypeConvConst(target_type, arg_type, arg->getBinaryValue()), target_type);
        return garbage_container->add(ret_val);
    }

    if((target_type == arg_type) || isUnknownTy(arg_type)){
        return arg;
    }
    else if(isFloating(target_type) && isFloating(arg_type)){
        if(target_type < arg_type)
            ret_val= new Operation(OpCodeEn::fptrunc, arg, target_type);
        else
            ret_val= new Operation(OpCodeEn::fpext, arg, target_type);
    }
    else if(isFloating(target_type) && isInteger(arg_type)){
        ret_val= new Operation(OpCodeEn::sitofp, arg, target_type);;
    }
    else if(isInteger(target_type) && isFloating(arg_type)){
        ret_val= new Operation(OpCodeEn::fptosi, arg, target_type);
    }
    else if(isInteger(target_type) && isInteger(arg_type)){
        if(target_type < arg_type)
            ret_val= new Operation(OpCodeEn::trunc, arg, target_type);
        else
            ret_val= new Operation(OpCodeEn::sext, arg, target_type);
    }
    else{
        print_error("newTypeConvOp");
    }

    return garbage_container->add(ret_val);
}

Value* newBuiltInFuncOperation(GarbageContainer* garbage_container, TypeEn target_type, Value* arg, OpCodeEn op_type) {

    Value* var = arg;

    if (TypeEn::float_jty > target_type) {
        var = newTypeConvOp(garbage_container,TypeEn::double_jty, arg);
        target_type=var->getType();
    }

    if (isConst(var) && !isUnknownTy(target_type)) {
        if (!isFloating(target_type))
            print_error("type is not float");

        return garbage_container->add(
            new Value(
                calcBuiltInFuncConst(op_type, target_type, var->getBinaryValue()), 
                target_type
            ));
    }

    return garbage_container->add(new Operation(op_type, var, target_type));;
}

Value* newArithmeticOperation(GarbageContainer* garbage_container, TypeEn target_type, Value* arg_a, Value* arg_b, OpCodeEn op_type) {

    if (!isÑompatible(arg_a, arg_b))
        print_error("uncompatible values");

    if (isConst(arg_a) && isConst(arg_b) && !isUnknownTy(target_type)) {
        if(isBool(target_type))
            print_error("invalid type: Int1_jty ");

        return garbage_container->add(
            new Value(
                calcAritheticConst(op_type, target_type, arg_a->getBinaryValue(), arg_b->getBinaryValue()), 
                target_type
            ));
    }

    OpCodeEn local_op_type = OpCodeEn::none_op;

    if (isInteger(target_type) || isUnknownTy(target_type)) {
        local_op_type=op_type;
    }
    else if (isFloating(target_type)) {
        switch (op_type)
        {
        case(OpCodeEn::add): local_op_type = OpCodeEn::fadd; break;
        case(OpCodeEn::sub): local_op_type = OpCodeEn::fsub; break;
        case(OpCodeEn::mul): local_op_type = OpCodeEn::fmul; break;
        case(OpCodeEn::sdiv): local_op_type = OpCodeEn::fdiv; break;
        case(OpCodeEn::srem): local_op_type = OpCodeEn::frem; break;
        case(OpCodeEn::pow): local_op_type = OpCodeEn::fpow; break;
        }
    }
    else {
        print_error("newArithmeticOperation - unsigned is not supported yet");
    }

    return garbage_container->add(new Operation(local_op_type, arg_a, arg_b));
}


Value* newComparsionOperation(GarbageContainer* garbage_container, TypeEn target_type, Value* arg_a, Value* arg_b, OpCodeEn op_type) {

    if (!isÑompatible(arg_a, arg_b))
        print_error("uncompatible values");

    if (isConst(arg_a) && isConst(arg_b) && !isUnknownTy(target_type)) {

        if(isBool(target_type))
            print_error("invalid type: Int1_jty ");

        return garbage_container->add(
            new Value(
                calcComparsionConst(op_type, target_type, arg_a->getBinaryValue(), arg_b->getBinaryValue()),
                TypeEn::int1_jty
            ));
    }

    OpCodeEn local_op_type = OpCodeEn::none_op;

    if (isInteger(target_type) || isUnknownTy(target_type)) {
        local_op_type=op_type;
    }
    else if (isFloating(target_type)) {
        switch(op_type){
        case(OpCodeEn::eq): local_op_type = (OpCodeEn::oeq); break;
        case(OpCodeEn::ne): local_op_type = (OpCodeEn::one); break;
        case(OpCodeEn::sgt): local_op_type = (OpCodeEn::ogt); break;
        case(OpCodeEn::sge): local_op_type = (OpCodeEn::oge); break;
        case(OpCodeEn::slt): local_op_type = (OpCodeEn::olt); break;
        case(OpCodeEn::sle): local_op_type = (OpCodeEn::ole); break;
        }
    }
    else {
        print_error("newComparsionOperation - unsigned is not supported yet");
    }

    return garbage_container->add(new Operation(local_op_type, arg_a, arg_b));
}


Value* newConvolveOperation(GarbageContainer* garbage_container, TypeEn target_type, Value* arg_a, Value* arg_b, int64_t shift, OpCodeEn op_type) {

    if (op_type != OpCodeEn::convolve)
        print_error("convolve_f operation is not supported yet");

    if ((isConst(arg_a) || isConst(arg_b) )&& !(isUnknownTy(arg_a) || isUnknownTy(arg_b))) {
        return garbage_container->add(newArithmeticOperation(garbage_container,target_type, arg_a, arg_b, OpCodeEn::mul));
    }
    else if (isSmallArr(arg_a) && isSmallArr(arg_b)) {
        return garbage_container->add(new Operation(op_type, arg_a, arg_b, shift));
        //print_error("convolve(SmallArr_t,SmallArr_t) - is not supported yet");
    }
    else if (isSmallArr(arg_a) || isSmallArr(arg_b)) {
        if (isSmallArr(arg_b))
            return garbage_container->add(new Operation(op_type, arg_a, arg_b, shift));
        else
            return garbage_container->add(new Operation(op_type, arg_b, arg_a, shift));
    }
    else if (isLargeArr(arg_a) && isLargeArr(arg_b)) {
        print_error("convolve(LargeArr_t,LargeArr_t) - is not supported");
    }

    return garbage_container->add(new Operation(op_type, arg_a, arg_b));
}


Value* newSelectOp(GarbageContainer* garbage_container, TypeEn target_type, Value* arg_a, Value* arg_b, Value* arg_c ,bool rec_call){

    if (!isÑompatible(arg_b, arg_c) || !isÑompatible(arg_a, arg_b))
        print_error("uncompatible values");

    auto i1=newTypeConvOp(garbage_container,TypeEn::int1_jty, arg_a);
    
    if (isConst(i1) && !isUnknownTy(i1))
        if (i1->getConvTypeVal<bool>())
            return arg_b;
        else
            return arg_c;

    return garbage_container->add(new Operation(OpCodeEn::select, i1, arg_b, arg_c, target_type, rec_call));
}

Value* newSliceOp(GarbageContainer* garbage_container, Value* arg_a, Value* arg_b, OpCodeEn op_type) {

    if (!(isConst(arg_b) && isInteger(arg_b)))
        print_error("(arr,int) - second argument must be integer consant");
    int64_t intVal =arg_b->getBinaryValue();

    return garbage_container->add(new Operation(op_type, arg_a, arg_a->getType(), intVal));
}

Value* newSliceOp(GarbageContainer* garbage_container, Value* arg_a, int64_t intVal, OpCodeEn uTypeOp) {

    return garbage_container->add(new Operation(uTypeOp, arg_a, arg_a->getType(), intVal));
}

Value* newSmallArrayDefOp(GarbageContainer* garbage_container, stack<Value*> &args, OpCodeEn op_type, bool is_prototype) {

    if (args.empty())
        print_error("SmallArray is empty");

    Value* var=args[0];
    
    bool  allIsConst =true;
    for (auto i : args) {
        var=maxTypeVar(i, var);
        allIsConst &= isConst(var);
    }

    if (!allIsConst)
        print_error("array concatenation is not supported yet");

    TypeEn targertType =var->getType();
    
    if (isUnknownTy(targertType) || op_type == OpCodeEn::smallArrayRange)
        return garbage_container->add(new Operation(op_type, args, targertType));

    if (is_prototype)
        return garbage_container->add(new Operation(OpCodeEn::smallArrayDef, args, targertType));

    stack<Value*> typedArgs;
    for (auto i : args)
        typedArgs.push(newTypeConvOp(garbage_container,targertType, i));

    return garbage_container->add(new Operation(OpCodeEn::smallArrayDef, typedArgs, targertType));
}