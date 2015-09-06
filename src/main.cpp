#include"include/MainWindow.h"
#include <qapplication.h>
#include <QTranslator>

using namespace SW;
int main(int argc, char**argv)
{
    QApplication app(argc,argv);

    QTranslator *translator = new QTranslator(&app);
    if(translator->load(QString("./language/zh_CN")))
    {
         app.installTranslator(translator);
    }

    MainWindow mainwindow;
    mainwindow.showMaximized();
    mainwindow.show();
    return app.exec();
}

