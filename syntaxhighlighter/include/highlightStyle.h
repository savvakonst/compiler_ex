#ifndef HIGHLIGHTSTYLE_H
#define HIGHLIGHTSTYLE_H
#include <QtWidgets>

/*
#ifdef _MSC_VER
#  ifdef QUILIB_LIBRARY
#    define QUILIBSHARED_EXPORT __declspec(dllexport)
#  elif QUIMONO_EXE
#    define QUILIBSHARED_EXPORT
#  else
#    define QUILIBSHARED_EXPORT __declspec(dllimport)
#  endif
*/
//#else
#  ifdef QUILIB_LIBRARY
#    define QUILIBSHARED_EXPORT Q_DECL_EXPORT
#  elif QUIMONO_EXE
#    define QUILIBSHARED_EXPORT
#  else
#    define QUILIBSHARED_EXPORT Q_DECL_IMPORT
#  endif
//#endif


class QUILIBSHARED_EXPORT  HighlightStyle{
public:
    HighlightStyle(){

        auto error_color_fd = QColor("#B53830");
        //auto error_color_fd = QColor("#FFFFFF");

        auto red_fd      = QColor("#E06C60");
        auto yellow_fd   = QColor("#D19A66");
        auto pink_fd     = QColor("#C66CAC");
        auto blue_fd     = QColor("#5EADEF");
        auto h_yellow_fd = QColor("#E5C07B");
        auto green_fd    = QColor("#98C379");
        auto water_fd    = QColor("#008080");

        //const QBrush kConstBrush        =Qt::darkBlue;
        const QBrush kConstBrush        =QBrush(red_fd);

        const QBrush kIdBrush           =QBrush(yellow_fd);

        //const QBrush kFuncBrush         =QBrush(QColor("#960F93"));
        const QBrush kFuncBrush         =QBrush(pink_fd);
        //const QBrush kBuiltInFuncBrush  =QBrush(QColor("#7F7942"));
        const QBrush kBuiltInFuncBrush  =QBrush(h_yellow_fd);
        //const QBrush specSymbolBrush    =QBrush(QColor("#0031C8"));
        const QBrush specSymbolBrush    =QBrush(blue_fd);
        //const QBrush stringLiteralBrush =QBrush(QColor("#008000"));
        const QBrush stringLiteralBrush =QBrush(green_fd);

        stringLiteralFormat.setForeground(stringLiteralBrush);

        constFormat.setForeground(kConstBrush);

        idFormat.setForeground(kIdBrush);
        unknownIdFormat.setForeground(kIdBrush);
        unknownIdFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::SingleUnderline);
        unknownIdFormat.setUnderlineColor(error_color_fd);
        //unknownIdFormat.setFontUnderline(true);
        //unknownIdFormat.setBackground(red_fd);

        funcFormat.setForeground(kFuncBrush);
        unknownFuncFormat.setForeground(kFuncBrush);
        unknownFuncFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::SingleUnderline);
        unknownFuncFormat.setUnderlineColor(error_color_fd);
        //unknownFuncFormat.setFontUnderline(true);

        builtInFuncFormat.setForeground(kBuiltInFuncBrush);

        specSymbolFormat.setForeground(specSymbolBrush);
    }

    QTextCharFormat   constFormat;
    QTextCharFormat   idFormat;
    QTextCharFormat   unknownIdFormat;
    QTextCharFormat   funcFormat;
    QTextCharFormat   unknownFuncFormat;
    QTextCharFormat   builtInFuncFormat;
    QTextCharFormat   specSymbolFormat;
    QTextCharFormat   stringLiteralFormat;

};
#endif
