QT += widgets
#requires(qtConfig(filedialog))
#CONFIG += c++11

INCLUDEPATH     = ../antlr4/antlr4-runtime \
                  ../EGrammar

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

SOURCES        += ../EGrammar/EGrammarBaseListener.cpp \
                  ../EGrammar/EGrammarLexer.cpp \
                  ../EGrammar/EGrammarListener.cpp \
                  ../EGrammar/EGrammarParser.cpp


Release:LIBS += "$$PWD/../antlr4/lib/vs-2015/x64/Release DLL/antlr4-runtime.lib"
Debug::LIBS += "$$PWD/../antlr4/lib/vs-2015/x64/Debug DLL/antlr4-runtime.lib"


# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/syntaxhighlighter

Release:DESTDIR += $$PWD/../express_ex/Release
Debug:DESTDIR += $$PWD/../express_ex/Debug
#DESTDIR += C:\Express_expr_compiler\compiler_ex\compiler_ex\x64\Release
INSTALLS += target

RESOURCES += \
    resource.qrc


#QMAKE_CXXFLAGS_DEBUG += /MT
