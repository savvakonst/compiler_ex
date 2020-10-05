#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "highlighter.h"
#include <QMainWindow>
#include <QtWidgets>

class KexEdit;
class TemplateTextEdit;
class SearchWidget : public QWidget {
    Q_OBJECT
public:
    SearchWidget(QWidget* parent = nullptr):QWidget(parent){

        line = new QLineEdit(this);

        auto newWidth=line->width()+line->height()*6;
        resize(newWidth,line->height()+hihghShift*2);

        QRect cr = contentsRect();

        crossButton=newButton(0,":/icons/cross.png");
        downButton=newButton(1,":/icons/downArrow.png");
        upButton=newButton(2,":/icons/upArrow.png");

        connect(crossButton, SIGNAL (clicked()),this->parent(), SLOT (hideSearchWidgetAndSetFocus()));
        connect(downButton, SIGNAL (clicked()),this->parent(), SLOT (findDown()));
        connect(upButton, SIGNAL (clicked()),this->parent(), SLOT (findUp()));


        this->setStyleSheet("QWidget{background-color: #F0F0F0;}");
        line->setStyleSheet("QWidget{background-color: #F0F0F0;border: 1px solid black;}");
        line->setGeometry(cr.left()+buttonShift, cr.top()+hihghShift, newWidth-line->height()*3-buttonShift*2, line->height());

        connect(line, SIGNAL (textChanged(QString )),this->parent(), SLOT (highlitText( QString )));
        //connect(line, SIGNAL (returnPressed()),this->parent(), SLOT (findDown( )));
    }

    ~SearchWidget(){
        delete line;
        delete upButton;
        delete downButton;
        delete crossButton;
    }

private:
    QPushButton* newButton(int index,QString icon){
        QPushButton* button = new QPushButton(this);

        auto bstyle =QString(
                    "QPushButton:pressed {"
                    //"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,   stop:0 rgba(60, 186, 162, 255), stop:1 rgba(98, 211, 162, 255))"
                    "     background-color: #F0F0F0; border: 1px solid black;"
                    "}"
                    "QPushButton {"
                    "     background-color: #F0F0F0;"// border: 1px solid black;"
                    "     border-radius: 0px;"
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: rgb(170, 170, 127)"
                    "}"
                    );

        QRect cr = contentsRect();
        button->setGeometry(cr.right()-line->height()+buttonShift-index*line->height(),cr.top()+hihghShift+buttonShift,line->height()-buttonShift*2,line->height()-buttonShift*2);
        button->setStyleSheet(bstyle);
        button->setIcon(QIcon(icon));
        return button ;
    }

public :
    inline void setFocus() { line->setFocus(); }
    inline QString text() { return line->text(); }
    inline bool hasFocus() { return line->hasFocus()|| QWidget::hasFocus(); }
private:
    const int hihghShift=2;
    const int buttonShift=2;

    QLineEdit *line;
    QPushButton *upButton;
    QPushButton *downButton;
    QPushButton *crossButton;
};



class LineNumberArea : public QWidget
{
public:
    LineNumberArea(KexEdit *editor);
    QSize sizeHint() const override  ;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    KexEdit *editor;
};


class TemplateTextEdit :public QPlainTextEdit{
    Q_OBJECT
public:
    TemplateTextEdit(): QPlainTextEdit() {
        runOutputHighlighter=new SearchHighLight(document());
        QRect cr = contentsRect();
        searchWidget=new SearchWidget(this);
        searchWidget->hide();
        auto rightOffset=verticalScrollBar()->isVisible()?verticalScrollBar()->width():0;

        searchWidget->setGeometry(
                    QRect(
                        cr.right()-searchWidget->width()-rightOffset,
                        cr.top(),
                        searchWidget->width(),
                        searchWidget->height()));

        setLineWrapMode(NoWrap);
    }
    ~TemplateTextEdit(){
        delete   runOutputHighlighter;
        delete   searchWidget;
    }

