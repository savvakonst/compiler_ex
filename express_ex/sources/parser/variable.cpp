#include "variable.h"

Value::Value(string text, TypeEn type) :SmallArr() {
    text_value_ = text;
    type_ = type;

    binary_value_ = 0;

    switch (type)
    {
    case TypeEn::int1_jty:   *((bool   *)(&binary_value_)) = (bool)stoi(text_value_); break;
    case TypeEn::int8_jty:   *((int8_t *)(&binary_value_)) = stoi(text_value_);   break;
    case TypeEn::int16_jty:  *((int16_t*)(&binary_value_)) = stoi(text_value_);   break;
    case TypeEn::int32_jty:  *((int32_t*)(&binary_value_)) = stoi(text_value_);   break;
    case TypeEn::int64_jty:  *((int64_t*)(&binary_value_)) = stol(text_value_);   break;
    case TypeEn::float_jty:  *((float  *)(&binary_value_)) = stof(text_value_);   break;
    case TypeEn::double_jty: *((double *)(&binary_value_)) = stod(text_value_);   break;
    case TypeEn::unknown_jty:                                                   break;
    default: print_error("constant reading error");                             break;
    }
};

Value::Value(untyped_t binary_value, TypeEn type) :SmallArr() {

    binary_value_ = binary_value;
    type_ = type;
#define OP(T) text_value_ = std::to_string(*((T   *)(&binary_value_)))
    SWITCH_TYPE_OP(type, ;);
#undef OP 
};


Value::Value(Value* arg1, Value* arg2, Value* arg3) :SmallArr() {
    if (!(isConst(arg1) && isConst(arg2) && isConst(arg3) && isInteger(arg3))) {
        print_error("range args must be a constant");
        return;
        length_    = arg3->getBinaryValue();
        type_      = TypeEn::double_jty;
        start_     = arg1->getDoubleValue();
        stop_      = arg2->getDoubleValue();
    }
    else {
        print_error("range(start_num,stop_num,length) -signature is not supported yet");
        return;
    }
};

Value::Value(Value* arg1, Value* arg2) :SmallArr() {
    if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2)) {
        length_    = arg2->getBinaryValue() - arg1->getBinaryValue();
        dsType_    = DataStructTypeEn::smallArr_dsty;
        type_      = TypeEn::int64_jty;
        text_value_ = "range(" + std::to_string(arg1->getBinaryValue()) + "," + std::to_string(arg2->getBinaryValue()) + ")";
        start_ = (double)arg1->getBinaryValue();
        stop_  = (double)arg2->getBinaryValue();
    }
    else {
        print_error("range(start_num,stop_num) - arg must be integer consant");
    }
};

Value::Value(Value* arg1) :SmallArr() {
    if (isConst(arg1) && isInteger(arg1)) {
        length_    = arg1->getBinaryValue();
        dsType_    = DataStructTypeEn::smallArr_dsty;
        type_      = TypeEn::int64_jty;
        text_value_ = "range(" + std::to_string(arg1->getBinaryValue()) + ")";
        start_     = 0.0;
        stop_      = (double)length_ ;
    }
    else {
        print_error("range(len) - arg must be integer consant");
    }
};

void Value::setBuffered() {
    if (isLargeArr(this)) {
        is_buffered=true;
    }
}

void Value::setBufferLength(uint64_t central_length) {
    buffer_length_ = central_length;
}

void Value::setBufferLength(uint64_t left, uint64_t right) {
    if (isLargeArr(this)) {
        left_buffer_length_=maxInt(left_buffer_length_, left);
        right_buffer_length_=maxInt(right_buffer_length_, right);
    }
}

void Value::setBufferLength(Value * var) {
    if (isLargeArr(this)) {
        left_buffer_length_ =maxInt(left_buffer_length_, var->getLeftBufferLen());
        right_buffer_length_ =maxInt(right_buffer_length_, var->getRightBufferLen());
    }
}

void Value::setLevel(int64_t var) {
    level_=maxInt(level_, var);
}

string Value::getTxtDSType()const{
    string t = "pass";
#define ENUM2STR(x) case (DataStructTypeEn::x):t=#x;   break
    switch (dsType_)
    {
        ENUM2STR(constant_dsty);
        ENUM2STR(smallArr_dsty);
        ENUM2STR(largeArr_dsty);
    }
    return  t;
#undef ENUM2STR
}

double Value::getDoubleValue()const {
    double ret=0.0;
#define OP(T) ret = (double)(*((T*)(&binary_value_)))
    SWITCH_TYPE_OP(type_, print_error("getDoubleValue error");)
#undef OP
    return ret;
}

void Value::calculate()
{
    double delta=0.0;
    if (isInteger(type_)) {
        delta = (stop_ - start_) / length_;
    }

    buffer_ptr_=calcSmallArrayAlloc(type_, (int)length_);

#define OP(T) {\
    T* point=(T*)buffer_ptr_;\
    for (uint64_t i=0; i < length_; i++)  \
        *point++ =(T)(start_ + delta * i);}
    
    SWITCH_TYPE_OP(type_, print_SA_error("samallarray range ");)
#undef OP
}

std::string Value::printSmallArray()
{
    std::string ret = "array=[";
    if (length_ < 1)
        return ret;

#define OP(T) {\
    T* point=(T*)buffer_ptr_;\
    ret+=std::to_string( point[0]);\
    for (uint64_t i=1; i < length_; i++)  \
        ret+=","+std::to_string( point[i]);}
    SWITCH_TYPE_OP(type_, print_SA_error("printSmallArray");)
#undef OP
    ret+="];";
    return ret;
}