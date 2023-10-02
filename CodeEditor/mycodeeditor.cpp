#include "mycodeeditor.h"
#include "myhighlighter.h"
#include <QDebug>
#include <QScrollBar>

MyCodeEditor::MyCodeEditor(QWidget *parent, QFont font)
    : QPlainTextEdit{parent}
{
    lineNumberWidget = new LineNumberWidget(this);
    this->installEventFilter(this);
    // 绑定
    initConnection();
    // 高亮显示
    initHighlighter();
    // 初始化字体
    setAllFont(font);
    // 行高亮
    highlightCurrentline();
    // 设置边距
    updateLineNumberWidgetWidth();

    setLineWrapMode(QPlainTextEdit::NoWrap);
}

MyCodeEditor::~MyCodeEditor()
{
    delete lineNumberWidget;
}

void MyCodeEditor::setAllFont(QFont font)
{
    this->setFont(font);
    mHighlighter->setFont(font);
    // 设置完字体后更新显示行号的宽度
    updateLineNumberWidgetWidth();
}

void MyCodeEditor::initConnection()
{
    // 光标
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentline()));

    // textChanged 更新保存状态
    connect(this,SIGNAL(textChanged()),this,SLOT(updateSaveState()));

    // blockCount
    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberWidgetWidth()));

    // [signal] void QPlainTextEdit::updateRequest(const QRect &rect, int dy)
    connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberWidget(QRect,int)));
}

void MyCodeEditor::initHighlighter()
{
    mHighlighter = new MyHighlighter(this->document());
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    /*
     *  // 计算行号部件的宽度
        int lineCount = blockCount() + 1;  // 获取文本编辑框中的总行数，并加上额外的一行用于显示光标行号

        // 使用 QString::number 将行数转换为字符串，并计算字符串的长度乘以每个字符的宽度
        int digitWidth = fontMetrics().horizontalAdvance(QChar('0')); // 获取单个数字字符的宽度
        int lineNumberWidth = digitWidth * QString::number(lineCount).length();  // 计算行号的总宽度

        return 8 + lineNumberWidth;  // 返回行号部件的宽度，包括一个固定偏移量（8）和行号宽度
    */
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}

void MyCodeEditor::highlightCurrentline()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 创建一个 QTextEdit::ExtraSelection 对象 selection，并设置背景颜色为 QColor(21, 180, 255, 51)
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(21, 180, 255, 51));

    // 设置选中文本的宽度占满整行，使用setProperty函数设置属性
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    // 获取编辑框的文本光标
    selection.cursor = this->textCursor();

    // 清除选择，只在光标位置高亮，用于处理多行选中时，只在光标当前行高亮
    selection.cursor.clearSelection();

    // 将 ExtraSelection 对象添加到 extraSelections 列表中
    extraSelections.append(selection);

    // 设置编辑框的额外选择项
    this->setExtraSelections(extraSelections);
}

// 更新行号
void MyCodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if (dy != 0) {
        // 如果 dy 不为 0，表示文本内容在垂直方向上有滚动偏移量，则将行号部件按照偏移量进行滚动
        lineNumberWidget->scroll(0, dy);
    } else {
        // 如果 dy 为 0，表示没有垂直滚动偏移量，需要更新指定的矩形区域
        lineNumberWidget->update(0, rect.y(), getLineNumberWidgetWidth(), rect.height());
    }
}

