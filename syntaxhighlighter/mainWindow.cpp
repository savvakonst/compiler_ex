

#include "mainWindow.h"
#include <QtWidgets>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    setupFileMenu();
    setupEditor();

    setCentralWidget(editor);

    setupRunOutput();

    setupRunMenu();
    setupEditMenu();
    setupHelpMenu();

    setWindowIcon(QIcon(":/icons/work.png"));
    setWindowTitle(tr("KEx editor"));
}

void MainWindow::about(){
    QMessageBox::about(this, tr("none"),
                tr("none"));
}

void MainWindow::newFile(){
    editor->clear();
    currentFileName=QString();
}

void MainWindow::openFile(const QString &path){
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "KSD Express Files (*.kex);; (*.ep);;Text files (*.txt)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QFile::ReadOnly | QFile::Text)){
            editor->setPlainText(file.readAll());
            currentFileName=fileName;
            file.close();
        }
    }
}

void MainWindow::saveFile(const QString &path){
    QString fileName = currentFileName;
    saveAsFile(fileName);
}

void MainWindow::saveAsFile(const QString &path){
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getSaveFileName(this, tr("Open File"), "",  "KSD Express Files (*.kex);; (*.ep);;text files (*.txt)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Text)){
            file.write(editor->toPlainText().toUtf8());
            file.close();
            currentFileName=fileName;
        }
    }
}

void MainWindow::setupEditor(){

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);

    font.setPointSizeF(11.0);

    editor = new KexEdit;
    editor->setFont(font);
    editor->setStyleSheet("QPlainTextEdit{"
                          "font-family:'Consolas';"
                          "color: #FFFFFF; "
                          //"font-size: 22px;"
                          "background-color: #282C34}"
                          );
}

void MainWindow::updateOutput(){

    auto a=currentProcess->readAll();
    //a=a.replace("\u001b[32m","</span><span style=\" font-size:11pt; font-weight:600; color:green;\">" ) ;
    a=a.replace("\u001b[31m","</span><span style=\" font-size:11pt; color:red;\">" ) ;
    a=a.replace("\u001b[32m","</span><span style=\" font-size:11pt; color:green;\">" ) ;
    a=a.replace("\u001b[0m","</span>" );
    a=a.replace("\n","<br>" );
    a="<pre>"+a+"</pre>";
    //qDebug()<<a;
    //Utils


    QTextCursor cursor = runOutput->textCursor();
    cursor.movePosition(QTextCursor::End);
    runOutput->setTextCursor(cursor);
    //runOutput->appendPlainText(a);
    //runOutput->insertPlainText(a);
    runOutput->appendHtml(a);
    runOutput->verticalScrollBar()->setValue(runOutput->verticalScrollBar()->maximum());
}

void MainWindow::closeOutput(int errCode){
    currentProcess->close();
}

void MainWindow::runScript(){

    QString fileName = currentFileName;
    if (fileName.isNull())
        return ;

    saveFile();

    if (currentProcess!=NULL){
        currentProcess->waitForFinished();
        disconnect(currentProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutput()));
        disconnect(currentProcess, SIGNAL(finished(int)),this, SLOT(closeOutput(int)));
        delete( currentProcess );
        currentProcess=NULL;
    }

    QString exeFileName("express_ex.exe");


    QStringList args={"-i", fileName,"-db", "A01_3.json" };

    for (auto i : properties){
        if(i->checkState())
            args.push_back("--"+i->text());
    }


    //runOutput->appendPlainText("\n\n./>>"+exeFileName+" "+args.join(" ")+"\n");
    runOutput->appendHtml("<pre>\n\n./>>"+exeFileName+" "+args.join(" ")+"\n"+"</pre>");
    currentProcess = new QProcess(this);

    connect(currentProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutput()));
    connect(currentProcess, SIGNAL(finished(int )),this, SLOT(closeOutput(int )));
    currentProcess->start(exeFileName,args);
}




void MainWindow::search(){

   if(editor->isSelectd())
       editor->showSearchWidget();
   else
       editor->hideSearchWidget();


   if(runOutput->isSelectd())
       runOutput->showSearchWidget();
   else
       runOutput->hideSearchWidget();
}


void MainWindow::setupRunOutput(){
    runOutput =new RunOutputBrowser();
    //runOutput->setReadOnly(true);
    runOutput->setStyleSheet("RunOutputBrowser{"
                             "font-family:'Consolas';"
                             "color: #FFFFFF; "
                             "background-color: #282C34}"
                             );
    QFont font;
    font.setFamily("Consolas");
    font.setFixedPitch(true);
    font.setPointSizeF(11.0);

    runOutput->setFont(font);

    QDockWidget *dockWidget = new QDockWidget(tr("run output"), this);
    dockWidget->setAllowedAreas( Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea );
    dockWidget->setWidget(runOutput);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

}


void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()), QKeySequence::Save);
    fileMenu->addAction(tr("&Save as..."), this, SLOT(saveAsFile()), QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence::Quit);
}


void MainWindow::addProperty(QString name)
{
    QCheckBox *pr =new QCheckBox(name);
    properties[name]=pr;
    addToolBar(name)->addWidget(pr);
}

void MainWindow::setupRunMenu()
{
    QMenu *editMenu = menuBar()->addMenu(tr("&Run"));
    QToolBar *editToolBar = addToolBar(tr("Run"));

    //menuBar()->addMenu(editMenu);



    QKeySequence keyRun(Qt::Key_F5);
    const QIcon runIcon = QIcon::fromTheme("run");//QIcon(":/images/run.png")
    QAction *runAct = new QAction(runIcon, tr("&run"), this);
    runAct->setShortcuts( {keyRun});
    runAct->setStatusTip(tr("run current script"));

    //connect(copyAct, &QAction::triggered, this, SLOT(runScript()));
    connect(runAct, &QAction::triggered, this, &MainWindow::runScript);
    editMenu->addAction(runAct);
    editToolBar->addAction(runAct);

    addToolBarBreak();
    addProperty("ansi");
    addProperty("nameList");
    addProperty("untypedFSR");
    addProperty("activeNameList");
    addToolBarBreak();
    addProperty("allFSR");
    addProperty("redusedFSR");
    addProperty("tableSSR");
    addProperty("llvmIRcode");
    properties["ansi"]->setChecked(true);
}



void MainWindow::setupEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(editMenu);

    editMenu->addAction(tr("&Find"), this, SLOT(search()), QKeySequence::Find);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, SLOT(about()));
}
