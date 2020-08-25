#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "highlightStyle.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class TreeShapeListener;



class QUILIBSHARED_EXPORT SearchHighLight : public QSyntaxHighlighter
{
    Q_OBJECT
    using BaseClass = QSyntaxHighlighter;
public:
    explicit SearchHighLight(QTextDocument* parent = nullptr);

    void setPattern(const QString& text);
    void searchText(const QString& text);

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    QRegularExpression m_pattern; // Regular expression to search for, in our case, this word or text
    QTextCharFormat m_format;     // Text formatting, highlighting
};




class QUILIBSHARED_EXPORT Highlighter : public SearchHighLight
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

    void setListener(TreeShapeListener *l){listener=l;}
protected:
    void highlightBlock(const QString &text) override;

private:

    TreeShapeListener * listener=NULL;
};



#endif
