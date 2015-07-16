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

    connect(actionToothSegmentationIdentifyPotentialToothBoundary, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationIdentifyPotentialToothBoundary()));
    connect(actionToothSegmentationAutomaticCuttingOfGingiva, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingiva()));
    connect(actionToothSegmentationBoundarySkeletonExtraction, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationBoundarySkeletonExtraction()));

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

        // If the file did not provide vertex normals, then calculate them
        if (mesh.has_face_normals() && mesh.has_vertex_normals())
        {
            mesh.update_normals(); // let the mesh update the normals
        }

        //计算顶点数、面片数、边数
        mesh.computeEntityNumbers();

        QMessageBox::information(this, "Info", QString("Vertices: %1 \n Faces: %2 \n Edges: %3").arg(mesh.mVertexNum).arg(mesh.mFaceNum).arg(mesh.mEdgeNum));
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
    statusBar()->showMessage("All mesh closed!");
}

void SW::MainWindow::doActionLaplacianDeformation()
{
    /*TODO*/
}

void SW::MainWindow::doActionUnion()
{
    /*TODO*/
}

void SW::MainWindow::doActionToothSegmentationIdentifyPotentialToothBoundary()
{
    if(gv->getMeshNum() <= 0)
    {
        return;
    }
    mToothSegmentation.setToothMesh(gv->getMesh(0));
    mToothSegmentation.identifyPotentialToothBoundary(this);
    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation.getToothMesh());

    QMessageBox::information(this, "Info", "Identify potential tooth boundary done!");

    gv->updateGL();
    gv->viewAll();

    //测试，将mesh保存到文件
//    OpenMesh::IO::Options options;
//    options += OpenMesh::IO::Options::VertexColor;
//    OpenMesh::IO::write_mesh(mToothSegmentation.getToothMesh(), "data/output.obj", options);
}

void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingiva()
{
    mToothSegmentation.automaticCuttingOfGingiva(this);
    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation.getToothMesh());

    QMessageBox::information(this, "Info", "Automatic cutting of gingiva done!");

    gv->updateGL();
    gv->viewAll();

    gv->addMesh(mToothSegmentation.getExtraMesh());

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationBoundarySkeletonExtraction()
{
    mToothSegmentation.boundarySkeletonExtraction(this);
    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation.getToothMesh());

    QMessageBox::information(this, "Info", "Boundary skeleton extraction done!");

    gv->updateGL();
    //gv->viewAll();
}
