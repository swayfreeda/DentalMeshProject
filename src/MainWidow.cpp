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
    connect(actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane()));
    connect(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp()));
    connect(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown()));
    connect(actionToothSegmentationBoundarySkeletonExtraction, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationBoundarySkeletonExtraction()));
    connect(actionToothSegmentationFindCuttingPoints, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationFindCuttingPoints()));
    connect(actionToothSegmentationRefineToothBoundary, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationRefineToothBoundary()));
    connect(actionToothSegmentationManuallyShowVertexProperties, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationManuallyShowVertexProperties(bool)));
    connect(actionToothSegmentationManuallyAddBoundaryVertex, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationManuallyAddBoundaryVertex(bool)));
    connect(actionToothSegmentationManuallyDeleteBoundaryVertex, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationManuallyDeleteBoundaryVertex(bool)));
    connect(actionToothSegmentationManuallyDeleteErrorToothRegion, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationManuallyDeleteErrorToothRegion(bool)));
    connect(actionToothSegmentationManuallyDeleteErrorContourSection, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationManuallyDeleteErrorContourSection(bool)));
    connect(actionToothSegmentationEnableManualOperation, SIGNAL(toggled(bool)), this, SLOT(doActionToothSegmentationEnableManualOperation(bool)));
    connect(actionToothSegmentationProgramControl, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationProgramControl()));

    update();

    mToothSegmentation = NULL;
    mToothSegmentationManualOperationActions.push_back(actionToothSegmentationManuallyShowVertexProperties);
    mToothSegmentationManualOperationActions.push_back(actionToothSegmentationManuallyAddBoundaryVertex);
    mToothSegmentationManualOperationActions.push_back(actionToothSegmentationManuallyDeleteBoundaryVertex);
    mToothSegmentationManualOperationActions.push_back(actionToothSegmentationManuallyDeleteErrorToothRegion);
    mToothSegmentationManualOperationActions.push_back(actionToothSegmentationManuallyDeleteErrorContourSection);
}
///////////////////////////////////////////////////////////////////////////////////
SW::MainWindow::~MainWindow()
{

}


void SW::MainWindow::doActionOpen()
{
    QString fileFilters= tr("Mesh(*.obj *.off *.ply)");
    QStringList  selectedFiles = QFileDialog::getOpenFileNames(this, tr("Open File(s)"), "./data", fileFilters);
    if(selectedFiles.size() == 0)
    {
        return;
    }

    foreach(QString filePath, selectedFiles)
    {
        statusBar()->showMessage(tr("Loading Mesh from ") + filePath);

        Mesh mesh(filePath);
        OpenMesh::IO::Options options;
        options += OpenMesh::IO::Options::VertexColor;
        options += OpenMesh::IO::Options::ColorFloat; //TODO ColorFloat只支持*.off和*.ply格式的模型，但是经测试还是读不到颜色信息，不知为何
        if(!OpenMesh::IO::read_mesh(mesh, filePath.toStdString().c_str(), options))
        {
            QMessageBox::information(this, tr("Error"), tr("Error to load ") + filePath);
            return;
        }

        // If the file did not provide vertex normals, then calculate them
        if (mesh.has_face_normals() && mesh.has_vertex_normals())
        {
            mesh.update_normals(); // let the mesh update the normals
        }

        //计算顶点数、面片数、边数
        mesh.computeEntityNumbers();

        QMessageBox::information(this, tr("Info"), QString(tr("Vertices: %1\nFaces: %2\nEdges: %3")).arg(mesh.mVertexNum).arg(mesh.mFaceNum).arg(mesh.mEdgeNum));
        statusBar()->showMessage(tr("Load Successed!"));

        //计算BoundingBox
        mesh.computeBoundingBox();

        //测试，显示BoundingBox
        /*std::cout << "mesh.MeshName: " << mesh.MeshName.toStdString() << std::endl;
        std::cout << "mesh.BBox.origin: " << mesh.BBox.origin << std::endl;
        std::cout << "mesh.BBox.size: " << mesh.BBox.size << std::endl;
        std::cout << std::endl;*/

        gv->addMesh(mesh);
    }
    gv->viewAll();

    mToothSegmentation = new ToothSegmentation(this, gv->getMesh(0));
    mToothSegmentationHistory.clear();
    mToothSegmentationHistory.push_back(*mToothSegmentation);
    mToothSegmentationUsingIndexInHistory = 0;
    connect(mToothSegmentation, SIGNAL(onSaveHistory()), this, SLOT(saveToothSegmentationHistory()));
    actionToothSegmentationProgramControl->setEnabled(true);
}

