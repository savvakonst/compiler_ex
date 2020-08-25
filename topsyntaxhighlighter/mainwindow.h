#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TextEdit.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public Q_SLOTS:
    void runScript();
    void about();
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile(const QString &path = QString());
    void saveAsFile(const QString &path = QString());
    void updateOutput();
    void closeOutput(int);
    void search();

    void outError(QString errTxt);

private Q_SLOTS:


private:
    void setupEditor();
    void setupRunOutput();
    void setupSearchWidget();
    void setupFileMenu();
    void setupRunMenu();
    void setupEditMenu();
    void setupHelpMenu();

    void addProperty(QString name);
    QString currentFileName =QString();

    RunOutputBrowser*   runOutput;

    KexEdit*           editor;
    QProcess*           currentProcess =NULL;

    QMap<QString,QCheckBox*> properties;


};

#endif
