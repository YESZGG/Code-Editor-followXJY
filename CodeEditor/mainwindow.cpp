#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->tabWidget); // 让 文本编辑输入框 为窗体中心
//    QSettings 是一个用于读取和写入应用程序设置的类
    mSettings = new QSettings("settings.ini",QSettings::IniFormat); //创建了一个  QSettings 对象以读取和写入名为 "settings.ini" 的 INI 格式设置文件

    // 初始化菜单
    initMenu();
    // 初始化字体
    initFont();
    // 初始化Action
    initAction();

#if !QT_CONFIG(printer)
    ui->print->setEnabled(false);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化recent历史记录菜单
void MainWindow::initMenu()
{
    QMenu * recent = this->findChild<QMenu *>("recent"); // 获取名为 "recent" 的菜单对象

    // 清空历史记录菜单项
    QSet<QObject *> chLists = recent->children().toSet();
    foreach (QObject * ch, chLists) {
        QAction *action = (QAction *)ch; // 将子对象转换为 QAction
        // 清空子菜单中的QAction
        recent->removeAction(action); // 从菜单中移除该操作项
    }

    // 获取历史记录列表
    QList<QString> lists = getHistory();

    // 遍历历史记录列表并添加到菜单中
    for(int i=lists.size()-1 ;i>=0;i--){
        //生成子菜单Action
        recent->addAction(lists[i],this,&MainWindow::on_open_recent_file);
    }

    if(lists.size() > 0) {
        recent->addAction("清除历史记录",this,&MainWindow::on_clear_history_triggered,QKeySequence("Ctrl+Alt+Shif+C"));
    }
}

// 设置字体
void MainWindow::initFont()
{
    mFontFamily = mSettings->value("font_family","Consolas").toString();
    mFontSize = mSettings->value("font_size",12).toInt();
}

void MainWindow::initAction()
{
    bool valid = ui->tabWidget->count() > 0;
    ui->save_file->setEnabled(valid);
    ui->save_as->setEnabled(valid);
    ui->copy->setEnabled(valid);
    ui->paste->setEnabled(valid);
    ui->cut->setEnabled(valid);
    ui->undo->setEnabled(valid);
    ui->redo->setEnabled(valid);
}


// 获取历史记录
QList<QString> MainWindow::getHistory()
{
    int size = mSettings->beginReadArray("history"); // 获取设置数组 "history" 的大小

    QList<QString> lists;
    for (int i = 0; i < size; i++)
    {
        mSettings->setArrayIndex(i); // 设置数组索引为当前位置，以便读取该位置的元素
        QString path = mSettings->value("path").toString();
        lists.append(path); // 将读取的路径值添加到列表中
    }
    mSettings->endArray(); // 结束对设置数组的读取操作

    return lists; // 返回历史记录列表
}

/* 保存并打开历史记录
   功能：将新的历史记录路径添加到设置文件中。 它会获取当前保存的历史记录列表，并将传入的新路径添加到列表末尾。 然后，它去除可能的重复项，
        并将更新后的历史记录列表写入设置文件。
*/
void MainWindow::saveHistory(QString path)
{
    QList<QString> lists = getHistory(); // 获取历史记录
    foreach(QString str,lists){
        if(str==path){
            lists.removeOne(str);
        }
    }
    lists.append(path); // 将新的历史记录路径添加到列表末尾
    /* toSet() 函数作用
     * 返回一个带有此QList中包含的数据的QSet对象。由于QSet不允许重复，因此生成的QSet可能比原始列表小
     * toList() 函数作用
     * 返回一个包含集合中元素的新QList。QList中元素的顺序未定义。
    */
//    lists.toSet().toList(); // 去重  (结果没有去重) 错误

    mSettings->beginWriteArray("history"); // 获取当前设置数组的大小，以准备写入新的历史记录

    // 遍历历史记录列表并写入设置文件
    for (int i = 0; i < lists.size(); i++)
    {
        mSettings->setArrayIndex(i); // 设置数组索引为当前位置，以便写入对应位置的元素

        // 将路径值写入设置文件
        mSettings->setValue("path", lists[i]);
    }

    mSettings->endArray(); // 结束设置数组的读取和写入操作
}
//[history]
//path=c://123.txt


// 新打开文件
void MainWindow::on_new_file_triggered()
{
    ui->tabWidget->addTab(new MyCodeEditor(this, QFont(mFontFamily, mFontSize)),"NewTab");
    initAction();
}

// 打开历史文件
void MainWindow::on_open_recent_file()
{
    QAction * action = (QAction *)sender(); // 获取触发信号的 QAction 对象
//    QString fileName = action->text(); // 获取菜单项的文本，即历史文件路径
    createTab(action->text());
}