void SW::MainWindow::doActionCloseAll()
{
    gv->removeAllMeshes();
    gv->updateGL();
    statusBar()->showMessage(tr("All mesh closed!"));

    actionToothSegmentationProgramControl->setEnabled(false);
    actionToothSegmentationEnableManualOperation->setChecked(false);
    actionToothSegmentationEnableManualOperation->setEnabled(false);
    actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
    actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
    actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
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
    mToothSegmentation->identifyPotentialToothBoundary(false);
    cout << "ToothSegmentationIdentifyPotentialToothBoundary 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, tr("Info"), tr("Identify potential tooth boundary done!"));

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
    mToothSegmentation->automaticCuttingOfGingiva(false, false, -0.2);
    cout << "ToothSegmentationAutomaticCuttingOfGingiva 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva done!"));

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(false, true, 0.0);
    cout << "ToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane 用时：" << time.elapsed() / 1000 << "s." << endl;
    saveToothSegmentationHistory();

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva(flip cutting plane) done!"));

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(false, false, 0.1);
    cout << "ToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp 用时：" << time.elapsed() / 1000 << "s." << endl;
    saveToothSegmentationHistory();

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva(move cutting plane up) done!"));

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(false, false, -0.1);
    cout << "ToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown 用时：" << time.elapsed() / 1000 << "s." << endl;
    saveToothSegmentationHistory();

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva(move cutting plane down) done!"));

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
    mToothSegmentation->boundarySkeletonExtraction(false);
    cout << "ToothSegmentationBoundarySkeletonExtraction 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, tr("Info"), tr("Boundary skeleton extraction done!"));

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
    mToothSegmentation->findCuttingPoints(false);
    cout << "ToothSegmentationFindCuttingPoints 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, tr("Info"), tr("Find cutting points done!"));

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
    mToothSegmentation->refineToothBoundary(false);
    cout << "ToothSegmentationRefineToothBoundary 用时：" << time.elapsed() / 1000 << "s." << endl;

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());

    QMessageBox::information(this, tr("Info"), tr("Refine tooth boundary done!"));

    gv->updateGL();
}

void SW::MainWindow::doActionToothSegmentationManuallyShowVertexProperties(bool checked)
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    if(checked)
    {
        setOtherManualOperationActionUnChecked(actionToothSegmentationManuallyShowVertexProperties);
        connect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventShowVertexAttributes(QMouseEvent*)));
    }
    else
    {
        //disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), 0, 0); //删除与onMousePressed连接的所有槽
        disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventShowVertexAttributes(QMouseEvent*)));
    }
}

void SW::MainWindow::doActionToothSegmentationManuallyAddBoundaryVertex(bool checked)
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    if(checked)
    {
        setOtherManualOperationActionUnChecked(actionToothSegmentationManuallyAddBoundaryVertex);
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_ADD_BOUNDARY_VERTEX);
        gv->setCallSuperMouseMoveEvent(false);
        connect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        connect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventStartRecordMouseTrack(QMouseEvent*)));
        connect(gv, SIGNAL(onMouseMoved(QMouseEvent*)), mToothSegmentation, SLOT(mouseMoveEventRecordMouseTrack(QMouseEvent*)));
        connect(gv, SIGNAL(onMouseReleased(QMouseEvent*)), mToothSegmentation, SLOT(mouseReleaseEventAddSelectedBoundaryVertex(QMouseEvent*)));
    }
    else
    {
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_DEFAULT);
        gv->setCallSuperMouseMoveEvent(true);
        disconnect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventStartRecordMouseTrack(QMouseEvent*)));
        disconnect(gv, SIGNAL(onMouseMoved(QMouseEvent*)), mToothSegmentation, SLOT(mouseMoveEventRecordMouseTrack(QMouseEvent*)));
        disconnect(gv, SIGNAL(onMouseReleased(QMouseEvent*)), mToothSegmentation, SLOT(mouseReleaseEventAddSelectedBoundaryVertex(QMouseEvent*)));
    }
}

