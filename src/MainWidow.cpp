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

    // load mesh
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(doActionOpen()) );

    // clear work space
    connect(actionCloseAll, SIGNAL(triggered()), this, SLOT(doActionCloseAll()) );

    // laplacian transformation (checked)
    connect(actionLaplacianDeformation, SIGNAL(toggled(bool)), this, SLOT(doActionLaplacianDeformation(bool)) );
    connect(actionSelectPoints, SIGNAL(toggled(bool)), this, SLOT(doActionSelectPoints(bool)));
    connect(actionDoDeformation, SIGNAL(toggled(bool)), this, SLOT(doActionDo_Deformation(bool)));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(doAcationReset()));



    // toolth segmentation (checked)
    connect(actionToothSegmentations, SIGNAL(toggled(bool)), SLOT(doActionToolthSegmentation(bool)));

    // bool oepration (checked)
    connect(actionBooleanOperation, SIGNAL(toggled(bool)), SLOT(doActionBooleanOperation(bool)));

    // boolean operatoin
    connect(actionUnion_2, SIGNAL(triggered()), this, SLOT(doActionUnion()));
    connect(actionIntersection, SIGNAL(triggered()), this, SLOT(doActionIntersection()));
    connect(actionDifference, SIGNAL(triggered()), this, SLOT(doActionDifference()));

    // displaye mode
    connect(actionDisplayVertices, SIGNAL(triggered()), gv, SLOT(toggleDisplayVertices()));
    connect(actionDisplayWireFrame, SIGNAL(triggered()), gv, SLOT(toggleDisplayWireFrame()));
    connect(actionDisplayFlatLine, SIGNAL(triggered()), gv, SLOT(toggleDisplayFlatLine()));

    // raise cutting plane up
    connect(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp()));
    // bring cutting plane down
    connect(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown()));
    // flipping cutting plane
    connect(actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane()));

#ifdef  EARLER_VERSION
    connect(actionToothSegmentationIdentifyPotentialToothBoundary, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationIdentifyPotentialToothBoundary()));
    connect(actionToothSegmentationAutomaticCuttingOfGingiva, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationAutomaticCuttingOfGingiva()));

    connect(actionToothSegmentationBoundarySkeletonExtraction, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationBoundarySkeletonExtraction()));
    connect(actionToothSegmentationFindCuttingPoints, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationFindCuttingPoints()));
    connect(actionToothSegmentationRefineToothBoundary, SIGNAL(triggered()), this, SLOT(doActionToothSegmentationRefineToothBoundary()));
