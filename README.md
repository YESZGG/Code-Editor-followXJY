# Code-Editor-followXJY
## 基于QT的代码编辑器
* 环境：Qt 5.12 qmake
  
模仿的代码编辑器，有函数，关键字等高亮显示，行号显示、滚动等，自定义设置字体样式、大小等，新建，打开历史文件，打开已存在文件，保存文件，另存为等功能

注意：
其中文件夹中的
mytextedit.h、mytextedit.cpp、mytextedit.ui都是没有用到的，是通过QT中design 设计的ui
mytexteditbycode.h、mytextedit.cpp也是没有用到的，是通过代码自定义的组件,由于是使用QTextEdit 效率不如 PlainTextEdit,所以没有用

------

mycodeeditor.cpp、myhighlighter.cpp分别是自定义的UI组件和高亮显示


