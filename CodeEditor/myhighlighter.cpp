#include "myhighlighter.h"

#include <QTextStream>

MyHighlighter::MyHighlighter(QTextDocument *parent,QString fontFamily, int fontSize):QSyntaxHighlighter(parent)
{
    mFontFamily = fontFamily;
    mFontSize = fontSize;
    // 对于普通文本
    addNormalTextFormat();
    // 对于数字
    addNumberFormat();
    // 对于特殊字符串
    addStringFormat();
    // 对于注释
    addCommentFormat();
    // 对于关键字
    addKeywordsFormat();
    // 对于类名
    addClassNameFormat();
    // 对于函数名
    addFunctionFormat();
}

void MyHighlighter::setFont(QFont font)
{
    mFontFamily = font.family();
    mFontSize = font.pointSize();
}

void MyHighlighter::addNormalTextFormat()
{
    HighlighterRule rule;
    // 设置正则表达式匹配规则
    rule.pattern = QRegExp("[a-z0-9A-Z]+");

    QTextCharFormat normalTextFormat;

    normalTextFormat.setFont(QFont(mFontFamily, mFontSize));
    // setForeground() 方法设置前景色
    normalTextFormat.setForeground(QColor(0, 0, 0));

    rule.format = normalTextFormat;

    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);
}

void MyHighlighter::addNumberFormat()
{
    HighlighterRule rule;
    // 设置正则表达式匹配规则
    rule.pattern = QRegExp("\\b\\d+|\\d+\\.\\d+\\b");// 用于匹配整数或浮点数

    QTextCharFormat numberFormat;

    // 设置字体样式和大小
    numberFormat.setFont(QFont(mFontFamily, mFontSize));
    // setForeground() 方法设置前景色
    numberFormat.setForeground(QColor(250, 80, 50));

    rule.format = numberFormat;

    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);
}

void MyHighlighter::addStringFormat()
{

    QTextCharFormat stringFormat;

    // 设置字体样式和大小
    stringFormat.setFont(QFont(mFontFamily, mFontSize));
    // setForeground() 方法设置前景色
    stringFormat.setForeground(QColor(42,161,152));

    HighlighterRule rule;
    rule.format = stringFormat;

    // 匹配''
    // [^']*表示匹配除了单引号之外的任意字符,使用 ^ 表示取反操作， * 表示匹配前面的字符零次或多次
    rule.pattern = QRegExp("'[^']*'");
    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);

    // 匹配""
    // [^\"]*：表示匹配除了双引号之外的任意字符，使用 ^ 表示取反操作， * 表示匹配前面的字符零次或多次
    rule.pattern = QRegExp("\"[^\"]*\"");
    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);

    // 匹配``
    // [^] ：表示匹配除了反引号之外的任意字符，使用 ^  表示取反操作，` 表示匹配前面的字符零次或多次
    rule.pattern = QRegExp("`[^`]*`");
    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);
}

// 单行注释
void MyHighlighter::addCommentFormat()
{
    QTextCharFormat commentFormat;

    // 设置字体样式和大小
    commentFormat.setFont(QFont(mFontFamily, mFontSize));
    // setForeground() 方法设置前景色
    commentFormat.setForeground(QColor(42,161,152));
    commentFormat.setFontItalic(true);

    HighlighterRule rule;
    rule.format = commentFormat;

    // 匹配 // 123123412
    rule.pattern = QRegExp("\\/\\/.*$");
    // 将规则添加到高亮器规则列表
    highlighterRules.append(rule);

}

void MyHighlighter::addKeywordsFormat()
{
    QFile file(":/config/config/keywords.txt");
    QTextStream keywordsStream(&file);

    HighlighterRule rule;
    QTextCharFormat keywordsFormat;
    keywordsFormat.setFont(QFont(mFontFamily,mFontSize));
    QColor color(144, 19, 255, 127); // 创建 QColor 对象，设置颜色和透明度
    QBrush brush(color); // 使用 QColor 对象创建 QBrush 对象
    keywordsFormat.setForeground(brush);
    rule.format = keywordsFormat;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        keywordsStream.seek(0); // 从零开始
        QString line; // 当前行
        while (!keywordsStream.atEnd()) // 如果没有到文件结尾
        {
            line = keywordsStream.readLine();
            if (line != ""){
                rule.pattern = QRegExp("\\b"+line+"\\b");
                highlighterRules.append(rule);
            }
        }
        file.close(); // 关闭文件
    }
}

/* 多行注释
* 参数：text传引用
* 函数作用：多行注释的正则表达式的实现
*/
void MyHighlighter::addMultiLineCommentFormat(const QString &text)
{
    // currentBlock(); // 返回当前文本块的状态。如果未设置任何值，则返回的值为-1。
    setCurrentBlockState(0);// 代表这一行已经highlighter 了

    // /*
    QRegExp commentStartRegExp("/\\*");
    // */
    QRegExp commentEndRegExp("\\*/");

    // 高亮格式
    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setFont(QFont(mFontFamily,mFontSize));
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    multiLineCommentFormat.setFontItalic(true);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartRegExp.indexIn(text); // /* 的开始位置

    while (startIndex >= 0){
        int endIndex = commentEndRegExp.indexIn(text,startIndex); // */ 的结束位置
        int commentLength = 0;
        if (endIndex == -1){ // 没有结束位置
            setCurrentBlockState(1);
            commentLength = text.length()-startIndex;
        }else { // 有结束位置
            commentLength = endIndex-startIndex+commentEndRegExp.matchedLength();
        }
        // 设置格式
        setFormat(startIndex,commentLength,multiLineCommentFormat);
        startIndex = commentStartRegExp.indexIn(text,commentLength + startIndex);
    }
}

// 对于类名的注释
void MyHighlighter::addClassNameFormat()
{
    HighlighterRule rule;

    QTextCharFormat classNameFormat;

    classNameFormat.setFont(QFont(mFontFamily,mFontSize));
    classNameFormat.setForeground(QColor(255, 127, 71, 127));
    classNameFormat.setFontWeight(99);
    rule.format = classNameFormat;

    rule.pattern = QRegExp("\\b[A-Z]+\\w*");
    highlighterRules.append(rule);
}

// 对于函数名的注释
void MyHighlighter::addFunctionFormat()
{
    HighlighterRule rule;

    QTextCharFormat functionFormat;

    functionFormat.setFont(QFont(mFontFamily,mFontSize));
    functionFormat.setForeground(QColor(10, 113, 244, 127));

    rule.format = functionFormat;

    rule.pattern = QRegExp("\\w+\\(");
    highlighterRules.append(rule);
    rule.pattern = QRegExp("\\)");
    highlighterRules.append(rule);
}

// 按行转入
void MyHighlighter::highlightBlock(const QString &text)
{
    QRegExp regExp;
    foreach (const HighlighterRule &rule, highlighterRules) {
        regExp = rule.pattern;// 匹配正则表达式
        int index = regExp.indexIn(text);
        while (index >= 0) {
            int length =regExp.matchedLength();
            setFormat(index,length,rule.format);
            index = regExp.indexIn(text,index+length);
        }
    }
    // 对于多行注释
    addMultiLineCommentFormat(text);
}





