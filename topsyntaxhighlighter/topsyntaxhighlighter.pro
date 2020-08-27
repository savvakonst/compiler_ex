QT += widgets
#requires(qtConfig(filedialog))
#CONFIG += c++11

INCLUDEPATH     =..\syntaxhighlighter

HEADERS         = ..\syntaxhighlighter\highlighter.h \
                  ..\syntaxhighlighter\highlightStyle.h \
                  ..\syntaxhighlighter\textEdit.h \
                  mainWindow.h

SOURCES         = main.cpp \
                  mainWindow.cpp



DEFINES += QUIMONO_EXE

#Release:DEPENDPATH += "$$PWD/../express_ex/Release"
#Debug:DEPENDPATH  += "$$PWD/../express_ex/Debug"


Release:LIBS += "$$PWD/../express_ex/Release/syntaxhighlighter.lib"
Debug::LIBS += "$$PWD/../express_ex/Debug/syntaxhighlighterd.lib"



Release:DESTDIR += $$PWD/../topSyntax/Release
Debug:DESTDIR += $$PWD/../topSyntax/Debug



#Release:DESTDIR += $$PWD/../express_ex/Release
#Debug:DESTDIR += $$PWD/../express_ex/Debug





Debug:Dll   +=  "$$PWD/../express_ex/Debug/*.dll"
Release:Dll +=  "$$PWD/../express_ex/Release/*.dll"

QMAKE_POST_LINK += xcopy /d/y  \"$$replace( Dll , /,\\ )\"   \"$$replace( DESTDIR, /,\\ )\"


INSTALLS += target

RESOURCES += \
    resource.qrc