#endif
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

    mCurrentProcessMode = NONE;

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

    foreach(QString filePath, selectedFiles)  {

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
        if (mesh.has_face_normals() && mesh.has_vertex_normals()){
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


void SW::MainWindow::doActionUnion()
{
    if(gv->getMeshNum() != 2){
        QMessageBox::warning(this, tr("Warning"), tr("Two models muste be imported!"));
        return;
    }

    Mesh result= boolOperation(UNION);
    gv->removeAllMeshes();
    gv->addMesh(result);
    gv->viewAll();
    gv->updateGL();
    QMessageBox::information(this, tr("Info"), tr("Union of two meshes done!"));
}

void SW::MainWindow::doActionIntersection(){
    if(gv->getMeshNum() != 2){
        QMessageBox::warning(this, tr("Warning"), tr("Two models muste be imported!"));
        return;
    }

    Mesh result= boolOperation(INTERSECTION);

    gv->removeAllMeshes();
    gv->addMesh(result);
    gv->viewAll();
    gv->updateGL();
    QMessageBox::information(this, tr("Info"), tr("Intersection of two meshes done!"));
}

void SW::MainWindow::doActionDifference(){
    if(gv->getMeshNum() != 2){
        QMessageBox::warning(this, tr("Warning"), tr("Two models muste be imported!"));
        return;
    }


    Mesh result= boolOperation(DIFFERENCE);
    gv->removeAllMeshes();
    gv->addMesh(result);
    gv->viewAll();
    gv->updateGL();
    QMessageBox::information(this, tr("Info"), tr("Difference of two meshes done!"));
}

void SW::MainWindow::doActionLaplacianDeformation(bool checked)
{
    if(gv->getMeshNum() ==0){
        mCurrentProcessMode = NONE;
        gv->setCurrentProcessMode(NONE);
        QMessageBox::warning(this, tr("Warning"), tr("At least one model must be imported!"));
        return;
    }
    if(checked){
        mCurrentProcessMode = LAPLACIAN_TRANSFORM_MODE;
        gv->setCurrentProcessMode(LAPLACIAN_TRANSFORM_MODE);

        //enable laplacian tranformation acitions
        actionSelectPoints->setEnabled(true);
        actionDoDeformation->setEnabled(true);

        // disable boolean operation actions
        actionBooleanOperation->setChecked(false);
        actionUnion_2->setEnabled(false);
        actionDifference->setEnabled(false);
        actionIntersection->setEnabled(false);

        // disabled tooth segmentation
        actionToothSegmentations->setChecked(false);
        actionToothSegmentationProgramControl->setEnabled(false);

        gv->initLaplacianTransformation();
    }
    else{
        mCurrentProcessMode = NONE;
        gv->setCurrentProcessMode(NONE);

        //disable laplacian tranformation acitions
        actionSelectPoints->setEnabled(false);
        actionDoDeformation->setEnabled(false);
    }

    update();
}

void SW::MainWindow::doActionSelectPoints(bool checked){
    if(checked){
        actionDoDeformation->setChecked(false);
        gv->toggleMovePoints(!checked);
    }

    gv->toggleSelectingVertexMode(checked);
    update();
}

void SW::MainWindow::doActionDo_Deformation(bool checked){
    if(checked){
        actionSelectPoints->setChecked(false);
        gv->toggleSelectingVertexMode(!checked);
    }
    gv->toggleMovePoints(checked);
    update();

}

void SW::MainWindow::doAcationReset(){
    if(mCurrentProcessMode ==  LAPLACIAN_TRANSFORM_MODE){
        actionSelectPoints->setChecked(false);
        actionDoDeformation->setChecked(false);
        gv->toggleModelReset();
    }
    else if( mCurrentProcessMode ==  BOOLEAN_OPERATION_MODE){
        gv->removeAllMeshes();
        gv->addMesh(m_OrignalMeshForBooleanOpearion[0]);
        gv->addMesh(m_OrignalMeshForBooleanOpearion[1]);
    }
    else if(mCurrentProcessMode ==  SEGMENTATION_MODE){

        // disable tooth segmentation
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);

        if(mToothSegmentationHistory.size()>0){
            mToothSegmentation->copyFrom(mToothSegmentationHistory.at(0));
            //更新显示
            gv->removeAllMeshes();
            gv->addMesh(mToothSegmentation->getToothMesh());
            mToothSegmentationHistory.clear();
        }
        if(mToothSegmentation){
            delete mToothSegmentation;
            mToothSegmentation = 0;
        }
    }
    else{
        actionBooleanOperation->setChecked(false);
        actionLaplacianDeformation->setChecked(false);
        actionToothSegmentations->setChecked(false);
    }
    gv->updateGL();
    update();
}


void SW::MainWindow::doActionToolthSegmentation(bool checked){
    if(gv->getMeshNum() ==0){
        mCurrentProcessMode = NONE;
        gv->setCurrentProcessMode(NONE);
        QMessageBox::warning(this, tr("Warning"), tr("At least one model must be imported!"));
        return;
    }
    if(checked) {

        mCurrentProcessMode = SEGMENTATION_MODE;
        gv->setCurrentProcessMode(SEGMENTATION_MODE);

        // disable boolean transformation actions
        actionBooleanOperation->setChecked(false);
        actionDifference->setEnabled(false);
        actionIntersection->setEnabled(false);
        actionUnion_2->setEnabled(false);

        // disabled laplacian transformation
        actionLaplacianDeformation->setChecked(false);
        actionSelectPoints->setEnabled(false);
        actionDoDeformation->setEnabled(false);


        // enable tooth segmentation
        actionToothSegmentationProgramControl->setEnabled(true);
    }
    else{

        mCurrentProcessMode = NONE;
        gv->setCurrentProcessMode(NONE);

        // disable tooth segmentation
        actionToothSegmentationProgramControl->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        disconnect(mToothSegmentation, SIGNAL(onSaveHistory()), this, SLOT(saveToothSegmentationHistory()));
        disconnect(mToothSegmentation, SIGNAL(onProgramScheduleChanged(int)), this, SLOT(changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(int)));

        if(mToothSegmentationHistory.size()>0){
            mToothSegmentation->copyFrom(mToothSegmentationHistory.at(0));
            //更新显示
            gv->removeAllMeshes();
            gv->addMesh(mToothSegmentation->getToothMesh());
            mToothSegmentationHistory.clear();
        }
        if(mToothSegmentation){
            delete mToothSegmentation;
            mToothSegmentation = 0;
        }

    }
    update();
    gv->updateGL();
}

void SW::MainWindow::doActionBooleanOperation(bool checked){

    if(checked){
        mCurrentProcessMode = BOOLEAN_OPERATION_MODE;
        gv->setCurrentProcessMode(BOOLEAN_OPERATION_MODE);

        // enable boolean transformation actions
        actionDifference->setEnabled(true);
        actionIntersection->setEnabled(true);
        actionUnion_2->setEnabled(true);

        // disabled laplacian transformation
        actionLaplacianDeformation->setChecked(false);
        actionSelectPoints->setEnabled(false);
        actionDoDeformation->setEnabled(false);


        // diasabled tooth segmentation
        actionToothSegmentations->setChecked(false);
        actionToothSegmentationProgramControl->setEnabled(false);
    }
    else{
        mCurrentProcessMode = NONE;
        gv->setCurrentProcessMode(NONE);

        // disable boolean transformations actions
        actionDifference->setEnabled(false);
        actionIntersection->setEnabled(false);
        actionUnion_2->setEnabled(false);
        //m_OrignalMeshForBooleanOpearion
    }
    update();

}
// raise the cutting plane up
void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp()
{
    if(mToothSegmentation == NULL) {
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(false, false, 0.05);
    cout << "ToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp 用时：" << time.elapsed() / 1000 << "s." << endl;
    saveToothSegmentationHistory();

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva(move cutting plane up) done!"));

    gv->updateGL();
}


// Flipping the cutting plane
void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane() {
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


// bring the cutting plane down
void SW::MainWindow::doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown(){

    if(mToothSegmentation == NULL){
        return;
    }

    QTime time;
    time.start();
    mToothSegmentation->automaticCuttingOfGingiva(false, false, -0.05);
    cout << "ToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown 用时：" << time.elapsed() / 1000 << "s." << endl;
    saveToothSegmentationHistory();

    gv->removeAllMeshes();
    gv->addMesh(mToothSegmentation->getToothMesh());
    gv->addMesh(mToothSegmentation->getExtraMesh());

    QMessageBox::information(this, tr("Info"), tr("Automatic cutting of gingiva(move cutting plane down) done!"));

    gv->updateGL();
}

#ifdef EARLER_VERSION
// Compute Guassian curvature to indetity potential tooth boundary
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

// Compute a Cutting plane to obtain Gingva ( intercation may needed)
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



// extract boundary skeleton -- single vertex
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

// finding the  cutting points
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
//
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
#endif

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
// main tooth segmentation program
void SW::MainWindow::doActionToothSegmentationProgramControl()
{
    if(mCurrentProcessMode != SEGMENTATION_MODE)
        return;

    if(mToothSegmentation == NULL) {
        mOriginalMeshForSegmentation = gv->getMesh(0);
        mToothSegmentation = new ToothSegmentation(this, gv->getMesh(0));
        mToothSegmentationHistory.clear();
        mToothSegmentationHistory.push_back(*mToothSegmentation);
        mToothSegmentationUsingIndexInHistory = 0;
        connect(mToothSegmentation, SIGNAL(onSaveHistory()), this, SLOT(saveToothSegmentationHistory()));
        connect(mToothSegmentation, SIGNAL(onProgramScheduleChanged(int)), this, SLOT(changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(int)));
    }

    switch(mToothSegmentation->getProgramSchedule()) {

    case ToothSegmentation::SCHEDULE_START:
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
        break;
    case ToothSegmentation::SCHEDULE_IdentifyPotentialToothBoundary_FINISHED:

        break;
    case ToothSegmentation::SCHEDULE_AutomaticCuttingOfGingiva_FINISHED:
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
        break;
    case ToothSegmentation::SCHEDULE_BoundarySkeletonExtraction_FINISHED:

        break;
    case ToothSegmentation::SCHEDULE_FindCuttingPoints_FINISHED:
        mToothSegmentation->refineToothBoundary(false);
        saveToothSegmentationHistory();
        gv->removeAllMeshes();
        gv->addMesh(mToothSegmentation->getToothMesh());
        if(mToothSegmentation->shouldShowExtraMesh()) {
            gv->addMesh(mToothSegmentation->getExtraMesh());
        }
        QMessageBox::information(this, tr("Info"), tr("Refine tooth boundary done!\nThe whole program completed!"));
        gv->updateGL();
        break;
    case ToothSegmentation::SCHEDULE_RefineToothBoundary_FINISHED:

        disconnect(mToothSegmentation, SIGNAL(onSaveHistory()), this, SLOT(saveToothSegmentationHistory()));
        disconnect(mToothSegmentation, SIGNAL(onProgramScheduleChanged(int)), this, SLOT(changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(int)));
        QMessageBox::information(this, tr("Info"), tr("The whole program completed!"));
        break;
    }
}

void SW::MainWindow::keyPressEvent(QKeyEvent *e)
{
    if((e->modifiers() & Qt::ControlModifier)&& mCurrentProcessMode == SEGMENTATION_MODE) //"Ctrl"
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

                //更新toolbar按钮状态
                changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(mToothSegmentation->getProgramSchedule());
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

                //更新toolbar按钮状态
                changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(mToothSegmentation->getProgramSchedule());
            }
            break;
        }
    }
}

