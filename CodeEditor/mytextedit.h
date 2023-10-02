#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>
#include <QFont>
#include <QTextEdit>

namespace Ui {
class MyTextEdit;
}

class MyTextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;


private slots:
    void textEditHScrollBarChanged();
    void scrollBarChanged();

    void textEditVScrollBarChanged();
    void textBrowserVScrollBarChanged();
    void onTextChanged();
    // 当前行高亮显示
    void highlightCurrentline();
private:
    Ui::MyTextEdit *ui;

    int tabSpacesCount;
    QFont mFont;
    void initConnect();
    void initFont();
    void initHighlighter();
};

#endif // MYTEXTEDIT_H