void SW::MainWindow::doActionToothSegmentationManuallyDeleteBoundaryVertex(bool checked)
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    if(checked)
    {
        setOtherManualOperationActionUnChecked(actionToothSegmentationManuallyDeleteBoundaryVertex);
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_DELETE_BOUNDARY_VERTEX);
        gv->setCallSuperMouseMoveEvent(false);
        connect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        connect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventStartRecordMouseTrack(QMouseEvent*)));
        connect(gv, SIGNAL(onMouseMoved(QMouseEvent*)), mToothSegmentation, SLOT(mouseMoveEventRecordMouseTrack(QMouseEvent*)));
        connect(gv, SIGNAL(onMouseReleased(QMouseEvent*)), mToothSegmentation, SLOT(mouseReleaseEventDeleteSelectedBoundaryVertex(QMouseEvent*)));
    }
    else
    {
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_DEFAULT);
        gv->setCallSuperMouseMoveEvent(true);
        disconnect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventStartRecordMouseTrack(QMouseEvent*)));
        disconnect(gv, SIGNAL(onMouseMoved(QMouseEvent*)), mToothSegmentation, SLOT(mouseMoveEventRecordMouseTrack(QMouseEvent*)));
        disconnect(gv, SIGNAL(onMouseReleased(QMouseEvent*)), mToothSegmentation, SLOT(mouseReleaseEventDeleteSelectedBoundaryVertex(QMouseEvent*)));
    }
}

void SW::MainWindow::doActionToothSegmentationManuallyDeleteErrorToothRegion(bool checked)
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    if(checked)
    {
        setOtherManualOperationActionUnChecked(actionToothSegmentationManuallyDeleteErrorToothRegion);
        connect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventDeleteErrorToothRegion(QMouseEvent*)));
    }
    else
    {
        disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventDeleteErrorToothRegion(QMouseEvent*)));
    }
}

void SW::MainWindow::doActionToothSegmentationManuallyDeleteErrorContourSection(bool checked)
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    if(checked)
    {
        setOtherManualOperationActionUnChecked(actionToothSegmentationManuallyDeleteErrorContourSection);
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_DELETE_ERROR_CONTOUR_SECTION);
        connect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        connect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventDeleteErrorContourSection(QMouseEvent*)));
    }
    else
    {
        mToothSegmentation->setCustomCursor(mToothSegmentation->CURSOR_DEFAULT);
        disconnect(gv, SIGNAL(onKeyPressed(QKeyEvent*)), mToothSegmentation, SLOT(keyPressEventChangeMouseRadius(QKeyEvent*)));
        disconnect(gv, SIGNAL(onMousePressed(QMouseEvent*)), mToothSegmentation, SLOT(mousePressEventDeleteErrorContourSection(QMouseEvent*)));
    }
}

void SW::MainWindow::doActionToothSegmentationEnableManualOperation(bool enable)
{
    if(!enable)
    {
        setAllManualOperationActionUnChecked();

        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
    }
    else
    {
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(true);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(true);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(true);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(true);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(true);
    }
}

void SW::MainWindow::setAllManualOperationActionUnChecked()
{
    for(int i = 0; i < mToothSegmentationManualOperationActions.size(); i++)
    {
        mToothSegmentationManualOperationActions.at(i)->setChecked(false);
    }
}

void SW::MainWindow::setOtherManualOperationActionUnChecked(QAction *checkedAction)
{
    for(int i = 0; i < mToothSegmentationManualOperationActions.size(); i++)
    {
        if(mToothSegmentationManualOperationActions.at(i)->text() != checkedAction->text()) //TODO 此处用两个QAction的text是否想同来判断是否同一个Action，欠妥
        {
            mToothSegmentationManualOperationActions.at(i)->setChecked(false);
        }
    }
}

