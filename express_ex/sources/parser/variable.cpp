#include "variable.h"

Variable::Variable(string text, TypeEn type) :SmallArr() {
    textValue_ = text;
    type_ = type;

    binaryValue_ = 0;

    switch (type)
    {
    case TypeEn::int1_jty:   *((bool   *)(&binaryValue_)) = (bool)stoi(textValue_); break;
    case TypeEn::int8_jty:   *((int8_t *)(&binaryValue_)) = stoi(textValue_);   break;
    case TypeEn::int16_jty:  *((int16_t*)(&binaryValue_)) = stoi(textValue_);   break;
    case TypeEn::int32_jty:  *((int32_t*)(&binaryValue_)) = stoi(textValue_);   break;
    case TypeEn::int64_jty:  *((int64_t*)(&binaryValue_)) = stol(textValue_);   break;
    case TypeEn::float_jty:  *((float  *)(&binaryValue_)) = stof(textValue_);   break;
    case TypeEn::double_jty: *((double *)(&binaryValue_)) = stod(textValue_);   break;
    case TypeEn::unknown_jty:                                                   break;
    default: print_error("constant reading error");                             break;
    }
};


Variable::Variable(int64_t value, TypeEn type) :SmallArr() {

    binaryValue_ = value;
    type_ = type;
#define OP(T) textValue_ = std::to_string(*((T   *)(&binaryValue_)))
    SWITCH_TYPE_OP(type, ;);
#undef OP 
};


Variable::Variable(Variable* arg1, Variable* arg2, Variable* arg3) :SmallArr() {
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

Variable::Variable(Variable* arg1, Variable* arg2) :SmallArr() {
    if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2)) {
        length_    = arg2->getBinaryValue() - arg1->getBinaryValue();
        dsType_    = DataStructTypeEn::smallArr_dsty;
        type_      = TypeEn::int64_jty;
        textValue_ = "range(" + std::to_string(arg1->getBinaryValue()) + "," + std::to_string(arg2->getBinaryValue()) + ")";
        start_ = arg1->getBinaryValue(); 
        stop_  = arg2->getBinaryValue();
    }
    else {
        print_error("range(start_num,stop_num) - arg must be integer consant");
    }
};

Variable::Variable(Variable* arg1) :SmallArr() {
    if (isConst(arg1) && isInteger(arg1)) {
        length_    = arg1->getBinaryValue();
        dsType_    = DataStructTypeEn::smallArr_dsty;
        type_      = TypeEn::int64_jty;
        textValue_ = "range(" + std::to_string(arg1->getBinaryValue()) + ")";
        start_     = 0;
        stop_      = length_ ;
    }
    else {
        print_error("range(len) - arg must be integer consant");
    }
};

void Variable::setBuffered() {
    if (isLargeArr(this)) {
        is_buffered=true;
    }
}

void Variable::setBufferLength(uint64_t central_length) {
    bufferLength_ = central_length;
}

void Variable::setBufferLength(uint64_t left, uint64_t right) {
    if (isLargeArr(this)) {
        leftBufferLength_=maxInt(leftBufferLength_, left);
        rightBufferLength_=maxInt(rightBufferLength_, right);
    }
}

void Variable::setBufferLength(Variable * var) {
    if (isLargeArr(this)) {
        leftBufferLength_ =maxInt(leftBufferLength_, var->getLeftBufferLen());
        rightBufferLength_ =maxInt(rightBufferLength_, var->getRightBufferLen());
    }
}

void Variable::setLevel(int64_t var) {
    level_=maxInt(level_, var);
}

string Variable::getTxtDSType() {
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

double Variable::getDoubleValue()
{
    double ret=0.0;
#define OP(T) ret = (double)(*((T*)(&binaryValue_)))
    SWITCH_TYPE_OP(type_, print_error("getDoubleValue error");)
#undef OP
    return ret;
}

void Variable::calculate()
{
    double delta=0.0;
    if (isInteger(type_)) {
        delta = (stop_ - start_) / length_;
    }

    bufferPtr_=calcSmallArrayAlloc(type_, length_);

#define OP(T) {\
    T* point=(T*)bufferPtr_;\
    for (uint64_t i=0; i < length_; i++)  \
        *point++ =(T)(start_ + delta * i);}
    
    SWITCH_TYPE_OP(type_, print_SA_error("samallarray range ");)
#undef OP
}

std::string Variable::printSmallArray()
{
    std::string ret = "array=[";
    if (length_ < 1)
        return ret;

#define OP(T) {\
    T* point=(T*)bufferPtr_;\
    ret+=std::to_string( point[0]);\
    for (uint64_t i=1; i < length_; i++)  \
        ret+=","+std::to_string( point[i]);}
    SWITCH_TYPE_OP(type_, print_SA_error("printSmallArray");)
#undef OP
    ret+="];";
    return ret;
}