Mesh SW::MainWindow::boolOperation(BooleanOperation type){

    m_OrignalMeshForBooleanOpearion[0] = gv->getMesh(0) ;
    m_OrignalMeshForBooleanOpearion[1] = gv->getMesh(1) ;
    TransformMode bool_operation(gv->getMesh(0),gv->getMesh(1));
    Mesh result;
    switch (type) {
    case UNION:
        result=bool_operation.Bool_Action('u');
        break;
    case INTERSECTION:
        result= bool_operation.Bool_Action('i');
        break;
    case DIFFERENCE:
        result = bool_operation.Bool_Action('d');
        break;
    default:
        break;
    }

    return result;
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

void SW::MainWindow::changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(int programSchedule)
{
    switch(programSchedule)    {
    case ToothSegmentation::SCHEDULE_START:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_IdentifyPotentialToothBoundary_STARTED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setChecked(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_IdentifyPotentialToothBoundary_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_AutomaticCuttingOfGingiva_STARTED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setChecked(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_AutomaticCuttingOfGingiva_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(true);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(true);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(true);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(true);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(true);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(true);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(true);
        break;
    case ToothSegmentation::SCHEDULE_BoundarySkeletonExtraction_STARTED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setChecked(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_BoundarySkeletonExtraction_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_FindCuttingPoints_STARTED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setChecked(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_FindCuttingPoints_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(true);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(true);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_RefineToothBoundary_STARTED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_pause.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setChecked(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setChecked(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    case ToothSegmentation::SCHEDULE_RefineToothBoundary_FINISHED:
        actionToothSegmentationProgramControl->setIcon(QIcon(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"));
        actionToothSegmentationProgramControl->setEnabled(true);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        break;
    }
}
