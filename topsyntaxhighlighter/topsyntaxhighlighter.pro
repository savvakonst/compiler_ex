QT += widgets
#requires(qtConfig(filedialog))
CONFIG += c++14

include(config.pri)


EXPRESS_DIR = $$PWD/..
SYNTAXHIGHLIGHTER_DIR = $${EXPRESS_DIR}/syntaxhighlighter

INCLUDEPATH     = $${SYNTAXHIGHLIGHTER_DIR}/include

HEADERS         = $${SYNTAXHIGHLIGHTER_DIR}/include/highlighter.h \
                  $${SYNTAXHIGHLIGHTER_DIR}/include/highlightStyle.h \
                  $${SYNTAXHIGHLIGHTER_DIR}/include/textEdit.h


HEADERS        += include/mainWindow.h

SOURCES         = sources/main.cpp \
                  sources/mainWindow.cpp



DEFINES += QUIMONO_EXE
#DEFINES += _$${_CONFIG}


LIBS     += "$${EXPRESS_DIR}/express_ex/$${_PLATFORM}/$${_CONFIG}/syntaxhighlighter$${LIBPOSTFIX}.lib"
DESTDIR   = $${EXPRESS_DIR}/express_ex/$${_PLATFORM}/$${_CONFIG}



INSTALLS += target

RESOURCES += \
    resource.qrc



