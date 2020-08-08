#include "highlightListener.h"
#include "textEdit.h"

KexEdit::KexEdit(): TemplateTextEdit() {
    highlighter = new Highlighter(this->document());
    highlightStyle =new HighlightStyle();
    connect(this, SIGNAL(textChanged()), this, SLOT(rehighlight()));

    lineNumberArea = new LineNumberArea(this);

    connect(this, &KexEdit::blockCountChanged, this, &KexEdit::updateLineNumberAreaWidth);
    connect(this, &KexEdit::updateRequest, this, &KexEdit::updateLineNumberArea);
    connect(this, &KexEdit::cursorPositionChanged, this, &KexEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setTabStopWidth(fontMetrics().width(' ') * tabWidth);
    delete runOutputHighlighter;
    runOutputHighlighter=highlighter;
}



KexEdit::~KexEdit() {
    //delete highlighter;
    //delete highlightStyle;
    //delete lineNumberArea;
}

void KexEdit::rehighlight(){
    //qDebug()<<"--------------------------- "<<"\n";
    if (rehighlightInProgress==true) return ;
    rehighlightInProgress=true;
    auto content=this->toPlainText().toStdString()+"\n";

    antlr4::ANTLRInputStream input(content);
    EGrammarLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    EGrammarParser parser(&tokens);

    parser.removeErrorListeners();
    EErrorListener errorListner(highlightStyle);
    parser.addErrorListener(&errorListner);


    antlr4::tree::ParseTree* tree=NULL;
    try{
        tree = parser.start();
    }catch(antlr4::NoViableAltException ){
        return;
    }


    TreeShapeListener listener(highlightStyle);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    highlighter->setListener(&listener);
    highlighter->rehighlight();
    rehighlightInProgress=false;
}

void KexEdit::wheelEvent(QWheelEvent* event) {
    TemplateTextEdit::wheelEvent(event);
    if ((event->modifiers() == Qt::ControlModifier)){
        if (event->delta()>0)
            zoomIn(1);
        else
            zoomOut(1);
    }
}

void KexEdit::changeEvent(QEvent *event) {
    TemplateTextEdit::changeEvent(event);
}

void KexEdit::keyPressEvent(QKeyEvent* event)  {
    if (spacesInsteadOfTabs && (event->key()==Qt::Key_Tab))
        insertPlainText(QString(tabWidth, ' '));
    else
        TemplateTextEdit::keyPressEvent(event);
}


void KexEdit::updateLineNumberAreaWidth(int /* newBlockCount */){

    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void KexEdit::updateLineNumberArea(const QRect &rect, int dy){

    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void KexEdit::resizeEvent(QResizeEvent *e){

    TemplateTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    setTabStopWidth(fontMetrics().width(' ') * tabWidth);
}

void KexEdit::highlightCurrentLine(){

    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        //QColor lineColor = QColor(Qt::yellow).lighter(160);

        //selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


void KexEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);


            painter.setFont(this->font());
            painter.drawText(0, top, lineNumberArea->width(),  qRound(blockBoundingRect(block).height()),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int KexEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}





LineNumberArea::LineNumberArea(KexEdit *editor) : QWidget(editor), editor(editor){}

QSize LineNumberArea::sizeHint() const {
    return QSize(editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}
