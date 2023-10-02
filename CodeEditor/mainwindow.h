#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mycodeeditor.h"

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFont>
#include <QFontDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QDebug>
#include <QSettings> // 实现配置文件的存储
#include <QList>
#include <QMenu>
#include <QSet>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_new_file_triggered();

    void on_open_file_triggered();

    void on_save_file_triggered();

    void on_save_as_triggered();

    void on_paste_triggered();

    void on_cut_triggered();

    void on_copy_triggered();

    void on_font_triggered();

    void on_about_triggered();

    void on_undo_triggered();

    void on_redo_triggered();

    void on_exit_triggered();

    void on_print_triggered();

    void on_clear_history_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void saveSuccessAction(MyCodeEditor * codeEditor);

private:
    Ui::MainWindow *ui;

    QString mFontFamily; // 字体样式
    int mFontSize;       // 字体大小
    QSettings * mSettings;

    void initMenu(); // 初始化菜单
    void initFont(); // 初始化字体
    void initAction(); // 初始化菜单栏的Acion
    void createTab(QString fileName);

    void on_open_recent_file(); // 打开近期文件
    void saveHistory(QString path); // 保存历史记录
    QList<QString> getHistory(); // 获取历史记录
};
#endif // MAINWINDOW_H
