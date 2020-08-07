#ifndef BUFFER_H
#define BUFFER_H

#include "ioIfs.h"
#include "common.h"
#include "variable.h"

TypeEn      RPMType2JITType(RPMTypesEn arg);



/*
typedef struct {
	ex_size_t   length = 0;
	ex_size_t   left_offset = 0;
	ex_size_t   right_offset = 0;
}BufferInfo;
*/

enum class BufferTypeEn {
	input,
	internal,
	output
};


class Buffer {
public:

	Buffer(Variable * var) {
		length_			  = var->getLength();
		left_offset_	  = var->getLeftBufferLen();
		right_offset_	  = var->getRightBufferLen();
		type_			  = TypeEn::unknown_jty;
		type_			  = var->getType();
		sizeof_data_type_ = sizeOfTy(type_);

		ptr_ = new char[ sizeof_data_type_ * (length_ + left_offset_ + right_offset_) ];
		//ptr_ = (char*)malloc(sizeof_data_type_ * (length_+ left_offset_ + right_offset_));
		bottom_ptr_ = ptr_ + (left_offset_ + right_offset_) * sizeof_data_type_;
		top_ptr_	= ptr_ + length_;

	}

	~Buffer(){
		delete ptr_;
	}

	virtual int64_t init(){
		return 0;
	}

	virtual int64_t update(){
		memcpy(top_ptr_, ptr_, (left_offset_ + right_offset_));
		return 0;
	}

	virtual BufferTypeEn getBufferType() {
		return BufferTypeEn::internal;
	}

	
	template <typename T> 
	T &stream(T &OS,  std::string offset="") {
		OS << offset << "BufferInfo{\n";
		OS << offset << "  ptr: " << ptr_ << "\n";
		OS << offset << "  length: ";
		OS.write_hex(length_);
		OS << "\n";
		OS << offset << "  left_offset: " << left_offset_ << "\n";
		OS << offset << "  right_offset: " << right_offset_ << "\n";
		OS << offset << "  type: " << toString(type_) << "\n";
		OS << offset << "  name: " << "" << "\n";
		OS << offset << "}\n";
		return OS;
	}
	

protected:
	TypeEn  type_=TypeEn::unknown_jty;
	int64_t	sizeof_data_type_=-1;
	int64_t	length_=-1;
	int64_t	left_offset_=-1;
	int64_t	right_offset_=-1;
	
	char* ptr_ =NULL;
	char* bottom_ptr_=NULL;
	char* top_ptr_=NULL;

};



class InputBuffer: public Buffer {

public:
	InputBuffer(Parameter_IFS * parameter, Variable * var):Buffer(var) { parameter_ = parameter; }
	~InputBuffer(){ }

	virtual int64_t init(){
		parameter_->open();
		parameter_->read(ptr_, left_offset_ + length_ + right_offset_);
		return 0;
	}

	virtual int64_t update() {
		memcpy(top_ptr_, ptr_, (left_offset_ + right_offset_));
		parameter_->read(bottom_ptr_, length_);
		return 0;
	}

	virtual BufferTypeEn getBufferType() {
		return BufferTypeEn::input;
	}

protected:
	Parameter_IFS * parameter_ = NULL;
	std::string		link_name_ = std::string();
};



class OutputBuffer : public Buffer {

public:
	OutputBuffer(Variable * var) :Buffer(var) { }
	~OutputBuffer() { }


	void addParameter(Parameter_IFS * parameter){
		parameter_ = parameter;
	}

	virtual int64_t init() {
		parameter_->open();
		return 0;
	}

	virtual int64_t update() {
		parameter_->write(ptr_, length_ );
		return 0;
	}

	virtual BufferTypeEn getBufferType() {
		return BufferTypeEn::output;
	}

protected:
	Parameter_IFS * parameter_=NULL;
	std::string link_name_="";
};


#endif