// 打开文件
void MainWindow::on_open_file_triggered()
{
    createTab(QFileDialog::getOpenFileName(this,"打开文件夹"));
}

// 创建tab 标签页
void MainWindow::createTab(QString fileName)
{
    QFile file(fileName); //QFile(QString &name)构造一个新的文件对象来表示具有给定名称的文件。
    /*
     * 打开文件，打开模式有 QIODevice::ReadOnly, QIODevice::WriteOnly, or QIODevice::ReadWrite
     * QIODevice::Text (会将换行符  \n 自动转换为平台特定的换行符)and QIODevice::Unbuffered
    */
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"警告","无法打开此文件"+file.errorString()); // 返回上一次发生的设备错误的可读说明。
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll(); // 读取所有数据

    // 创建对象
    MyCodeEditor * codeEditor = new MyCodeEditor(this, QFont(mFontFamily, mFontSize));
    codeEditor->setPlainText(text);
    // 设置文件名
    codeEditor->setFileName(fileName);
    // 添加到 tab 中
    ui->tabWidget->addTab(codeEditor,fileName);
    initAction();
    // 设置当前索引
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() -1);

    file.close(); // 关闭文件
    saveHistory(fileName);
    initMenu();
}


// 保存文件
void MainWindow::on_save_file_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor)
    {
        if (codeEditor->saveFile()){
            saveSuccessAction(codeEditor);
        }
    }
}

// 另存为
void MainWindow::on_save_as_triggered()
{
    // 把保存交给 CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor)
    {
        if (codeEditor->saveAsFile()){
            saveSuccessAction(codeEditor);
        }
    }
}

// 粘贴
void MainWindow::on_paste_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
        codeEditor->paste();
    }
}

// 剪切
void MainWindow::on_cut_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
        codeEditor->cut();
    }
}

// 复制
void MainWindow::on_copy_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
        codeEditor->copy();
    }
}

// 字体
void MainWindow::on_font_triggered()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, QFont(mFontFamily,mFontSize), this);
    if (fontSelected)
    {
        MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
        if (codeEditor){
            codeEditor->setAllFont(font);
        }
        mSettings->setValue("font_family",font.family());
        mSettings->setValue("font_size",font.pointSize());
    }
}

// 关于
void MainWindow::on_about_triggered()
{
    QMessageBox::about(this,"这是我的Code Editor","这是我的Code Editor,欢迎使用!");
}

// 撤销
void MainWindow::on_undo_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
        codeEditor->undo();
    }
}

// 取消撤销
void MainWindow::on_redo_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
        codeEditor->redo();
    }
}

// 退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}

// 打印
void MainWindow::on_print_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (codeEditor){
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
        QPrinter printDev;
#if QT_CONFIG(printdialog)
        QPrintDialog dialog(&printDev, this);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
#endif
        codeEditor->print(&printDev);
        //ui->textEdit->print(&printDev);
#else // 如果不满足条件，则给出错误提示或其他处理方式
        qDebug() << "Printing functionality is not available.";
#endif
    }
}


// 清除历史记录
void MainWindow::on_clear_history_triggered()
{
    mSettings->remove("history");
    initMenu();
}

// 关闭标签
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    MyCodeEditor *codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    // 如果文件是空的，则直接关闭
    if (codeEditor->toPlainText().isEmpty())
    {
        delete ui->tabWidget->currentWidget();
        ui->tabWidget->removeTab(index);
        initAction();
        return;
    }

    if (!codeEditor->checkSaved()){
        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","您还没有保存文档！是否保存?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (btn == QMessageBox::Yes){
            if (codeEditor->saveFile()){
                saveSuccessAction(codeEditor);
                return;
            }
        }else if (btn == QMessageBox::Cancel){
            return;
        }
    }
    ui->tabWidget->removeTab(index);
    delete ui->tabWidget->currentWidget();
    initAction();
}

void MainWindow::saveSuccessAction(MyCodeEditor *codeEditor)
{
    QString fileName = codeEditor->getFileName();
    // 保存历史
    saveHistory(fileName);
    // 设置tab标题
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), fileName);
    // 初始化菜单
    initMenu();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    MyCodeEditor * codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();
    if (ui->tabWidget->count() > 0){
        if (codeEditor->toPlainText().isEmpty()){
            event->accept();
            return;
        }

        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","有未保存的文件，确定要关闭吗?",QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            event->accept();
        }
        else if (btn == QMessageBox::No){
            event->ignore();
        }
    }
}