    bool isSelectd(){
        return hasFocus()||searchWidget->hasFocus();
    }

protected:
        /*
        void contextMenuEvent(QContextMenuEvent *e) override { //example. do nothing
            QMenu *menu = createStandardContextMenu();
            menu->addAction(tr("hide"),this, SLOT(showSearchWidget()), QKeySequence::Find);
            menu->addAction(tr("show"),this, SLOT(showSearchWidget()));
            menu->exec(e->globalPos());
            delete menu;
        }*/

    void keyPressEvent(QKeyEvent* event) override {

        if(!hasFocus()){
            auto key =event->key();

            if (((key==Qt::Key_Enter)||(key==Qt::Key_Return)))
                findDown();
            else if (key==Qt::Key_Escape)
                hideSearchWidgetAndSetFocus();
            else
                return;
        }
        else
            QPlainTextEdit::keyPressEvent(event);
    }


    void resizeEvent(QResizeEvent *e) override{
        QPlainTextEdit::resizeEvent(e);
        QRect cr = contentsRect();

        auto rightOffset=verticalScrollBar()->isVisible()?verticalScrollBar()->width():0;

        searchWidget->setGeometry(
                    QRect(
                        cr.right()-searchWidget->width()-rightOffset,
                        cr.top(),
                        searchWidget->width(),
                        searchWidget->height()));
    }

public Q_SLOTS:
    void showSearchWidget(){
        searchWidget->show();
        this->clearFocus();
        searchWidget->setFocus();
        highlitText(searchWidget->text());
    }

    void hideSearchWidget(){
        searchWidget->hide();
    }

    void hideSearchWidgetAndSetFocus(){
        searchWidget->hide();
        setFocus();
        highlitText("");
    }

    virtual void highlitText(const QString &text){
        runOutputHighlighter->searchText(text);
    }



    void findDown(){
        searchWidget->text();
        findString(searchWidget->text(),false,false,false);
    }

    void findUp(){
        searchWidget->text();
        findString(searchWidget->text(),true,false,false);
    }

private:
    void findString(QString s, bool reverse, bool casesens, bool words)
    {
        QTextDocument::FindFlags flag;
        if (reverse) flag |= QTextDocument::FindBackward;
        if (casesens) flag |= QTextDocument::FindCaseSensitively;
        if (words) flag |= QTextDocument::FindWholeWords;

        QTextCursor cursor = this->textCursor();
        // here , you save the cursor position
        QTextCursor cursorSaved = cursor;

        if (!find(s, flag))
        {
            //nothing is found | jump to start/end
            cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

            /* following line :
            - the cursor is set at the beginning/end of the document (if search is reverse or not)
            - in the next "find", if the word is found, now you will change the cursor position
            */
            setTextCursor(cursor);

            if (!find(s, flag))
            {
                //setTextCursor(cursorSaved);
            }
        }
    }

protected:
    SearchHighLight*    runOutputHighlighter;
private:
    SearchWidget *      searchWidget;
};


class HighlightStyle;

class KexEdit:public TemplateTextEdit{
    Q_OBJECT
public:
    KexEdit();
    ~KexEdit();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

public Q_SLOTS:

    void rehighlight();

    virtual void highlitText(const QString &text) override{
        highlighter->setPattern(text);
        rehighlight();
    }

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

protected:
    //void changeEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent* event) override;

    void changeEvent(QEvent *event) override;

    void keyPressEvent(QKeyEvent* event) override ;

Q_SIGNALS:
    void uncorrect_syntax(QString);
    void correct_syntax();

private:
    bool rehighlightInProgress= false;
    bool spacesInsteadOfTabs  = true;
    bool isInvalidSyntax      = false;
    int  tabWidth=2;

    Highlighter*        highlighter;
    HighlightStyle*     highlightStyle;
    QWidget*            lineNumberArea;
};

class RunOutputBrowser:public TemplateTextEdit{
Q_OBJECT
public:
    RunOutputBrowser(): TemplateTextEdit() {
    }
};

#endif
