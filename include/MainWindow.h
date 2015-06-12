#ifndef SW_MAINWINDOW_H
#define SW_MAINWINDOW_H

#include"GLViewer.h"
#include"ui_mainwindow.h"

#include <QMainWindow>
#include"qlayout.h"
//#include"qpopmenu.h"
#include"qcursor.h"
#include"qevent.h"

#include<QWidget>
#include"qnamespace.h"
#include<QFrame>
#include<QSlider>
#include<QCheckBox>
#include<QAction>
#include<QActionGroup>
#include<QToolBar>
#include<QMenu>
#include<QMenuBar>
#include<QStatusBar>
#include<QMessageBox>
#include<QListWidget>


#include<opencv2/opencv.hpp>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem; //
//class QActionGroup;
//class QToolBar;
//class QMenu;
//class QSlider;
//class QFrame;
//class QCheckBox;
class QPushButton;
class QTimer;
class QKeyEvent;
class GLViewer;

namespace SW
{
    class MainWindow: public QMainWindow, public Ui::MainWindow
    {
        Q_OBJECT

    public:

         MainWindow();
        ~MainWindow(); // to be implemented

    protected:

        virtual void keyPressEvent(QKeyEvent *){}

    public slots:


    public:
         //////////////////////定义窗口部件/////////////////////////////////////
         GLViewer *gv;

  signals:

    };

}



#endif // SW_MAINWINDOW_H


