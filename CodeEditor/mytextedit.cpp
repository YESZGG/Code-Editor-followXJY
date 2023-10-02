#include "myhighlighter.h"
#include "mytextedit.h"
#include "ui_mytextedit.h"
#include <QDebug>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    tabSpacesCount = 4; // 设置每个Tab键的空格数量

    // 初始化关联
    initConnect();

    // 初始化字体
    initFont();

    // 初始化高亮
    initHighlighter();

    // 当前行高亮显示
    highlightCurrentline();
}

MyTextEdit::~MyTextEdit()
{
    delete ui;
}

bool MyTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->textEdit && event->type() == QEvent::KeyPress){
        // 将事件转换为按键事件
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        // 检测是否按下了Tab键
        if (keyEvent->key() == Qt::Key_Tab){
            // 获取当前的光标位置
            QTextCursor cursor = ui->textEdit->textCursor();
            int position = cursor.position();
            // 计算出插入空格后新的光标位置
            int newPosition = ((position + tabSpacesCount) / tabSpacesCount) * tabSpacesCount;
            // 获取文本的长度
            int textLength = ui->textEdit->toPlainText().length();

            // 如果新位置超出文本范围，则将新位置设置为文本末尾
            newPosition = qMin(newPosition, textLength);

            // 构造指定数量的空格字符串
            QString spaces(tabSpacesCount, QChar(' '));
            // 在文本框中插入空格字符串
            ui->textEdit->insertPlainText(spaces);
            // 将光标移动到新的位置处
            cursor.setPosition(newPosition + tabSpacesCount);
            ui->textEdit->setTextCursor(cursor);
            // 返回true以拦截Tab键事件，不再向下传递
            return true;
        }
    }

    // 返回false以继续向下传递事件
    return false;
}


void MyTextEdit::initConnect()
{
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
    // 水平滚动条关联
    connect(ui->textEdit->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditHScrollBarChanged()));
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),this,SLOT(scrollBarChanged()));

    connect(ui->textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditVScrollBarChanged()));
    connect(ui->textBrowser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textBrowserVScrollBarChanged()));
    // 光标
    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentline()));
}

void MyTextEdit::initFont()
{
    // 设置字体为Consolas，大小为12
    mFont = QFont("Consolas", 12);

    // 将编辑框的字体设置为mFont
    ui->textEdit->setFont(mFont);

    // 创建一个文本块格式对象format，并设置行高为当前字体高度的1.1倍，固定行高方式
    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height() * 1.1, QTextBlockFormat::FixedHeight);

    // 获取编辑框的文本光标
    QTextCursor cursor = ui->textEdit->textCursor();

    // 选中整个文档
    cursor.select(QTextCursor::Document);

    // 合并文本块格式到选中的文本处
    cursor.mergeBlockFormat(format);

    // 将浏览器的字体设置为mFont
    ui->textBrowser->setFont(mFont);
}

void MyTextEdit::initHighlighter()
{
    new MyHighlighter(ui->textEdit->document(),"Microsoft YaHei",12);
}
/*
 *  对于垂直滚动条， value表示滑块的顶部位置所代表的数值。
 *  对于水平滚动条， value表示滑块的左侧位置所代表的数值。
 *  pagestep 用来表示在单次大步操作（如点击滚动条空白处或按下滚动条增加/减少按钮）中滑块移动的距离。 它控制了滚动条的快速滚动行为
*/
void MyTextEdit::textEditHScrollBarChanged() // textEdit的滚动条绑定 ui的滚动条
{
    // 最小和最大值绑定成一样
    ui->horizontalScrollBar->setMinimum(ui->textEdit->horizontalScrollBar()->minimum());
    ui->horizontalScrollBar->setMaximum(ui->textEdit->horizontalScrollBar()->maximum());
    // 滚动条长度比例一样
    ui->horizontalScrollBar->setPageStep(ui->textEdit->horizontalScrollBar()->pageStep());
    ui->horizontalScrollBar->setValue(ui->textEdit->horizontalScrollBar()->value());

}

// ui 的滚动条
void MyTextEdit::scrollBarChanged()
{
    // 实现与 textEdit 的滚动条的双向绑定
    ui->textEdit->horizontalScrollBar()->setValue(ui->horizontalScrollBar->value());
}

/*
    当文本编辑框的垂直滚动条值发生变化时，该函数会被调用。
    这个函数将文本浏览器的垂直滚动条值设置为与文本编辑框相同的值，从而实现滚动条同步。

*/
void MyTextEdit::textEditVScrollBarChanged()
{
    // 将文本编辑框的垂直滚动条值设置为文本浏览器的垂直滚动条值
    ui->textBrowser->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->value());
}

/*
    当文本浏览器的垂直滚动条值发生变化时，该函数会被调用。
    这个函数将文本编辑框的垂直滚动条值设置为与文本浏览器相同的值，实现滚动条的同步。
*/
void MyTextEdit::textBrowserVScrollBarChanged()
{
    // 将文本浏览器的垂直滚动条值设置为文本编辑框的垂直滚动条值
    ui->textEdit->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->value());
}


void MyTextEdit::onTextChanged()
{
    // 获取文本编辑框中的行数
    int lineCountOfTextEdit = ui->textEdit->document()->lineCount();

    // 初始化一个空字符串来保存生成的文本
    QString text = ui->textBrowser->toPlainText();
    int lineCountOfTextBrowser = text.trimmed().split("\n").length();

    // 如果文本浏览器的行数大于文本编辑框的行数
    if (lineCountOfTextBrowser > lineCountOfTextEdit)
    {
        // 删除多余的行号
        for (int i = lineCountOfTextBrowser; i > lineCountOfTextEdit; i--)
        {
            text.chop((QString::number(i)+"\n").length());
        }
    }
    else if (lineCountOfTextBrowser == 1 && text.length() < 1) // 如果文本浏览器只有一行空白行
    {
        // 原始文本为空，添加第一行行号
        text += "1\n";
    }
    else if (lineCountOfTextBrowser < lineCountOfTextEdit) // 如果文本浏览器的行数小于文本编辑框的行数
    {
        // 添加缺失的行号
        for (int i = lineCountOfTextBrowser; i < lineCountOfTextEdit; i++)
        {
            text += QString::number(i+1) + "\n";
        }
    }

    // 遍历每一行，生成行号并添加到字符串中
//    for (int i = 0; i < lineCount; i++)
//    {
//        // 将当前行号转换为字符串，并添加到文本末尾
//        text += QString::number(1+i) + "\n";
//    }
    // for循环，当行号大时，循环次数就变大，所以会有延迟


    // 根据行号的数量调整文本浏览器的最大宽度 默认宽度是 25 ，自适应 就是 原来的宽度加上行号的数字长度 * 7,每个字符宽度为7
    // 调整文本浏览器的最大宽度，以适应行号的长度
    ui->textBrowser->setMaximumWidth(25 + QString::number(lineCountOfTextEdit).length() * 7);

    // 将处理结果设置为文本浏览器（textBrowser）控件的文本
    ui->textBrowser->setText(text);

    QTextBlockFormat format;
    format.setLineHeight(QFontMetrics(mFont).height()*1.1, QTextBlockFormat::FixedHeight);
    format.setAlignment(Qt::AlignRight);
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
}

void MyTextEdit::highlightCurrentline()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(21,180,255,51));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor = ui->textEdit->textCursor();
    selection.cursor.clearSelection(); // 选中多行时，只在光标位置高亮

    extraSelections.append(selection);

    ui->textEdit->setExtraSelections(extraSelections);
}



