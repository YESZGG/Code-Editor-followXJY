#ifndef MYCODEEDITOR_H
#define MYCODEEDITOR_H

#include "myhighlighter.h"

#include <QPlainTextEdit>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

class LineNumberWidget;

class MyCodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MyCodeEditor(QWidget *parent = nullptr, QFont font = QFont("Consolas",12));
    ~MyCodeEditor();

    void setAllFont(QFont font);
    void lineNumberWidgetPaintEvent(QPaintEvent *event);
    void lineNumberWidgetMousePressEvent(QMouseEvent *event);
    void lineNumberWidgetWheelEvent(QWheelEvent *event);
    bool saveFile();
    bool saveAsFile();
    void setFileName(QString fileName);
    QString getFileName();
    bool checkSaved();
protected:
    void resizeEvent(QResizeEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

signals:

private slots:
    // 当前行高亮显示
    void highlightCurrentline();
    void updateLineNumberWidget(QRect rect,int dy);
    void updateLineNumberWidgetWidth();
    void updateSaveState();

private:
    LineNumberWidget * lineNumberWidget;
    MyHighlighter *mHighlighter;
    QString mFileName;
    bool isSaved = false;
    int tabSpacesCount = 4;
    // 初始化字体
    void initFont();
    // 绑定
    void initConnection();
    // 高亮显示
    void initHighlighter();
    // 获取自适应宽度
    int getLineNumberWidgetWidth();
};

class LineNumberWidget : public QWidget{
public:
    explicit LineNumberWidget(MyCodeEditor *editor = nullptr) : QWidget(editor){
        codeEditor = editor;
    }
protected:
    void paintEvent(QPaintEvent *event) override{
        // 把绘制任务提交给 MyCodeEditor
        codeEditor->lineNumberWidgetPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override{
        // 把鼠标点击任务提交给 MyCodeEditor
        codeEditor->lineNumberWidgetMousePressEvent(event);
    }
    void wheelEvent(QWheelEvent *event) override{
        // 把滚轮任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetWheelEvent(event);
    }


private:
    MyCodeEditor *codeEditor;
};

#endif // MYCODEEDITOR_H
