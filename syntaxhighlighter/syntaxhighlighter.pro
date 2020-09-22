QT += widgets
#requires(qtConfig(filedialog))
#CONFIG += c++11

DEFINES += QUILIB_LIBRARY

TARGET = $$qtLibraryTarget(syntaxhighlighter)
TEMPLATE = lib

EXPRESS_DIR = $$PWD/..

PROJ_INCLUDE_DIR = include
PROJ_SOURCES_DIR = sources

INCLUDEPATH     = $${EXPRESS_DIR}/antlr4/antlr4-runtime \
                  $${EXPRESS_DIR}/EGrammar \
                  $${PROJ_INCLUDE_DIR}


HEADERS         = $${PROJ_INCLUDE_DIR}/highlighter.h \
                  $${PROJ_INCLUDE_DIR}/highlightListener.h \
                  $${PROJ_INCLUDE_DIR}/highlightStyle.h \
                  $${PROJ_INCLUDE_DIR}/mainWindow.h \
                  $${PROJ_INCLUDE_DIR}/textEdit.h \
                  $${PROJ_INCLUDE_DIR}/tree.h

SOURCES         = $${PROJ_SOURCES_DIR}/highlighter.cpp \
                  $${PROJ_SOURCES_DIR}/highlightListener.cpp \
                  $${PROJ_SOURCES_DIR}/main.cpp \
                  $${PROJ_SOURCES_DIR}/mainWindow.cpp \
                  $${PROJ_SOURCES_DIR}/textEdit.cpp \
                  $${PROJ_SOURCES_DIR}/tree.cpp

SOURCES        += $${EXPRESS_DIR}/EGrammar/EGrammarBaseListener.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarLexer.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarListener.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarParser.cpp


Release:LIBS += "$${EXPRESS_DIR}/antlr4/lib/vs-2015/x64/Release DLL/antlr4-runtime.lib"
Debug::LIBS += "$${EXPRESS_DIR}/antlr4/lib/vs-2015/x64/Debug DLL/antlr4-runtime.lib"


# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/syntaxhighlighter

Release:DESTDIR += $${EXPRESS_DIR}/express_ex/Release
Debug:DESTDIR += $${EXPRESS_DIR}/express_ex/Debug



#DESTDIR += C:\Express_expr_compiler\compiler_ex\compiler_ex\x64\Release
INSTALLS += target

RESOURCES += \
    resource.qrc


#QMAKE_CXXFLAGS_DEBUG += /MT