void SW::MainWindow::doActionToothSegmentationProgramControl()
{
    if(mToothSegmentation == NULL)
    {
        return;
    }

    switch(mToothSegmentation->getProgramSchedule())
    {
    case ToothSegmentation::SCHEDULE_START:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));

        mToothSegmentation->identifyPotentialToothBoundary(false);
        mToothSegmentation->automaticCuttingOfGingiva(false, false, -0.2);
        saveToothSegmentationHistory();
        gv->removeAllMeshes();
        gv->addMesh(mToothSegmentation->getToothMesh());
        if(mToothSegmentation->shouldShowExtraMesh())
        {
            gv->addMesh(mToothSegmentation->getExtraMesh());
        }
        QMessageBox::information(this, tr("Info"), tr("Identify potential tooth boundary done!\nAutomatic cutting of gingiva done!\nPlease manually refine potential tooth boundary!"));
        gv->updateGL();

        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(true);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(true);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(true);
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        break;
    case ToothSegmentation::SCHEDULE_IdentifyPotentialToothBoundary_FINISHED:

        break;
    case ToothSegmentation::SCHEDULE_AutomaticCuttingOfGingiva_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);

        mToothSegmentation->boundarySkeletonExtraction(false);
        mToothSegmentation->findCuttingPoints(false);
        saveToothSegmentationHistory();
        gv->removeAllMeshes();
        gv->addMesh(mToothSegmentation->getToothMesh());
        if(mToothSegmentation->shouldShowExtraMesh())
        {
            gv->addMesh(mToothSegmentation->getExtraMesh());
        }
        QMessageBox::information(this, tr("Info"), tr("Boundary skeleton extraction done!\nFind cutting points done!\nPlease manually remove error contour sections!"));
        gv->updateGL();

        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        break;
    case ToothSegmentation::SCHEDULE_BoundarySkeletonExtraction_FINISHED:

        break;
    case ToothSegmentation::SCHEDULE_FindCuttingPoints_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));

        mToothSegmentation->refineToothBoundary(false);
        saveToothSegmentationHistory();
        gv->removeAllMeshes();
        gv->addMesh(mToothSegmentation->getToothMesh());
        if(mToothSegmentation->shouldShowExtraMesh())
        {
            gv->addMesh(mToothSegmentation->getExtraMesh());
        }
        QMessageBox::information(this, tr("Info"), tr("Refine tooth boundary done!\nThe whole program completed!"));
        gv->updateGL();

        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        break;
    case ToothSegmentation::SCHEDULE_RefineToothBoundary_FINISHED:
        QMessageBox::information(this, tr("Info"), tr("The whole program completed!"));
        break;
    }
}

void SW::MainWindow::keyPressEvent(QKeyEvent *e)
{
    if((e->modifiers() & Qt::ControlModifier)) //"Ctrl"
    {
        switch(e->key())
        {
        case Qt::Key_Z: //"Z"（撤销）
            if(mToothSegmentationUsingIndexInHistory > 0)
            {
                mToothSegmentationUsingIndexInHistory--;
                //*mToothSegmentation = *(mToothSegmentationHistory.data() + mToothSegmentationUsingIndexInHistory);
                //memcpy(mToothSegmentation, mToothSegmentationHistory.data() + mToothSegmentationUsingIndexInHistory, sizeof(ToothSegmentation));
                mToothSegmentation->copyFrom(mToothSegmentationHistory.at(mToothSegmentationUsingIndexInHistory));

                //更新显示
                gv->removeAllMeshes();
                gv->addMesh(mToothSegmentation->getToothMesh());
                if(mToothSegmentation->shouldShowExtraMesh())
                {
                    gv->addMesh(mToothSegmentation->getExtraMesh());
                }
                gv->updateGL();
            }
            break;
        case Qt::Key_R: //"R"（重做）
            if(mToothSegmentationUsingIndexInHistory < (mToothSegmentationHistory.size() - 1))
            {
                mToothSegmentationUsingIndexInHistory++;
                mToothSegmentation->copyFrom(mToothSegmentationHistory.at(mToothSegmentationUsingIndexInHistory));

                //更新显示
                gv->removeAllMeshes();
                gv->addMesh(mToothSegmentation->getToothMesh());
                if(mToothSegmentation->shouldShowExtraMesh())
                {
                    gv->addMesh(mToothSegmentation->getExtraMesh());
                }
                gv->updateGL();
            }
            break;
        }
    }
}

void SW::MainWindow::saveToothSegmentationHistory()
{
    if(mToothSegmentationUsingIndexInHistory != (mToothSegmentationHistory.size() - 1))
    {
        mToothSegmentationHistory.remove(mToothSegmentationUsingIndexInHistory + 1, mToothSegmentationHistory.size() - mToothSegmentationUsingIndexInHistory - 1);
    }
    mToothSegmentationHistory.push_back(*mToothSegmentation);
    mToothSegmentationUsingIndexInHistory++;
}
