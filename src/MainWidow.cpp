#include"MainWindow.h"

#include<qfiledialog.h>
#include<QListWidget>
#include<QTextStream>
#include<QGridLayout>
#include<QProgressDialog>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::MainWindow()
{
    setupUi(this);
  //  gv = new GLViewer(fFrame,"GLViewer");
  //  horizonlayout1->addWidget(gv, 1);

   // gv->setCursor(Qt::ArrowCursor);
   // updateWidgets();

   // connect(gv, SIGNAL(sigUpdateGL()), this, SLOT(updateWidgets()));//?????????????????

}
///////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::~MainWindow()
{

}
