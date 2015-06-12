#include"MainWindow.h"
#include"Mesh.h"
#include"MeshIO.h"


#include<qfiledialog.h>
#include<QListWidget>
#include<QTextStream>
#include<QGridLayout>
#include<QProgressDialog>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::MainWindow()
{
    setupUi(this);

    connect(actionOpen, SIGNAL(triggered()), this, SLOT(doActionOpen()));
    connect(actionLaplacianDeformation, SIGNAL(triggered()), this, SLOT(doActionLaplacianDeformation()));
    connect(actionUnion, SIGNAL(triggered()), this, SLOT(doActionUion()));
    connect(actionDisplayVertices, SIGNAL(triggered()), gv, SLOT(toggleDisplayVertices()));
    connect(actionDisplayWireFrame, SIGNAL(triggered()), gv, SLOT(toggleDisplayWireFrame()));
    connect(actionDisplayFlat, SIGNAL(triggered()), gv, SLOT(toggleDisplayFlat()));
}
///////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::~MainWindow()
{

}



void SW::MainWindow::doActionOpen(){

    QString fileFilters = "Mesh(*.obj)";

    QStringList selectedFiles =   QFileDialog::getOpenFileNames(this,
                                                                "Open File(s)",
                                                                "/home/sway/DentalMeshProject/data",
                                                                fileFilters);
    if(selectedFiles.size() ==0) return;

    gv->meshes.resize(selectedFiles.size());
    int id =0;

    foreach(QString filePath, selectedFiles){

        statusBar()->showMessage("Load Mesh from " + filePath);
        if(gv->meshes[id].read(filePath.toStdString())){
            QMessageBox::warning(this, "Error", "Error to load from "+ filePath);
            return;
        }

        int vNum = gv->meshes[id].vertices.size();
        int fNum = gv->meshes[id].faces.size();
        int eNum = gv->meshes[id].edges.size();

        QMessageBox::information(this, "Mesh Info", QString("Vertices: %1 \n Facets: %2 \n Edges: %3").arg(vNum).arg(fNum).arg(eNum));
        id++;
    }


}

void SW::MainWindow::doActionLaplacianDeformation(){

    /*TODO*/
}

void SW::MainWindow::doActionUion(){
    /*TODO*/
}
