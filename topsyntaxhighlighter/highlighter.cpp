
#include "highlightListener.h"
#include "highlighter.h"
//#include <QTextCharFormat>

SearchHighLight::SearchHighLight(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    // Set the backlight color
    m_format.setBackground(Qt::green);
    m_format.setForeground(Qt::black);
}

void SearchHighLight::highlightBlock(const QString& text)
{
    // Using a regular expression, we find all matches.
    QRegularExpressionMatchIterator matchIterator = m_pattern.globalMatch(text);
    while (matchIterator.hasNext())
    {
        // Highlight all matches
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_format);
    }
}

void SearchHighLight::setPattern(const QString& text)
{
    m_pattern = QRegularExpression(text);
}

void SearchHighLight::searchText(const QString& text)
{
    // Set the text as a regular expression.
    m_pattern = QRegularExpression(text);
    rehighlight(); // Restart the backlight
}




Highlighter::Highlighter(QTextDocument *parent): SearchHighLight(parent){
    disconnect(document(), SIGNAL(contentsChange(int,int,int)),
               this, SLOT(_q_reformatBlocks(int,int,int)));
}

void Highlighter :: highlightBlock(const QString &text){
    if (listener==NULL) return ;
    int lineNumber = this->currentBlock().blockNumber()+1;

    for( auto i :listener->formatMap[lineNumber] ){
        setFormat(i.start,i.count,*i.format);
    }

    SearchHighLight::highlightBlock(text);
}

