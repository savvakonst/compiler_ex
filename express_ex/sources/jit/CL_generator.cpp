#include "CL_generator.h"


typedef float local_T;
void clGpuConvolve() {
    GPUPlatform platform;



    const int size_of_large_v = 1 << 20;
    const int size_of_small_v = 1 << 10;

    std::vector<local_T> large_input_v;  rangeVector(large_input_v, size_of_large_v + size_of_small_v, 1);
    std::vector<local_T> small_input_v;  rangeVector(small_input_v, size_of_small_v, 1);

    std::vector<local_T> output_v; output_v.resize(size_of_large_v);


    GpuConvolve<local_T> convolve(&platform);
    convolve.sizeInit(size_of_large_v, size_of_small_v, 0);

    for (int i=0; i < (1024*1); i++) {
        convolve.run(large_input_v.data(), small_input_v.data(), output_v.data());
        //if ((i % 32) == 0)
        //    std::cout << "convolve in progress\n";
    }
    convolve.run(large_input_v.data(), small_input_v.data(), output_v.data());
    std::cout << "convolve complete";
}





//template<typename local_T>
int clGpuConvolveTemplate(void)
{

    
    std::string stype = typeid(local_T).name();
    std::string kernel_header = std::string() + "__kernel void convolve( global const" + stype +"*left_v, global const" + stype + "*right_v,  global double" + stype + "*output_v)";
    std::string kernel_source  = kernel_header + R"CLC(
    {   
        int index=get_global_id(0);
        output_v[index] = input_v[index] * 3;
    }
    )CLC";


    cl_int err = CL_SUCCESS;
    try {



        ///
        ///
        ///
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        std::string platver = platforms[0].getInfo<CL_PLATFORM_VERSION>();
        std::cout << "platform version:" << platver << ".\n";
        if (platforms.size() == 0) {
            std::cout << "Platform size 0\n";
            return -1;
        }


        //cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
        //cl::Context context(CL_DEVICE_TYPE_CPU, properties);
        cl::Context context = cl::Context::getDefault();
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();



        cl::Program program_ = cl::Program(context, kernel_source);
        program_.build(devices);

        cl::Kernel kernel(program_, "convolve", &err);

        cl::Event event;

        const int number_of_points = 4000;
        const int size_in_bytes    = number_of_points * sizeof(local_T);

        std::vector<local_T> input_v;  rangeVector(input_v, number_of_points, 1);
        std::vector<local_T> output_v; output_v.resize(number_of_points);
        //CL_MEM_READ_ONLY;

        cl::CommandQueue queue(context, devices[0], 0, &err);

        cl::Buffer input_buffer(context, CL_MEM_READ_WRITE, size_in_bytes);
        cl::Buffer output_buffer(context, CL_MEM_READ_WRITE, size_in_bytes);

        queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, size_in_bytes, input_v.data());


        kernel.setArg(0, input_buffer);
        kernel.setArg(1, output_buffer);

        auto status =queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            cl::NDRange(number_of_points),
            cl::NullRange,
            NULL,
            &event);

        if (CL_SUCCESS != status)
            return EXIT_SUCCESS;

        queue.finish();
        queue.enqueueReadBuffer(output_buffer, CL_TRUE, 0, size_in_bytes, output_v.data());
        event.wait();

    }
    catch (cl::Error err) {
        std::cerr
            << "ERROR: "
            << err.what()
            << "("
            << err.err()
            << ")"
            << std::endl;

    }

    return EXIT_SUCCESS;

}