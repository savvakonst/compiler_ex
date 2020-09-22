QT += widgets
#requires(qtConfig(filedialog))
#CONFIG += c++11


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

#Release:DEPENDPATH += "$$PWD/../express_ex/Release"
#Debug:DEPENDPATH  += "$$PWD/../express_ex/Debug"


Release:LIBS += "$${EXPRESS_DIR}/express_ex/Release/syntaxhighlighter.lib"
Debug::LIBS += "$${EXPRESS_DIR}/express_ex/Debug/syntaxhighlighterd.lib"



#Release:DESTDIR += $${EXPRESS_DIR}/topSyntax/Release
#Debug:DESTDIR += $${EXPRESS_DIR}/topSyntax/Debug



Release:DESTDIR = $${EXPRESS_DIR}/express_ex/Release
Debug:DESTDIR = $${EXPRESS_DIR}/express_ex/Debug


#Debug:Dll   +=  "$$PWD/../express_ex/Debug/*.dll"
#Release:Dll +=  "$$PWD/../express_ex/Release/*.dll"

#QMAKE_POST_LINK += xcopy /d/y  \"$$replace( Dll , /,\\ )\"   \"$$replace( DESTDIR, /,\\ )\"


INSTALLS += target

RESOURCES += \
    resource.qrc



