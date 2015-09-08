#include"include/MainWindow.h"
#include <qapplication.h>

#ifdef LANGUAGE_CH_ZN
#include <QTranslator>
#endif

using namespace SW;
int main(int argc, char**argv)
{
    QApplication app(argc,argv);

#ifdef LANGUAGE_CH_ZN
    QTranslator *translator = new QTranslator(&app);
    if(translator->load(QString("./language/zh_CN")))
    {
        app.installTranslator(translator);
    }
#endif


    MainWindow mainwindow;
    mainwindow.setWindowState(mainwindow.windowState()^Qt::WindowMaximized);

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    app.setMainWidget(&mainwindow);
#else
    app.setActiveWindow(&mainwindow);
#endif
    mainwindow.showMaximized();
    mainwindow.show();

    return app.exec();
}

