
cmake_minimum_required(VERSION 3.10)

# set the project name
project(express_ex)

SET(IS_SHARED True)


# specify the C++ standard 
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if(MSVC AND NOT (${CMAKE_VERSION} VERSION_LESS 2.8.11))
  set(CMAKE_CXX_STACK_SIZE "10000000")
endif()  



set(EGrammarDir ../EGrammar)
set(llvmDir ../llvm)
set(antlr4Dir ../antlr4)


include_directories(${EGrammarDir})
include_directories(${llvmDir}/include)
include_directories(${llvmDir}/build/include)
include_directories(${antlr4Dir}/antlr4-runtime)


include_directories(include/ifs)
include_directories(include/jit)
include_directories(include/parser)
include_directories(include/ifs)


set(sourcesDir sources)

aux_source_directory(${EGrammarDir} EGrammar)
aux_source_directory(${sourcesDir}/jit jit)
aux_source_directory(${sourcesDir}/parser parser)
aux_source_directory(${sourcesDir}/ifs ifs)

file(GLOB EGrammarHeaders ${EGrammarDir}/*.h)
file(GLOB jitHeaders include/jit/*.h)
file(GLOB parserHeaders include/parser/*.h)
file(GLOB ifsHeaders include/ifs/*.h)


if(IS_SHARED)
    add_library( express_ex SHARED ${EGrammar} ${ifs} ${jit} ${parser} ${EGrammarHeaders} ${ifsHeaders} ${jitHeaders} ${parserHeaders} )
else()
    add_executable(express_ex ${EGrammar} ${ifs} ${jit} ${parser} ${EGrammarHeaders} ${ifsHeaders} ${jitHeaders} ${parserHeaders}  )
endif()

#add_library()

source_group( "EGrammar\\headers"   FILES ${EGrammarHeaders} )
source_group( "EGrammar\\sources"   FILES ${EGrammar} )
source_group( "jit\\headers"        FILES ${jitHeaders} )
source_group( "jit\\sources"        FILES ${jit})
source_group( "parser\\headers"     FILES ${parserHeaders} )
source_group( "parser\\sources"     FILES ${parser})
source_group( "ifs\\headers"        FILES ${ifsHeaders} )
source_group( "ifs\\sources"        FILES ${ifs})

message("------ EGrammarHeaders:   " "${EGrammarHeaders}")


set(CMAKE_CONFIGURATION_TYPES Debug Release)

set_target_properties(express_ex        PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG      "Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE    "Release"
)

if(MSVC) 
    if(NOT CMAKE_CL_64)
        message(FATAL_ERROR  "WIN32 target is not supported yet" )   
    endif()

    target_compile_options(express_ex PRIVATE 
        /wd4141   #(level 1)  'modifier' : used more than once  inline inline void func ()
        /wd4146   #(level 2)   unary minus operator applied to unsigned type, result still unsigned
        /wd4244   #(level 2)  'argument' : conversion from 'type1' to 'type2', possible loss of data
        /wd4267   #(level 3)  'var' : conversion from 'size_t' to 'type', possible loss of data
        /wd4291   #(level 1)  'declaration' : no matching operator delete found memory will not be freed if initialization throws an exception
        /wd4345   #(level 2)  behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
        /wd4351 
        /wd4456 
        /wd4457 
        /wd4458 
        /wd4459 
        /wd4503 
        /wd4624 
        /wd4722 
        /wd4100 
        /wd4127 
        /wd4512 
        /wd4505 
        /wd4610 
        /wd4510 
        /wd4702 
        /wd4245 
        /wd4706 
        /wd4310 
        /wd4701 
        /wd4703 
        /wd4389 
        /wd4611 
        /wd4805 
        /wd4204 
        /wd4577 
        /wd4091 
        /wd4592 
        /wd4319 
        /wd4709 
        /wd4324 
        /wd4251 
        /wd4348
        /wd4804
        /wd4996
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
        LLVMX86Utils
        LLVMCore
        LLVMExecutionEngine
        LLVMInterpreter
        LLVMSupport
        LLVMDemangle
        LLVMMCJIT
        LLVMDemangle
        LLVMMCJIT

    )



    SET(llvmLibPath ${llvmDir}/Lib/x64)

    SET(llvmReleaseLibPath ${llvmDir}/Lib/Release/lib)
    SET(llvmDebugLibPath ${llvmDir}/../../llvm-project/build_exp/Debug/lib)

    SET(antlrReleaseLibPath ${antlr4Dir}/lib/vs-2015/x64/Release\ DLL)

    SET(antlrDebugLibPath ${antlr4Dir}/lib/vs-2015/x64/Debug\ DLL)
    SET(antlrLibPath    ${antlr4Dir}/lib/vs-2015/x64/Release\ DLL)
    #debug ${llvmLibPath} ${antlrDebugLibPath}

    target_link_directories(express_ex PUBLIC 
        "$<$<CONFIG:Debug>:   ${antlrDebugLibPath}>"   "$<$<CONFIG:Debug>:   ${llvmDebugLibPath}>" 
        "$<$<CONFIG:Release>: ${antlrReleaseLibPath}>" "$<$<CONFIG:Release>: ${llvmReleaseLibPath}>" 
    )
    #target_link_directories(express_ex PUBLIC ${llvmLibPath}  ${antlrReleaseLibPath} )
    #target_compile_options(express_ex PUBLIC "$<$<CONFIG:DEBUG>:/MD>")
    #set_property(TARGET express_ex APPEND PROPERTY
    #    COMPILE_DEFINITIONS "$<$<CONFIG:Debug>: _ITERATOR_DEBUG_LEVEL=0>" 
    #)


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
    else()
        set_property(TARGET express_ex APPEND PROPERTY
            COMPILE_DEFINITIONS ${commonMSVC_Definitions} 
        )
    endif()


    

    #add_custom_command(TARGET express_ex PRE_BUILD 
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #        ${antlrLibPath}/antlr4-runtime.dll
    #        $<TARGET_FILE_DIR:express_ex>) 

    #set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT "express_ex")
    set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "express_ex")
else()
    message(FATAL_ERROR  "compilers other than MSVC are not supported" )   
endif()





#COMMAND $<$<CONFIG:Release>:strip>  