// 更新行号宽度
void MyCodeEditor::updateLineNumberWidgetWidth()
{
    // 设置边距
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

// 更新保存状态
void MyCodeEditor::updateSaveState()
{
    isSaved = false;
}

// 更新行号
void MyCodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    // 使用绘图工具 QPainter 绘制行号部件的背景色
    QPainter painter(lineNumberWidget);

    // 使用填充函数 fillRect 填充矩形区域，使用 QColor(100, 100, 100, 51) 作为背景颜色
    painter.fillRect(event->rect(), QColor(100, 100, 100, 51));

    // 获取第一个可见文本块
    QTextBlock block = firstVisibleBlock();

    // 获取该文本块的块号
    int blockNumber = block.blockNumber();

    // 计算光标的top值
    int cursorTop = blockBoundingGeometry(textCursor().block()).translated(contentOffset()).top();

    // 计算第一个可见文本块的顶部位置
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    // 计算第一个可见文本块的底部位置
    int bottom = top + blockBoundingRect(block).height();

    while (block.isValid()&& top <= event->rect().bottom()) {
        // 设置画笔颜色
        painter.setPen(cursorTop == top ? Qt::black : Qt::gray);

        // 在行号部件中绘制当前块的行号
        painter.drawText(0, top, getLineNumberWidgetWidth() - 4, blockBoundingRect(block).height(), Qt::AlignRight, QString::number(blockNumber + 1));

        // 移动到下一个文本块
        block = block.next();

        // 更新 top 和 bottom 的值
        top = bottom;
        bottom = top + blockBoundingRect(block).height();

        // 块号递增
        blockNumber++;
    }
}

void MyCodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    // 根据点击位置计算所在行号（y 坐标除以字体高度），然后通过 findBlockByLineNumber 在文档中找到对应的文本块
    setTextCursor(QTextCursor(document()->findBlockByLineNumber(event->y() / fontMetrics().height() + verticalScrollBar()->value())));
}

void MyCodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Horizontal){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->delta());
    }else {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - event->delta());
    }
    event->accept();
}

bool MyCodeEditor::saveFile()
{
    QString fileName;
    if (mFileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this,"保存文件");
        mFileName = fileName;
    }
    else {
        fileName = mFileName;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"警告","无法保存此文件"+file.errorString()); // 返回上一次发生的设备错误的可读说明。
        return false;
    }
    //setWindowTitle(fileName);
    QTextStream out(&file);

    out << toPlainText();
    file.close();
    isSaved = true;
    return true;
}

// 另存为
bool MyCodeEditor::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,"另存文件");
    QFile file(fileName); // 需要输入fileName
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"警告","无法保存此文件"+file.errorString()); // 返回上一次发生的设备错误的可读说明。
        return false;
    }
    mFileName = fileName;
    QTextStream out(&file);
    QString text = toPlainText();
    out << text;
    file.close();
    isSaved = true;
    return true;
}

void MyCodeEditor::setFileName(QString fileName)
{
    mFileName = fileName;
}

QString MyCodeEditor::getFileName()
{
    return mFileName;
}

// 返回 isSaved 标志位
bool MyCodeEditor::checkSaved()
{
    return isSaved;
}


void MyCodeEditor::resizeEvent(QResizeEvent *event)
{
    // 调用基类 QPlainTextEdit 的 resizeEvent 函数处理默认的调整大小事件
    QPlainTextEdit::resizeEvent(event);

    // 设置行号部件的几何形状，相对于文本编辑框的 contentsRect
    lineNumberWidget->setGeometry(0, 0, getLineNumberWidgetWidth(), contentsRect().height());
}

bool MyCodeEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::KeyPress){
        // 将事件转换为按键事件
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        // 检测是否按下了Tab键
        if (keyEvent->key() == Qt::Key_Tab){
            // 获取当前的光标位置
            QTextCursor cursor = this->textCursor();
            int position = cursor.position();
            // 计算出插入空格后新的光标位置
            int newPosition = ((position + tabSpacesCount) / tabSpacesCount) * tabSpacesCount;
            // 获取文本的长度
            int textLength = this->toPlainText().length();

            // 如果新位置超出文本范围，则将新位置设置为文本末尾
            newPosition = qMin(newPosition, textLength);

            // 构造指定数量的空格字符串
            QString spaces(tabSpacesCount, QChar(' '));
            // 在文本框中插入空格字符串
            this->insertPlainText(spaces);
            // 将光标移动到新的位置处
            cursor.setPosition(newPosition + tabSpacesCount);
            this->setTextCursor(cursor);
            // 返回true以拦截Tab键事件，不再向下传递
            return true;
        }
    }

    // 返回false以继续向下传递事件
    return false;
}
