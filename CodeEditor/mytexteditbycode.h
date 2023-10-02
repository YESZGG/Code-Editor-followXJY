#ifndef MYTEXTEDITBYCODE_H
#define MYTEXTEDITBYCODE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QTextEdit>
#include <QScrollBar>

class MyTextEditByCode:public QWidget
{
    Q_OBJECT
public:
    explicit MyTextEditByCode(QWidget *parent = nullptr);
    ~MyTextEditByCode();
private slots:
    void onTextEditHorizontalScroBallBarChanged();
    void onTextEditVerticalScroBallBarChanged();
    void onScroBallBarChanged();
    void onTextBrowserHorizontalScroBallBarChanged();
    void onTextChanged();
    // 当前行高亮显示
    void highlightCurrentline();
private:
    QTextEdit *textEdit;
    QTextBrowser *textBrowser;
    QScrollBar *scrollBar;
    QFont mFont;
    // 初始化UI控件
    void initWidget();
    // 字体高亮
    void initFont();
    // 绑定
    void initConnect();
    // 高亮显示
    void initHighlighter();

};

#endif // MYTEXTEDITBYCODE_H
