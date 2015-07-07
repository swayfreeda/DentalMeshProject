#include"include/MainWindow.h"

#include<qfiledialog.h>
#include<QListWidget>
#include<QTextStream>
#include<QGridLayout>
#include<QProgressDialog>

#include "ToothSegmentation.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::MainWindow()
{
    setupUi(this);

    connect(actionOpen, SIGNAL(triggered()), this, SLOT(doActionOpen()) );
    connect(actionCloseAll, SIGNAL(triggered()), this, SLOT(doActionCloseAll()) );
    connect(actionLaplacianDeformation, SIGNAL(triggered()), this, SLOT(doActionLaplacianDeformation()) );
    connect(actionUnion, SIGNAL(triggered()), this, SLOT(doActionUnion()));
    connect(actionDisplayVertices, SIGNAL(triggered()), gv, SLOT(toggleDisplayVertices()));
    connect(actionDisplayWireFrame, SIGNAL(triggered()), gv, SLOT(toggleDisplayWireFrame()));
    connect(actionDisplayFlatLine, SIGNAL(triggered()), gv, SLOT(toggleDisplayFlatLine()));

    connect(actionSegmentation, SIGNAL(triggered()), this, SLOT(doActionSegmentation()));

    update();
}
///////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::~MainWindow()
{

}


void SW::MainWindow::doActionOpen()
{
    QString fileFilters= "Mesh(*.obj *.off *.ply)";
    QStringList  selectedFiles = QFileDialog::getOpenFileNames(this, tr("Open File(s)"), "./data", fileFilters);
    if(selectedFiles.size() == 0)
    {
        return;
    }

    foreach(QString filePath, selectedFiles)
    {
        statusBar()->showMessage("Loading Mesh from " + filePath);

        Mesh mesh(filePath);
        if(!OpenMesh::IO::read_mesh(mesh, filePath.toStdString().c_str()))
        {
            QMessageBox::information(this, "Error","Error to load " + filePath);
        }
        int vNum =0;
        int fNum = 0;
        int eNum = 0;
        for(Mesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
        {
            vNum++;
        }
        for(Mesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
        {
            fNum++;
        }
        for(Mesh::EdgeIter eit = mesh.edges_begin(); eit != mesh.edges_end(); eit++)
        {
            eNum++;
        }

        // If the file did not provide vertex normals, then calculate them
        if (mesh.has_face_normals() && mesh.has_vertex_normals())
        {
            mesh.update_normals(); // let the mesh update the normals
        }
        QMessageBox::information(this, "Info", QString("Vertices: %1 \n Faces: %2 \n Edges: %2").arg(vNum).arg(fNum).arg(eNum));
        statusBar()->showMessage("Load Successed!");

        //计算BoundingBox
        mesh.computeBoundingBox();

        //测试，显示BoundingBox
        /*std::cout << "mesh.MeshName: " << mesh.MeshName.toStdString() << std::endl;
        std::cout << "mesh.BBox.origin: " << mesh.BBox.origin << std::endl;
        std::cout << "mesh.BBox.size: " << mesh.BBox.size << std::endl;
        std::cout << std::endl;*/

        gv->addMesh(mesh);
        gv->viewAll();
    }

}

void SW::MainWindow::doActionCloseAll()
{
    gv->removeAllMeshes();
    gv->updateGL();
}

void SW::MainWindow::doActionLaplacianDeformation()
{
    /*TODO*/
}

void SW::MainWindow::doActionUnion()
{
    /*TODO*/
}

void SW::MainWindow::doActionSegmentation()
{
    //QMessageBox::information(this,"Segmentation","Segmentation start.");
    if(gv->getMeshNum() <= 0)
    {
        return;
    }
    ToothSegmentation toothSegmentation(gv->getMesh(0));
    toothSegmentation.identifyingPotentialToothBoundary();
    gv->removeAllMeshes();
    gv->addMesh(toothSegmentation.getToothMesh());
    gv->updateGL();
    QMessageBox::information(this,"Segmentation","Added mesh from toothSegmentation.");
}
