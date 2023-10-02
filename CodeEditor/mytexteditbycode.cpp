#include "myhighlighter.h"
#include "mytexteditbycode.h"


MyTextEditByCode::MyTextEditByCode(QWidget *parent):QWidget(parent)
{
    // 初始化 UI 控件
    initWidget();
    // 字体
    initFont();
    // 绑定
    initConnect();
    // 高亮显示
    initHighlighter();
    // 当前行高亮显示
    highlightCurrentline();
}

MyTextEditByCode::~MyTextEditByCode()
{
    delete textEdit;
    delete textBrowser;
    delete scrollBar;
}

// 初始化 UI 控件
void MyTextEditByCode::initWidget()
{
    QWidget * horizontalLayoutwidget = new QWidget();
    // 竖向布局
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0); // 设置控件间距为 0
    verticalLayout->setMargin(0); // 设置边距为 0，消除控件周围的额外空白区域，使其紧密贴合布局的边缘或相邻控件
    // 横向布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout(horizontalLayoutwidget);
    horizontalLayout->setSpacing(0); // 设置控件间距为 0
    horizontalLayout->setMargin(0);

    textEdit = new QTextEdit();
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setLineWrapMode(QTextEdit::NoWrap); // 设置不自动换行

    textBrowser = new QTextBrowser();
    textBrowser->setMaximumWidth(25);// 设置最大宽度为 25
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollBar = new QScrollBar();
    scrollBar->setOrientation(Qt::Horizontal);

    // 横向布局 行号显示 文本显示框
    horizontalLayout->addWidget(textBrowser);
    horizontalLayout->addWidget(textEdit);

    verticalLayout->addWidget(horizontalLayoutwidget);
    verticalLayout->addWidget(scrollBar);
}

void MyTextEditByCode::initFont()
{
    // 设置字体为Consolas，大小为12
    mFont = QFont("Consolas", 12);

    // 将编辑框的字体设置为mFont
    textEdit->setFont(mFont);

    // 创建一个文本块格式对象format，并设置行高为当前字体高度的1.1倍，固定行高方式
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);

    // 获取编辑框的文本光标
    QTextCursor cursor = textEdit->textCursor();

    // 选中整个文档
    cursor.select(QTextCursor::Document);

    // 合并文本块格式到选中的文本处
    cursor.mergeBlockFormat(format);

    // 将浏览器的字体设置为mFont
    textBrowser->setFont(mFont);
}

void MyTextEditByCode::initConnect()
{
    // text
    connect(textEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));

    // 滚动条
    connect(textEdit->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextEditHorizontalScroBallBarChanged()));
    connect(textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextEditVerticalScroBallBarChanged()));
    connect(scrollBar,SIGNAL(valueChanged(int)),this,SLOT(onScroBallBarChanged()));
    connect(textBrowser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onTextBrowserHorizontalScroBallBarChanged()));
    // 光标
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentline()));
}

void MyTextEditByCode::initHighlighter()
{
    new MyHighlighter(textEdit->document());
}

void MyTextEditByCode::highlightCurrentline()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 创建一个 QTextEdit::ExtraSelection 对象 selection，并设置背景颜色为 QColor(21, 180, 255, 51)
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(21, 180, 255, 51));

    // 设置选中文本的宽度占满整行，使用setProperty函数设置属性
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    // 获取编辑框的文本光标
    selection.cursor = textEdit->textCursor();

    // 清除选择，只在光标位置高亮，用于处理多行选中时，只在光标当前行高亮
    selection.cursor.clearSelection();

    // 将 ExtraSelection 对象添加到 extraSelections 列表中
    extraSelections.append(selection);

    // 设置编辑框的额外选择项
    textEdit->setExtraSelections(extraSelections);
}

void MyTextEditByCode::onTextEditHorizontalScroBallBarChanged()
{
    scrollBar->setMaximum(textEdit->horizontalScrollBar()->maximum());
    scrollBar->setMinimum(textEdit->horizontalScrollBar()->minimum());
    scrollBar->setPageStep(textEdit->horizontalScrollBar()->pageStep());
    scrollBar->setValue(textEdit->horizontalScrollBar()->value());
}

void MyTextEditByCode::onTextEditVerticalScroBallBarChanged()
{
    textBrowser->verticalScrollBar()->setMaximum(textEdit->verticalScrollBar()->maximum());
    textBrowser->verticalScrollBar()->setMinimum(textEdit->verticalScrollBar()->minimum());
    textBrowser->verticalScrollBar()->setPageStep(textEdit->verticalScrollBar()->pageStep());
    textBrowser->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value());
}

void MyTextEditByCode::onScroBallBarChanged()
{
    textEdit->horizontalScrollBar()->setValue(scrollBar->value());
}

void MyTextEditByCode::onTextBrowserHorizontalScroBallBarChanged()
{
    textEdit->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->value());
}

void MyTextEditByCode::onTextChanged()
{
    // 获取编辑框中文本的行数
    int lineCountOfTextEdit = textEdit->document()->lineCount();

    // 获取浏览器中纯文本的行数
    QString text = textBrowser->toPlainText();
    int lineCountOfTextBrowser = text.trimmed().split("\n").length();

    if (lineCountOfTextBrowser > lineCountOfTextEdit) {
        // 如果浏览器中的行数大于编辑框中的行数，则需要删除多余的行
        for (int i = lineCountOfTextBrowser; i > lineCountOfTextEdit; i--) {
            // 通过chop函数将字符串text末尾的行移除，行号使用QString::number(i)+"\n"来判断
            text.chop((QString::number(i) + "\n").length());
        }
    } else if (lineCountOfTextBrowser == 1 && text.length() < 1) {
        // 如果浏览器中只有一行并且长度小于1，则设置为包含一个行号为1的文本
        text = "1\n";
    } else if (lineCountOfTextBrowser < lineCountOfTextEdit) {
        // 如果浏览器中的行数小于编辑框中的行数，则需要添加缺少的行
        for (int i = lineCountOfTextBrowser; i < lineCountOfTextEdit; i++) {
            // 通过+=运算符将行号加入到字符串text的末尾，行号使用QString::number(i+1)+"\n"
            text += QString::number(i + 1) + "\n";
        }
    }
    textBrowser->setMaximumWidth(25+QString::number(lineCountOfTextEdit).length()*7);
    // 将更新后的文本设置给浏览器控件
    textBrowser->setText(text);

    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1, QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor = textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}

