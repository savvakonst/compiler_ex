#include "variable.h"

Variable::Variable(string text, TypeEn type_) {
    textValue = text;
    type = type_;

    binaryValue = 0;

    switch (type_)
    {
    case TypeEn::Int1_jty:   *((bool   *)(&binaryValue)) = (bool)stoi(textValue); break;
    case TypeEn::Int8_jty:   *((int8_t *)(&binaryValue)) = stoi(textValue); break;
    case TypeEn::Int16_jty:  *((int16_t*)(&binaryValue)) = stoi(textValue); break;
    case TypeEn::Int32_jty:  *((int32_t*)(&binaryValue)) = stoi(textValue); break;
    case TypeEn::Int64_jty:  *((int64_t*)(&binaryValue)) = stol(textValue); break;
    case TypeEn::Float_jty:  *((float  *)(&binaryValue)) = stof(textValue); break;
    case TypeEn::Double_jty: *((double *)(&binaryValue)) = stod(textValue); break;
    case TypeEn::Unknown_jty:                                               break;
    default: print_error("constant reading error");                         break;
    }

};

Variable::Variable(int64_t value, TypeEn type_) {
    binaryValue = value;
    type = type_;

    switch (type_)
    {
    case TypeEn::Int1_jty:   textValue = std::to_string(*((bool   *)(&binaryValue))); break;
    case TypeEn::Int8_jty:   textValue = std::to_string(*((int8_t *)(&binaryValue))); break;
    case TypeEn::Int16_jty:  textValue = std::to_string(*((int16_t*)(&binaryValue))); break;
    case TypeEn::Int32_jty:  textValue = std::to_string(*((int32_t*)(&binaryValue))); break;
    case TypeEn::Int64_jty:  textValue = std::to_string(*((int64_t*)(&binaryValue))); break;
    case TypeEn::Float_jty:  textValue = std::to_string(*((float  *)(&binaryValue))); break;
    case TypeEn::Double_jty: textValue = std::to_string(*((double *)(&binaryValue))); break;
    case TypeEn::Unknown_jty:                                                         break;
    default: print_error("constant calc error");                                      break;
    }
};

Variable::Variable(Variable* arg1, Variable* arg2, Variable* arg3) {
    if (!(isConst(arg1) && isConst(arg2) && isConst(arg3))) {
        print_error("range args must be a constant");
        return;
    }
    else {
        print_error("range(start_num,stop_num,step) -signature is not supported yet");
        return;
    }
};

Variable::Variable(Variable* arg1, Variable* arg2) {
    if (isConst(arg1) && isConst(arg2) && isInteger(arg1) && isInteger(arg2)) {
        length    = arg2->getBinaryValue() - arg1->getBinaryValue();
        dstype    = DataStructTypeEn::smallArr_dsty;
        type      = TypeEn::Double_jty;
        textValue = "range(" + std::to_string(arg1->getBinaryValue()) + "," + std::to_string(arg2->getBinaryValue()) + ")";
    }
    else {
        print_error("range(start_num,stop_num) - arg must be integer consant");
    }
};

Variable::Variable(Variable* arg1) {
    if (isConst(arg1) && isInteger(arg1)) {
        length    = arg1->getBinaryValue();
        dstype    = DataStructTypeEn::smallArr_dsty;
        type      = TypeEn::Double_jty;
        textValue = "range(" + std::to_string(arg1->getBinaryValue()) + ")";
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

void Variable::setBufferLength(uint64_t left, uint64_t right) {
    if (isLargeArr(this)) {
        leftBufferLength=maxInt(leftBufferLength, left);
        rightBufferLength=maxInt(rightBufferLength, right);
    }
}

void Variable::setBufferLength(Variable * var) {
    if (isLargeArr(this)) {
        leftBufferLength =maxInt(leftBufferLength, var->getLeftBufferLen());
        rightBufferLength =maxInt(rightBufferLength, var->getRightBufferLen());
    }
}

void Variable::setLevel(int64_t var) {
    level=maxInt(level, var);
}

string  Variable::getTxtDSType() {
    string t = "pass";
#define ENUM2STR(x) case (DataStructTypeEn::x):t=#x;   break
    switch (dstype)
    {
        ENUM2STR(constant_dsty);
        ENUM2STR(smallArr_dsty);
        ENUM2STR(largeArr_dsty);
    }
    return  t;
#undef ENUM2STR
}






