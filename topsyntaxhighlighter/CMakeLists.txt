
cmake_minimum_required(VERSION 3.10)

# set the project name
project(syntaxhighlighter)

set(CMAKE_CONFIGURATION_TYPES Debug Release)


set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOMOC_RELAXED_MODE TRUE)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


find_package(Qt5 COMPONENTS Widgets REQUIRED)
get_target_property(QtCore_location Qt5::Widgets LOCATION)





# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(EGrammarDir ../EGrammar)
set(antlr4Dir ../antlr4)



#-------------------------done

set(mainDir .)

include_directories(${mainDir} )
include_directories(${EGrammarDir})
include_directories(${QT_USE_FILE})
include_directories(${antlr4Dir}/antlr4-runtime)


aux_source_directory(${EGrammarDir} EGrammar)
aux_source_directory(${mainDir} mainSources)

file(GLOB EGrammarHeaders ${EGrammarDir}/*.h)
file(GLOB mainHeaders ${mainDir}/*.h)




add_executable(syntaxhighlighter ${EGrammar} ${mainSources}  resource.qrc )


source_group( "EGrammar\\headers"   FILES ${EGrammarHeaders} )
source_group( "EGrammar\\sources"   FILES ${EGrammar} )
source_group( "headers"         FILES ${mainHeaders} )
source_group( "sources"         FILES ${mainSources})

#set_property(SOURCE ${antlr4Dir}/antlr4-runtime.h PROPERTY SKIP_AUTOMOC ON)

add_definitions(-DQT_NO_KEYWORDS)




set_target_properties(syntaxhighlighter        PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG      "Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE    "Release"
)


if(MSVC) 
    if(NOT CMAKE_CL_64)
        message(FATAL_ERROR  "WIN32 target is not supported yet" )
    endif()

    if(MSVC AND NOT (${CMAKE_VERSION} VERSION_LESS 2.8.11))
        set(CMAKE_CXX_STACK_SIZE "10000000")
    endif()

    SET(antlrLibPath ${antlr4Dir}/lib/vs-2015/x64/Release\ DLL)
    
    target_link_directories(syntaxhighlighter PUBLIC  ${antlrLibPath})
    target_link_libraries(syntaxhighlighter Qt5::Widgets antlr4-runtime )


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

    target_compile_options(syntaxhighlighter PUBLIC "$<$<CONFIG:DEBUG>:/MD>")


    set_property(TARGET syntaxhighlighter APPEND PROPERTY
        COMPILE_DEFINITIONS ${commonMSVC_Definitions} 
    )
    set_property(TARGET syntaxhighlighter APPEND PROPERTY
        COMPILE_DEFINITIONS "$<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>"
    )


    set_property(DIRECTORY PROPERTY
                VS_STARTUP_PROJECT syntaxhighlighter
    )


    add_custom_command(TARGET syntaxhighlighter PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${antlrLibPath}/antlr4-runtime.dll
            $<TARGET_FILE_DIR:syntaxhighlighter>)



else()
    message(FATAL_ERROR  "compilers other than MSVC are not supported" )   
endif()





#COMMAND $<$<CONFIG:Release>:strip>  
