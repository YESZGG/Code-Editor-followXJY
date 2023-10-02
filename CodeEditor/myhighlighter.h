#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>

class MyHighlighter : public QSyntaxHighlighter
{
public:
    // 避免隐式构造
    explicit MyHighlighter(QTextDocument *parent = nullptr, QString fontFamily = "Consolas", int fontSize = 12);
    void setFont(QFont font);
protected:
    void highlightBlock(const QString &text);
private:
    QString mFontFamily;
    int mFontSize;

    struct HighlighterRule{
        QRegExp pattern; // 匹配的方式
        QTextCharFormat format;
    };
    QVector<HighlighterRule> highlighterRules;

    void addNormalTextFormat();
    void addNumberFormat();
    void addStringFormat();
    void addCommentFormat();
    void addKeywordsFormat();
    void addMultiLineCommentFormat(const QString &text);
    void addClassNameFormat();
    void addFunctionFormat();
};

#endif // MYHIGHLIGHTER_H
