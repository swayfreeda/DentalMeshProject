#include"include/MainWindow.h"
#include <qapplication.h>


using namespace SW;
int main(int argc, char**argv)
{
    QApplication app(argc,argv);

    MainWindow mainwindow;
    mainwindow.setWindowState(mainwindow.windowState()^Qt::WindowMaximized);

    app.setMainWidget(&mainwindow);
    mainwindow.showMaximized();
    mainwindow.show();


    return app.exec();



}

