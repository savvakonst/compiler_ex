
#include "highlightListener.h"
#include "textEdit.h"

KexEdit::KexEdit(): TemplateTextEdit() {
    highlighter__ = new Highlighter(this->document());
    highlight_style_ =new HighlightStyle();
    connect(this, SIGNAL(textChanged()), this, SLOT(rehighlight()));

    line_number_area_ = new LineNumberArea(this);

    connect(this, &KexEdit::blockCountChanged, this, &KexEdit::updateLineNumberAreaWidth);
    connect(this, &KexEdit::updateRequest, this, &KexEdit::updateLineNumberArea);
    connect(this, &KexEdit::cursorPositionChanged, this, &KexEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setTabStopWidth(fontMetrics().width(' ') * tab_width_);
    delete run_output_highlighter_;
    run_output_highlighter_=highlighter__;
}



KexEdit::~KexEdit() {
    //delete highlighter__;
    //delete highlight_style_;
    //delete line_number_area_;
    for (auto i : namespase_list_)
        delete i;
}



void KexEdit::rehighlight(){
    //qDebug()<<"--------------------------- "<<"\n";
    if (rehighlight_in_progress_==true) return ;
    Q_EMIT start_rehighlight();

    rehighlight_in_progress_=true;
    auto content=this->toPlainText().toStdString()+"\n";

    antlr4::ANTLRInputStream input(content);
    EGrammarLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    EGrammarParser parser(&tokens);

    parser.removeErrorListeners();
    EErrorListener errorListner(highlight_style_);
    parser.addErrorListener(&errorListner);


    antlr4::tree::ParseTree* tree=NULL;
    try{
        tree = parser.start();
    }catch(antlr4::NoViableAltException ){
        return;
    }

    if (errorListner.hasError()){
        is_invalid_syntax_=true;
        Q_EMIT uncorrect_syntax(errorListner.getErrorMessage());
    }

    else if(is_invalid_syntax_){
        is_invalid_syntax_=false;
        Q_EMIT correct_syntax();
    }


    TreeShapeListener listener(highlight_style_, namespase_list_); 
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    highlighter__->setListener(&listener);
    highlighter__->rehighlight();
    rehighlight_in_progress_ = false;
}

void KexEdit::updateNamespace(QList<QString> &sources) {
    for(auto i : namespase_list_)
        delete i;
    for (auto i : sources)
        addToNamespace(i.toStdString());
}

void KexEdit::clearNamespace() {
    for (auto i : namespase_list_)
        delete i;
}

void KexEdit::addToNamespace(const std::string &content) {

    antlr4::ANTLRInputStream input(content);
    EGrammarLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    EGrammarParser parser(&tokens);

    parser.removeErrorListeners();
    EErrorListener errorListner(highlight_style_);
    parser.addErrorListener(&errorListner);


    antlr4::tree::ParseTree* tree=NULL;
    try {
        tree = parser.start();
    }
    catch (antlr4::NoViableAltException) {
        return;
    }

    if (errorListner.hasError()) {
        is_invalid_syntax_=true;
        Q_EMIT uncorrect_syntax(errorListner.getErrorMessage());
    }

    else if (is_invalid_syntax_) {
        is_invalid_syntax_=false;
        Q_EMIT correct_syntax();
    }

    TreeShapeListener listener(highlight_style_,namespase_list_); //??????????????????????????
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    for (auto i : namespase_list_)
        delete i;
    namespase_list_ = listener.getNamespaceList();
}

void KexEdit::highlitText(const QString &text){
        highlighter__->setPattern(text);
        rehighlight();
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
    if (spaces_instead_of_tabs_ && (event->key()==Qt::Key_Tab))
        insertPlainText(QString(tab_width_, ' '));
    else
        TemplateTextEdit::keyPressEvent(event);
}


void KexEdit::updateLineNumberAreaWidth(int /* newBlockCount */){

    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void KexEdit::updateLineNumberArea(const QRect &rect, int dy){

    if (dy)
        line_number_area_->scroll(0, dy);
    else
        line_number_area_->update(0, rect.y(), line_number_area_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void KexEdit::resizeEvent(QResizeEvent *e){

    TemplateTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    line_number_area_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    setTabStopWidth(fontMetrics().width(' ') * tab_width_);
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
    QPainter painter(line_number_area_);
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
            painter.drawText(0, top, line_number_area_->width(),  qRound(blockBoundingRect(block).height()),
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

void KexEdit::setHighlightStyle(HighlightStyle * h)
{
    delete highlight_style_ ;
    highlight_style_ = h ;
}





LineNumberArea::LineNumberArea(KexEdit *editor) : QWidget(editor), editor(editor){}

QSize LineNumberArea::sizeHint() const {
    return QSize(editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    editor->lineNumberAreaPaintEvent(event);
}
