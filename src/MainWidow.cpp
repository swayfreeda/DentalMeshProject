#include"include/MainWindow.h"

#include<qfiledialog.h>
#include<QListWidget>
#include<QTextStream>
#include<QGridLayout>
#include<QProgressDialog>
#include <QTime>

#include "ToothSegmentation.h"

using namespace std;

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
    connect(actionToothSegmentationFindCuttingPoints, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationFindCuttingPoints()));
    connect(actionToothSegmentationRefineToothBoundary, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationRefineToothBoundary()));

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
        OpenMesh::IO::Options options;
        options += OpenMesh::IO::Options::VertexColor;
        options += OpenMesh::IO::Options::ColorFloat; //TODO ColorFloat只支持*.off和*.ply格式的模型，但是经测试还是读不到颜色信息，不知为何
        if(!OpenMesh::IO::read_mesh(mesh, filePath.toStdString().c_str(), options))
        {
            QMessageBox::information(this, "Error","Error to load " + filePath);
            return;
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

    mToothSegmentation = new ToothSegmentation(this, gv->getMesh(0));
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
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->identifyPotentialToothBoundary(true);
    cout << "ToothSegmentationIdentifyPotentialToothBoundary 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, "Info", "Identify potential tooth boundary done!");

    gv->updateGL();
    gv->viewAll();
}

void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingiva()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(true);
    cout << "ToothSegmentationAutomaticCuttingOfGingiva 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, "Info", "Automatic cutting of gingiva done!");

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationBoundarySkeletonExtraction()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->boundarySkeletonExtraction(true);
    cout << "ToothSegmentationBoundarySkeletonExtraction 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, "Info", "Boundary skeleton extraction done!");

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationFindCuttingPoints()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->findCuttingPoints(true);
    cout << "ToothSegmentationFindCuttingPoints 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, "Info", "Find cutting points done!");

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationRefineToothBoundary()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->refineToothBoundary(true);
    cout << "ToothSegmentationRefineToothBoundary 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, "Info", "Refine tooth boundary done!");

    gv->updateGL();
}
