QT += widgets

CONFIG += c++14

include(config.pri)

_OUTPUTDIR = $${_PLATFORM}/$${_CONFIG}



DEFINES += QUILIB_LIBRARY

TARGET = $$qtLibraryTarget(syntaxhighlighter)
TEMPLATE = lib

EXPRESS_DIR = $$PWD/..
ANTLR_RUNTIME_DIR = $${EXPRESS_DIR}/../common/antlr4/runtime/Cpp/runtime


PROJ_INCLUDE_DIR = include
PROJ_SOURCES_DIR = sources

INCLUDEPATH     = $${ANTLR_RUNTIME_DIR}/src \
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



LIBS    += "$${ANTLR_RUNTIME_DIR}/bin/vs-2019/$${_OUTPUTDIR} DLL/antlr4-runtime.lib"
DESTDIR += $${EXPRESS_DIR}/express_ex/$${_OUTPUTDIR}

ANTLR_Dll = "$${ANTLR_RUNTIME_DIR}/bin/vs-2019/$${_OUTPUTDIR} DLL/*.dll"
QMAKE_POST_LINK += xcopy /d/y  \"$$replace( ANTLR_Dll , /,\\ )\"   \"$$replace( DESTDIR, /,\\ )\"

QT_DEPENDENCYS = Qt5Core Qt5Widgets Qt5Gui

for(VAR, QT_DEPENDENCYS) {
    QT_Dll =  "\"$$[QT_INSTALL_BINS]/$${VAR}$${LIBPOSTFIX}.dll\""
    QMAKE_POST_LINK += & xcopy /d/y  $$replace( QT_Dll , /,\\ )   \"$$replace( DESTDIR, /,\\ )\"
}
unset(QT_Dll)

INSTALLS += target

RESOURCES += \
    resource.qrc


#QMAKE_CXXFLAGS_DEBUG += /MT
