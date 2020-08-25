
#ifndef __linux__
    #ifdef DLL_express_ex_EXPORT 
        #define DLL_EXPORT  __declspec(dllexport)
    #else 
        #define DLL_EXPORT 
    #endif 
#else
    #define DLL_EXPORT_
#endif 