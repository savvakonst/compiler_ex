
#ifndef CL_GENERATOR_H
#define CL_GENERATOR_H

#define __CL_ENABLE_EXCEPTIONS
#include <typeinfo>
#include <iostream>
#include <strstream>
#include <fstream>
#include <CL/cl.hpp>
#include <cstdlib>



template<typename T>
void rangeVector(std::vector<T> & arr, int N, double step) {
    arr.clear();
    arr.resize(N);
    for (int i=0; i < N; i++)
        arr[i]=(T)(i * step);
}



//template<typename local_T>
class GPUPlatform {
public:
    GPUPlatform() {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        platform_ver_ = platforms[0].getInfo<CL_PLATFORM_VERSION>();

        if (platforms.size() == 0) {
            err_string_ = "Platform size 0\n";
            status_ = -1;
            return;
        }


        cl_context_properties properties[] ={ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
        //context_ = cl::Context::getDefault();
        context_ = cl::Context(CL_DEVICE_TYPE_GPU, properties);
        devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
      
    }

    template<typename local_T>
        friend class GpuConvolve;

private:
    std::vector<cl::Device> devices_;
    cl::Context  context_ ;
    std::string  platform_ver_ = "";

    std::string  err_string_ = "";
    int          status_ = 0;
};


template<typename local_T>
class GpuConvolve
{
public:
    GpuConvolve(GPUPlatform *parent){
        parent_ = parent ;


        std::string stype = std::string(" ")+typeid(local_T).name()+" ";
        std::string kernel_header = std::string() + 
            "__kernel void convolve("
            "   global const" + stype + "*left_v,"
            "   global const" + stype + "*right_v,"
            "   global " + stype + "*output_v,"
            "   int left_size,"
            "   int right_size,"
            "   int output_offset"
            ")";

        std::string kernel_source = kernel_header + R"CLC(
        {)CLC" + 
            stype +R"CLC( summ = 0.0 ;
            int size = right_size;
            int left_offset = size / 2;

            int block_ix = get_group_id(0);
            int local_ix = get_local_id(0);
            int block_dim = get_local_size(0);
            int index     = block_dim * block_dim + local_ix ;

            if(index<left_size){
                for(int i=0;i < size;i++)
                    summ += left_v[i + index] * right_v[size - 1 - i];
        
                output_v[index + left_offset + output_offset] = summ;
            }
        }
        )CLC";

        auto &context = parent_->context_;
        auto &devices = parent_->devices_;

        cl::Program program_ = cl::Program(context, kernel_source);
        program_.build(devices);

        cl_int err = CL_SUCCESS;

        kernel_ = cl::Kernel(program_, "convolve", &err);
        queue_ = cl::CommandQueue(context, devices[0], 0, &err);
    }

    ~GpuConvolve()
    {

    }

    void run( const local_T * left_v,  const local_T * right_v, local_T * output_v) {

        auto &context = parent_->context_;
        const int type_size=sizeof(local_T);

        queue_.enqueueWriteBuffer(input_left_buffer_, CL_TRUE, 0, (left_size_ + right_size_) * type_size, left_v);
        queue_.enqueueWriteBuffer(input_right_buffer_, CL_TRUE, 0, right_size_ * type_size, right_v);

        kernel_.setArg(0, input_left_buffer_);
        kernel_.setArg(1, input_right_buffer_);
        kernel_.setArg(2, output_buffer_);

        int output_offset = 0;

        kernel_.setArg(3, sizeof(cl_int), (void *)& left_size_ );
        kernel_.setArg(4, sizeof(cl_int), (void *)& right_size_ );
        kernel_.setArg(5, sizeof(cl_int), (void *)& output_offset);

        cl::Event event;

        int block_dim = 64;
        auto status = queue_.enqueueNDRangeKernel(
            kernel_,
            cl::NullRange,
            cl::NDRange(left_size_/ block_dim),
            cl::NDRange(block_dim),
            NULL,
            &event);

        if (CL_SUCCESS != status)
            return ;

        queue_.finish();
        //std::cout << "convolve in progress 1\n";
        queue_.enqueueReadBuffer(output_buffer_, CL_TRUE, 0, left_size_* type_size, output_v);

        event.wait();
        //std::cout << "convolve in progress 2\n";
    }


    void sizeInit(const uint64_t left_size, const uint64_t right_size, const uint64_t offset) {
        auto &context = parent_->context_;
        const int type_size=sizeof(local_T);

        left_size_=left_size;
        right_size_=right_size;
        offset_=offset;

        input_left_buffer_   = cl::Buffer(context, CL_MEM_READ_ONLY, (left_size + right_size)* type_size);
        input_right_buffer_  = cl::Buffer(context, CL_MEM_READ_ONLY, right_size* type_size);
        output_buffer_       = cl::Buffer(context, CL_MEM_READ_WRITE, left_size * type_size);

    }

private:
    cl::Kernel kernel_ ;
    cl::CommandQueue queue_ ;

    cl::Buffer input_left_buffer_ ;
    cl::Buffer input_right_buffer_ ;
    cl::Buffer output_buffer_ ;

    uint64_t left_size_ ;
    uint64_t right_size_ ;
    uint64_t offset_;

    GPUPlatform *parent_ ;
};

void clGpuConvolve();
#endif // !CL_GENERATOR_H


