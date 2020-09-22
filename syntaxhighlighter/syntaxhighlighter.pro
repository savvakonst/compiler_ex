QT += widgets
#requires(qtConfig(filedialog))
#CONFIG += c++11

DEFINES += QUILIB_LIBRARY

TARGET = $$qtLibraryTarget(syntaxhighlighter)
TEMPLATE = lib

EXPRESS_DIR = $$PWD/express

INCLUDEPATH     = $${EXPRESS_DIR}/antlr4/antlr4-runtime \
                  $${EXPRESS_DIR}/EGrammar

HEADERS         = highlighter.h \
                  highlightListener.h \
                  highlightStyle.h \
                  mainWindow.h \
                  textEdit.h \
                  tree.h

SOURCES         = highlighter.cpp \
                  highlightListener.cpp \
                  main.cpp \
                  mainWindow.cpp \
                  textEdit.cpp \
                  tree.cpp

SOURCES        += $${EXPRESS_DIR}/EGrammar/EGrammarBaseListener.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarLexer.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarListener.cpp \
                  $${EXPRESS_DIR}/EGrammar/EGrammarParser.cpp


Release:LIBS += "$${EXPRESS_DIR}/antlr4/lib/vs-2015/x64/Release DLL/antlr4-runtime.lib"
Debug::LIBS += "$${EXPRESS_DIR}/antlr4/lib/vs-2015/x64/Debug DLL/antlr4-runtime.lib"


# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/syntaxhighlighter

Release:DESTDIR += $$PWD/../express_ex/Release
Debug:DESTDIR += $$PWD/../express_ex/Debug



#DESTDIR += C:\Express_expr_compiler\compiler_ex\compiler_ex\x64\Release
INSTALLS += target

RESOURCES += \
    resource.qrc


#QMAKE_CXXFLAGS_DEBUG += /MT
