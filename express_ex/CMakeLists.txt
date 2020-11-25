
cmake_minimum_required(VERSION 3.10)

# set the project name
project(express_ex)

SET(IS_SHARED False)

if( NOT CMAKE_CL_64 )
    set(SPlatform  x86)
    set(APlarform  Win32)
else()
    set(SPlatform  x64)
    set(APlarform  x64)
endif()

# specify the C++ standard 
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CLHPP       C:/Users/SVK/Desktop/openCl/khronos/OpenCL-CLHPP/include) 
set(cudaPath    C:/Program\ Files/NVIDIA\ GPU\ Computing\ Toolkit/CUDA/v11.0)
set(openCL_includeDir  ${cudaPath}/include)

set(EGrammarDir ../EGrammar)
set(llvmDir     ../../common/llvm-project)
set(antlr4Dir   ../../common/antlr4/runtime/Cpp/runtime)


include(CMakeMacro.txt)

include_directories(${CLHPP})
include_directories(${openCL_includeDir})
include_directories(${EGrammarDir})
include_directories(${llvmDir}/llvm/include)
include_directories(${llvmDir}/build_${SPlatform}/include)
include_directories(${antlr4Dir}/src)


aux_source_directory(${EGrammarDir} EGrammar)
file(GLOB EGrammarHeaders ${EGrammarDir}/*.h)
init_source_groups(express_grops_list ifs parser jit)


if(IS_SHARED)
    add_library( express_ex SHARED  ${express_grops_list} ${EGrammar} ${EGrammarHeaders}  )
else()
    add_executable(express_ex  ${express_grops_list} ${EGrammar} ${EGrammarHeaders} )
endif()

#add_library()

source_group( "EGrammar\\headers"   FILES ${EGrammarHeaders} )
source_group( "EGrammar\\sources"   FILES ${EGrammar} )


set(CMAKE_CONFIGURATION_TYPES Debug Release)

set_target_properties(express_ex        PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG      "Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE    "Release"
)

if(MSVC) 

    if(NOT (${CMAKE_VERSION} VERSION_LESS 2.8.11))
        set(CMAKE_CXX_STACK_SIZE "10000000")
    endif()  

    target_compile_options(express_ex PRIVATE 
        #/wd4141   #(level 1)  'modifier' : used more than once  inline inline void func ()
        #/wd4146   #(level 2)   unary minus operator applied to unsigned type, result still unsigned
        #/wd4244   #(level 2)  'argument' : conversion from 'type1' to 'type2', possible loss of data
        #/wd4267   #(level 3)  'var' : conversion from 'size_t' to 'type', possible loss of data
        #/wd4291   #(level 1)  'declaration' : no matching operator delete found memory will not be freed if initialization throws an exception
        #/wd4345   #(level 2)  behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
        #/wd4351 
        #/wd4456 
        #/wd4457 
        #/wd4458 
        #/wd4459 
        #/wd4503 
        #/wd4624 
        #/wd4722 
        #/wd4100  #ignore unused variables
        #/wd4127 
        #/wd4512 
        #/wd4505 
        #/wd4610 
        #/wd4510 
        #/wd4702 
        #/wd4245 
        #/wd4706 
        #/wd4310 
        #/wd4701 
        #/wd4703 
        #/wd4389 
        #/wd4611
        #/wd4204 
        #/wd4577 
        #/wd4091 
        #/wd4592 
        #/wd4319 
        #/wd4709 
        #/wd4324 
        #/wd4251 
        #/wd4348
        #/wd4996
        #/wd4805 
        /wd4804
        /wd4018
        /wd5208
    )


    SET(ObjNames
        LLVMX86CodeGen 
        LLVMX86Desc 
        LLVMX86Info 
        LLVMRuntimeDyld 
        LLVMMCDisassembler 
        LLVMAsmPrinter 
        LLVMDebugInfoDWARF 
        LLVMCFGuard 
        LLVMGlobalISel 
        LLVMSelectionDAG 
        LLVMCodeGen 
        LLVMTarget 
        LLVMBitWriter 
        LLVMScalarOpts 
        LLVMAggressiveInstCombine 
        LLVMInstCombine 
        LLVMTransformUtils 
        LLVMAnalysis 
        LLVMObject 
        LLVMBitReader 
        LLVMMCParser
        LLVMMC
        LLVMDebugInfoCodeView
        LLVMDebugInfoMSF
        LLVMTextAPI
        LLVMProfileData
        LLVMBinaryFormat
        LLVMRemarks
        LLVMBitstreamReader
        #LLVMX86Utils
        LLVMCore
        LLVMExecutionEngine
        LLVMInterpreter
        LLVMSupport
        LLVMDemangle
        LLVMMCJIT
        LLVMDemangle
        LLVMMCJIT
        OpenCL
    )

    
    set(openCL_libDir ${cudaPath}/lib/${APlarform})
    SET(llvmLibPath   ${llvmDir}/build_${SPlatform}/$<CONFIG>/lib)
    SET(antlrLibPath  ${antlr4Dir}/bin/vs-2019/${SPlatform}/$<CONFIG>\ DLL)

    target_link_directories(express_ex PUBLIC 
        ${antlrLibPath}
        ${llvmLibPath}
        ${openCL_libDir}
    )


    target_link_libraries(express_ex ${ObjNames} antlr4-runtime )


    SET(commonMSVC_Definitions 
        WIN32
        _WINDOWS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_NONSTDC_NO_WARNINGS
        _SCL_SECURE_NO_DEPRECATE
        _SCL_SECURE_NO_WARNINGS
        UNICODE 
        _UNICODE
        __STDC_CONSTANT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_LIMIT_MACROS
    )

   

    
    
    if(IS_SHARED)
        set_property(TARGET express_ex APPEND PROPERTY
            COMPILE_DEFINITIONS ${commonMSVC_Definitions} DLL_express_ex_EXPORT
        )

        set(FILE_NAMES_TO_COPY "express_ex.*" "syntaxhighlighter.*" "topsyntaxhighlighter.*" "antlr4-runtime.*")


        
        foreach( FILE_NAME_MASK ${FILE_NAMES_TO_COPY})
            file(GLOB FILES_TO_COPY "$<TARGET_FILE_DIR:express_ex>/${FILE_NAME_MASK}")
            set(FILES_TO_COPYasfsaf "$<TARGET_FILE_DIR:express_ex>/${FILE_NAME_MASK}" )

            message( "-------------------------FILES_TO_COPY: " "$<TARGET_FILE_DIR:express_ex>" )   
            #set(FILES_TO_COPY "$<TARGET_FILE_DIR:express_ex>/${FILE_NAME_MASK}" )

            foreach( FILE_NAME_VAR ${FILES_TO_COPY})
                message( "-------------------------FILE_NAME_VAR: ${FILE_NAME_VAR}" )   
                add_custom_command(TARGET express_ex POST_BUILD 
                    COMMAND  ${CMAKE_COMMAND} -E copy_if_different
                        ${FILE_NAME_VAR}
                        //nas_sy/Common/svk/express_ex/$<CONFIG>.Shared
                    ) 
            endforeach( FILE_NAME_VAR )
        endforeach( FILE_NAME_MASK )
        
        


    else()
        set_property(TARGET express_ex APPEND PROPERTY
            COMPILE_DEFINITIONS ${commonMSVC_Definitions} CONSOLE_APP_
        )
    endif()



    #set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT "express_ex")
    set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "express_ex")
else()
    message(FATAL_ERROR  "compilers other than MSVC are not supported" )   
endif()





#COMMAND $<$<CONFIG:Release>:strip>  
