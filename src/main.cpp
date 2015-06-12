#include"include/MainWindow.h"
#include <qapplication.h>


using namespace SW;
int main(int argc, char**argv)
{
    QApplication app(argc,argv);

    MainWindow mainwindow;

    mainwindow.resize(400,600);
    mainwindow.setWindowState(mainwindow.windowState()^Qt::WindowMaximized);

    app.setMainWidget(&mainwindow);


    mainwindow.show();

    return app.exec();



}

