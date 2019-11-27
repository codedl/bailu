#include "mysyntaxhighlighter.h"

mysyntaxhighlighter::mysyntaxhighlighter(QTextDocument *parent )
                                         :QSyntaxHighlighter(parent)
{

}

void mysyntaxhighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setBackground(Qt::green);
    QString pattern = "\\bchar\\b";
    QRegExp exp(pattern);
    int index = text.indexOf(exp);
    while(index >= 0){
        int length = exp.matchedLength();
        setFormat(index,length,format);
        index = text.indexOf(exp,index+length);
    }

}
