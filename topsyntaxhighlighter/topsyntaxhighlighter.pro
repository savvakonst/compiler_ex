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

Release:LIBS += "$$PWD/../express_ex/Release/syntaxhighlighter.lib"
Debug::LIBS += "$$PWD/../express_ex/Debug/syntaxhighlighter.lib"


Release:DESTDIR += $$PWD/../express_ex/Release
Debug:DESTDIR += $$PWD/../express_ex/Debug


INSTALLS += target

RESOURCES += \
    resource.qrc



