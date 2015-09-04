/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Sep 3 19:48:31 2015
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include <include/GLViewer.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *fileOpenAction;
    QAction *fileSaveAction;
    QAction *fileSaveAsAction;
    QAction *exitAction;
    QAction *selectPointsAction;
    QAction *setBackGroundAction;
    QAction *helpAboutAction;
    QAction *resetAction;
    QAction *extractPlaneBoundaryAction;
    QAction *displayAllPlanesAction;
    QAction *displaySinglePlaneAction;
    QAction *displayAllPolygonsAction;
    QAction *displaySinglePolygonAction;
    QAction *displayPointCloudAction;
    QAction *exportOFFFileAction;
    QAction *delaunary_TriangulationAction;
    QAction *displayDelaunaryTriangulationAction;
    QAction *texture_MappingAction;
    QAction *loadImageAction;
    QAction *loadProjectionMatrixAction;
    QAction *displayPolygonOnImageAction;
    QAction *triangulationBrokenAction;
    QAction *displayAxisAction;
    QAction *displayGridAction;
    QAction *attitudeAdjustmentAction;
    QAction *exportProjectionMatrixFileAction;
    QAction *structureOptimizationAction;
    QAction *InconsistentDetectionAction;
    QAction *RANSACFittingPlanesAction;
    QAction *ExportPlaneInfromationAction;
    QAction *LoadPlaneInformationAction;
    QAction *TestPhotoConsistencyAction;
    QAction *LoadVisibilityAction;
    QAction *LoadCameraInformationAction;
    QAction *ExportCameraInformationAction;
    QAction *loadPointCloudFromXMLAction;
    QAction *savePointCloudAsXMLAction;
    QAction *displayCamrerasAction;
    QAction *floorPlanReconstructionAction;
    QAction *SuperPixelsAction;
    QAction *loadModelAction;
    QAction *saveModelAction;
    QAction *displaySinglePlaneDTsAction;
    QAction *displayModelResultsAction;
    QAction *actionDisplay_Texture_Model;
    QAction *actionLoad_Texure_Model;
    QAction *actionSave_Texture_Model;
    QAction *actionOpen;
    QAction *actionLaplacianDeformation;
    QAction *actionUnion;
    QAction *actionDisplayVertices;
    QAction *actionDisplayWireFrame;
    QAction *actionDisplayFlatLine;
    QAction *actionCloseAll;
    QAction *actionToothSegmentationIdentifyPotentialToothBoundary;
    QAction *actionToothSegmentationAutomaticCuttingOfGingiva;
    QAction *actionToothSegmentationBoundarySkeletonExtraction;
    QAction *actionToothSegmentationRefineToothBoundary;
    QAction *actionToothSegmentationFindCuttingPoints;
    QAction *actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane;
    QAction *actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp;
    QAction *actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown;
    QAction *actionToothSegmentationManuallyAddBoundaryVertex;
    QAction *actionToothSegmentationManuallyDeleteBoundaryVertex;
    QAction *actionToothSegmentationManuallyShowVertexProperties;
    QAction *actionToothSegmentationManuallyDeleteErrorToothRegion;
    QAction *actionToothSegmentationManuallyDeleteErrorContourSection;
    QAction *actionToothSegmentationEnableManualOperation;
    QAction *actionToothSegmentationProgramControl;
    QAction *actionToothSegmentationTour;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    SW::GLViewer *gv;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toothSegmentationMenu;
    QMenu *settingMenu;
    QMenu *displayMenu;
    QMenu *helpMenu;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(830, 753);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        fileOpenAction = new QAction(MainWindow);
        fileOpenAction->setObjectName(QString::fromUtf8("fileOpenAction"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/fileopen"), QSize(), QIcon::Normal, QIcon::Off);
        fileOpenAction->setIcon(icon);
        fileSaveAction = new QAction(MainWindow);
        fileSaveAction->setObjectName(QString::fromUtf8("fileSaveAction"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/filesave"), QSize(), QIcon::Normal, QIcon::Off);
        fileSaveAction->setIcon(icon1);
        fileSaveAsAction = new QAction(MainWindow);
        fileSaveAsAction->setObjectName(QString::fromUtf8("fileSaveAsAction"));
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        selectPointsAction = new QAction(MainWindow);
        selectPointsAction->setObjectName(QString::fromUtf8("selectPointsAction"));
        selectPointsAction->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/pointsSlection.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectPointsAction->setIcon(icon2);
        setBackGroundAction = new QAction(MainWindow);
        setBackGroundAction->setObjectName(QString::fromUtf8("setBackGroundAction"));
        helpAboutAction = new QAction(MainWindow);
        helpAboutAction->setObjectName(QString::fromUtf8("helpAboutAction"));
        resetAction = new QAction(MainWindow);
        resetAction->setObjectName(QString::fromUtf8("resetAction"));
        extractPlaneBoundaryAction = new QAction(MainWindow);
        extractPlaneBoundaryAction->setObjectName(QString::fromUtf8("extractPlaneBoundaryAction"));
        extractPlaneBoundaryAction->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/boundaryExtraction.png"), QSize(), QIcon::Normal, QIcon::Off);
        extractPlaneBoundaryAction->setIcon(icon3);
        displayAllPlanesAction = new QAction(MainWindow);
        displayAllPlanesAction->setObjectName(QString::fromUtf8("displayAllPlanesAction"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/showALLPlanes.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAllPlanesAction->setIcon(icon4);
        displaySinglePlaneAction = new QAction(MainWindow);
        displaySinglePlaneAction->setObjectName(QString::fromUtf8("displaySinglePlaneAction"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/showSinglePlane.png"), QSize(), QIcon::Normal, QIcon::Off);
        displaySinglePlaneAction->setIcon(icon5);
        displayAllPolygonsAction = new QAction(MainWindow);
        displayAllPolygonsAction->setObjectName(QString::fromUtf8("displayAllPolygonsAction"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/showAllPolygons.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAllPolygonsAction->setIcon(icon6);
        displaySinglePolygonAction = new QAction(MainWindow);
        displaySinglePolygonAction->setObjectName(QString::fromUtf8("displaySinglePolygonAction"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/showSinglePolygon.png"), QSize(), QIcon::Normal, QIcon::Off);
        displaySinglePolygonAction->setIcon(icon7);
        displayPointCloudAction = new QAction(MainWindow);
        displayPointCloudAction->setObjectName(QString::fromUtf8("displayPointCloudAction"));
        displayPointCloudAction->setCheckable(true);
        displayPointCloudAction->setChecked(true);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/MeshLive1.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        displayPointCloudAction->setIcon(icon8);
        exportOFFFileAction = new QAction(MainWindow);
        exportOFFFileAction->setObjectName(QString::fromUtf8("exportOFFFileAction"));
        delaunary_TriangulationAction = new QAction(MainWindow);
        delaunary_TriangulationAction->setObjectName(QString::fromUtf8("delaunary_TriangulationAction"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/flat.png"), QSize(), QIcon::Normal, QIcon::Off);
        delaunary_TriangulationAction->setIcon(icon9);
        displayDelaunaryTriangulationAction = new QAction(MainWindow);
        displayDelaunaryTriangulationAction->setObjectName(QString::fromUtf8("displayDelaunaryTriangulationAction"));
        displayDelaunaryTriangulationAction->setCheckable(true);
        displayDelaunaryTriangulationAction->setChecked(false);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/empty_sphere.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        displayDelaunaryTriangulationAction->setIcon(icon10);
        texture_MappingAction = new QAction(MainWindow);
        texture_MappingAction->setObjectName(QString::fromUtf8("texture_MappingAction"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/textureMapping.png"), QSize(), QIcon::Normal, QIcon::Off);
        texture_MappingAction->setIcon(icon11);
        loadImageAction = new QAction(MainWindow);
        loadImageAction->setObjectName(QString::fromUtf8("loadImageAction"));
        loadProjectionMatrixAction = new QAction(MainWindow);
        loadProjectionMatrixAction->setObjectName(QString::fromUtf8("loadProjectionMatrixAction"));
        displayPolygonOnImageAction = new QAction(MainWindow);
        displayPolygonOnImageAction->setObjectName(QString::fromUtf8("displayPolygonOnImageAction"));
        displayPolygonOnImageAction->setCheckable(true);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/show_facet.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        displayPolygonOnImageAction->setIcon(icon12);
        triangulationBrokenAction = new QAction(MainWindow);
        triangulationBrokenAction->setObjectName(QString::fromUtf8("triangulationBrokenAction"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/triangulation_devide.png"), QSize(), QIcon::Normal, QIcon::Off);
        triangulationBrokenAction->setIcon(icon13);
        displayAxisAction = new QAction(MainWindow);
        displayAxisAction->setObjectName(QString::fromUtf8("displayAxisAction"));
        displayAxisAction->setCheckable(true);
        displayAxisAction->setChecked(true);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/axis.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAxisAction->setIcon(icon14);
        displayGridAction = new QAction(MainWindow);
        displayGridAction->setObjectName(QString::fromUtf8("displayGridAction"));
        displayGridAction->setCheckable(true);
        displayGridAction->setChecked(false);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/grid.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayGridAction->setIcon(icon15);
        attitudeAdjustmentAction = new QAction(MainWindow);
        attitudeAdjustmentAction->setObjectName(QString::fromUtf8("attitudeAdjustmentAction"));
        attitudeAdjustmentAction->setCheckable(true);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/qtlogo.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        attitudeAdjustmentAction->setIcon(icon16);
        exportProjectionMatrixFileAction = new QAction(MainWindow);
        exportProjectionMatrixFileAction->setObjectName(QString::fromUtf8("exportProjectionMatrixFileAction"));
        structureOptimizationAction = new QAction(MainWindow);
        structureOptimizationAction->setObjectName(QString::fromUtf8("structureOptimizationAction"));
        structureOptimizationAction->setCheckable(true);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/structureOptimization.png"), QSize(), QIcon::Normal, QIcon::Off);
        structureOptimizationAction->setIcon(icon17);
        InconsistentDetectionAction = new QAction(MainWindow);
        InconsistentDetectionAction->setObjectName(QString::fromUtf8("InconsistentDetectionAction"));
        InconsistentDetectionAction->setCheckable(true);
        RANSACFittingPlanesAction = new QAction(MainWindow);
        RANSACFittingPlanesAction->setObjectName(QString::fromUtf8("RANSACFittingPlanesAction"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/\351\241\271\347\233\25618.ico"), QSize(), QIcon::Normal, QIcon::Off);
        RANSACFittingPlanesAction->setIcon(icon18);
        ExportPlaneInfromationAction = new QAction(MainWindow);
        ExportPlaneInfromationAction->setObjectName(QString::fromUtf8("ExportPlaneInfromationAction"));
        LoadPlaneInformationAction = new QAction(MainWindow);
        LoadPlaneInformationAction->setObjectName(QString::fromUtf8("LoadPlaneInformationAction"));
        TestPhotoConsistencyAction = new QAction(MainWindow);
        TestPhotoConsistencyAction->setObjectName(QString::fromUtf8("TestPhotoConsistencyAction"));
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/preferences.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        TestPhotoConsistencyAction->setIcon(icon19);
        LoadVisibilityAction = new QAction(MainWindow);
        LoadVisibilityAction->setObjectName(QString::fromUtf8("LoadVisibilityAction"));
        LoadCameraInformationAction = new QAction(MainWindow);
        LoadCameraInformationAction->setObjectName(QString::fromUtf8("LoadCameraInformationAction"));
        ExportCameraInformationAction = new QAction(MainWindow);
        ExportCameraInformationAction->setObjectName(QString::fromUtf8("ExportCameraInformationAction"));
        loadPointCloudFromXMLAction = new QAction(MainWindow);
        loadPointCloudFromXMLAction->setObjectName(QString::fromUtf8("loadPointCloudFromXMLAction"));
        savePointCloudAsXMLAction = new QAction(MainWindow);
        savePointCloudAsXMLAction->setObjectName(QString::fromUtf8("savePointCloudAsXMLAction"));
        displayCamrerasAction = new QAction(MainWindow);
        displayCamrerasAction->setObjectName(QString::fromUtf8("displayCamrerasAction"));
        displayCamrerasAction->setCheckable(true);
        displayCamrerasAction->setChecked(false);
        floorPlanReconstructionAction = new QAction(MainWindow);
        floorPlanReconstructionAction->setObjectName(QString::fromUtf8("floorPlanReconstructionAction"));
        floorPlanReconstructionAction->setCheckable(true);
        SuperPixelsAction = new QAction(MainWindow);
        SuperPixelsAction->setObjectName(QString::fromUtf8("SuperPixelsAction"));
        loadModelAction = new QAction(MainWindow);
        loadModelAction->setObjectName(QString::fromUtf8("loadModelAction"));
        saveModelAction = new QAction(MainWindow);
        saveModelAction->setObjectName(QString::fromUtf8("saveModelAction"));
        displaySinglePlaneDTsAction = new QAction(MainWindow);
        displaySinglePlaneDTsAction->setObjectName(QString::fromUtf8("displaySinglePlaneDTsAction"));
        displaySinglePlaneDTsAction->setCheckable(true);
        displayModelResultsAction = new QAction(MainWindow);
        displayModelResultsAction->setObjectName(QString::fromUtf8("displayModelResultsAction"));
        displayModelResultsAction->setCheckable(true);
        actionDisplay_Texture_Model = new QAction(MainWindow);
        actionDisplay_Texture_Model->setObjectName(QString::fromUtf8("actionDisplay_Texture_Model"));
        actionDisplay_Texture_Model->setCheckable(true);
        actionLoad_Texure_Model = new QAction(MainWindow);
        actionLoad_Texure_Model->setObjectName(QString::fromUtf8("actionLoad_Texure_Model"));
        actionSave_Texture_Model = new QAction(MainWindow);
        actionSave_Texture_Model->setObjectName(QString::fromUtf8("actionSave_Texture_Model"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionLaplacianDeformation = new QAction(MainWindow);
        actionLaplacianDeformation->setObjectName(QString::fromUtf8("actionLaplacianDeformation"));
        actionUnion = new QAction(MainWindow);
        actionUnion->setObjectName(QString::fromUtf8("actionUnion"));
        actionDisplayVertices = new QAction(MainWindow);
        actionDisplayVertices->setObjectName(QString::fromUtf8("actionDisplayVertices"));
        actionDisplayWireFrame = new QAction(MainWindow);
        actionDisplayWireFrame->setObjectName(QString::fromUtf8("actionDisplayWireFrame"));
        actionDisplayFlatLine = new QAction(MainWindow);
        actionDisplayFlatLine->setObjectName(QString::fromUtf8("actionDisplayFlatLine"));
        actionCloseAll = new QAction(MainWindow);
        actionCloseAll->setObjectName(QString::fromUtf8("actionCloseAll"));
        actionToothSegmentationIdentifyPotentialToothBoundary = new QAction(MainWindow);
        actionToothSegmentationIdentifyPotentialToothBoundary->setObjectName(QString::fromUtf8("actionToothSegmentationIdentifyPotentialToothBoundary"));
        actionToothSegmentationAutomaticCuttingOfGingiva = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingiva->setObjectName(QString::fromUtf8("actionToothSegmentationAutomaticCuttingOfGingiva"));
        actionToothSegmentationBoundarySkeletonExtraction = new QAction(MainWindow);
        actionToothSegmentationBoundarySkeletonExtraction->setObjectName(QString::fromUtf8("actionToothSegmentationBoundarySkeletonExtraction"));
        actionToothSegmentationRefineToothBoundary = new QAction(MainWindow);
        actionToothSegmentationRefineToothBoundary->setObjectName(QString::fromUtf8("actionToothSegmentationRefineToothBoundary"));
        actionToothSegmentationFindCuttingPoints = new QAction(MainWindow);
        actionToothSegmentationFindCuttingPoints->setObjectName(QString::fromUtf8("actionToothSegmentationFindCuttingPoints"));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setObjectName(QString::fromUtf8("actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane"));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_flip_cutting_plane.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setIcon(icon20);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setObjectName(QString::fromUtf8("actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp"));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_move_cutting_plane_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setIcon(icon21);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setObjectName(QString::fromUtf8("actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown"));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_move_cutting_plane_down.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setIcon(icon22);
        actionToothSegmentationManuallyAddBoundaryVertex = new QAction(MainWindow);
        actionToothSegmentationManuallyAddBoundaryVertex->setObjectName(QString::fromUtf8("actionToothSegmentationManuallyAddBoundaryVertex"));
        actionToothSegmentationManuallyAddBoundaryVertex->setCheckable(true);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_add_boundary_vertex.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyAddBoundaryVertex->setIcon(icon23);
        actionToothSegmentationManuallyDeleteBoundaryVertex = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setObjectName(QString::fromUtf8("actionToothSegmentationManuallyDeleteBoundaryVertex"));
        actionToothSegmentationManuallyDeleteBoundaryVertex->setCheckable(true);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        QIcon icon24;
        icon24.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_delete_boundary_vertex.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setIcon(icon24);
        actionToothSegmentationManuallyShowVertexProperties = new QAction(MainWindow);
        actionToothSegmentationManuallyShowVertexProperties->setObjectName(QString::fromUtf8("actionToothSegmentationManuallyShowVertexProperties"));
        actionToothSegmentationManuallyShowVertexProperties->setCheckable(true);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        QIcon icon25;
        icon25.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_show_vertex_properties.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyShowVertexProperties->setIcon(icon25);
        actionToothSegmentationManuallyDeleteErrorToothRegion = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setObjectName(QString::fromUtf8("actionToothSegmentationManuallyDeleteErrorToothRegion"));
        actionToothSegmentationManuallyDeleteErrorToothRegion->setCheckable(true);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        QIcon icon26;
        icon26.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_delete_error_tooth_region.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setIcon(icon26);
        actionToothSegmentationManuallyDeleteErrorContourSection = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteErrorContourSection->setObjectName(QString::fromUtf8("actionToothSegmentationManuallyDeleteErrorContourSection"));
        actionToothSegmentationManuallyDeleteErrorContourSection->setCheckable(true);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        QIcon icon27;
        icon27.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_delete_error_contour_section.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteErrorContourSection->setIcon(icon27);
        actionToothSegmentationEnableManualOperation = new QAction(MainWindow);
        actionToothSegmentationEnableManualOperation->setObjectName(QString::fromUtf8("actionToothSegmentationEnableManualOperation"));
        actionToothSegmentationEnableManualOperation->setCheckable(true);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        QIcon icon28;
        icon28.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_enable_manual_operation_normal.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon28.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_enable_manual_operation_checked.png"), QSize(), QIcon::Normal, QIcon::On);
        actionToothSegmentationEnableManualOperation->setIcon(icon28);
        actionToothSegmentationProgramControl = new QAction(MainWindow);
        actionToothSegmentationProgramControl->setObjectName(QString::fromUtf8("actionToothSegmentationProgramControl"));
        actionToothSegmentationProgramControl->setEnabled(false);
        QIcon icon29;
        icon29.addFile(QString::fromUtf8(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationProgramControl->setIcon(icon29);
        actionToothSegmentationTour = new QAction(MainWindow);
        actionToothSegmentationTour->setObjectName(QString::fromUtf8("actionToothSegmentationTour"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gv = new SW::GLViewer(centralwidget);
        gv->setObjectName(QString::fromUtf8("gv"));
        gv->setStyleSheet(QString::fromUtf8("QFrame{\n"
"background:rgb(207, 207, 207)\n"
"}"));
        gv->setFrameShape(QFrame::StyledPanel);
        gv->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(gv);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 830, 26));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        editMenu = new QMenu(menubar);
        editMenu->setObjectName(QString::fromUtf8("editMenu"));
        toothSegmentationMenu = new QMenu(editMenu);
        toothSegmentationMenu->setObjectName(QString::fromUtf8("toothSegmentationMenu"));
        settingMenu = new QMenu(menubar);
        settingMenu->setObjectName(QString::fromUtf8("settingMenu"));
        settingMenu->setEnabled(false);
        displayMenu = new QMenu(menubar);
        displayMenu->setObjectName(QString::fromUtf8("displayMenu"));
        helpMenu = new QMenu(menubar);
        helpMenu->setObjectName(QString::fromUtf8("helpMenu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setEnabled(true);
        toolBar->setStyleSheet(QString::fromUtf8("QMainWindow{\n"
"background: rgb(95, 95, 95);\n"
"}"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(editMenu->menuAction());
        menubar->addAction(settingMenu->menuAction());
        menubar->addAction(displayMenu->menuAction());
        menubar->addAction(helpMenu->menuAction());
        fileMenu->addAction(actionOpen);
        fileMenu->addSeparator();
        fileMenu->addAction(actionCloseAll);
        editMenu->addAction(actionLaplacianDeformation);
        editMenu->addAction(actionUnion);
        editMenu->addSeparator();
        editMenu->addAction(toothSegmentationMenu->menuAction());
        toothSegmentationMenu->addAction(actionToothSegmentationIdentifyPotentialToothBoundary);
        toothSegmentationMenu->addAction(actionToothSegmentationAutomaticCuttingOfGingiva);
        toothSegmentationMenu->addAction(actionToothSegmentationBoundarySkeletonExtraction);
        toothSegmentationMenu->addSeparator();
        toothSegmentationMenu->addAction(actionToothSegmentationFindCuttingPoints);
        toothSegmentationMenu->addSeparator();
        toothSegmentationMenu->addAction(actionToothSegmentationRefineToothBoundary);
        displayMenu->addAction(actionDisplayVertices);
        displayMenu->addAction(actionDisplayWireFrame);
        displayMenu->addAction(actionDisplayFlatLine);
        helpMenu->addAction(actionToothSegmentationTour);
        toolBar->addAction(actionToothSegmentationProgramControl);
        toolBar->addSeparator();
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane);
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp);
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown);
        toolBar->addSeparator();
        toolBar->addAction(actionToothSegmentationManuallyShowVertexProperties);
        toolBar->addAction(actionToothSegmentationManuallyAddBoundaryVertex);
        toolBar->addAction(actionToothSegmentationManuallyDeleteBoundaryVertex);
        toolBar->addAction(actionToothSegmentationManuallyDeleteErrorToothRegion);
        toolBar->addAction(actionToothSegmentationManuallyDeleteErrorContourSection);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        fileOpenAction->setText(QApplication::translate("MainWindow", "Load Point Cloud From PLY", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        fileOpenAction->setToolTip(QApplication::translate("MainWindow", "Open a PLY file to Load point cloud.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        fileOpenAction->setStatusTip(QApplication::translate("MainWindow", "Load point cloud from PLY file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        fileOpenAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        fileSaveAction->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        fileSaveAction->setToolTip(QApplication::translate("MainWindow", "Save a PLY file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        fileSaveAction->setStatusTip(QApplication::translate("MainWindow", "Save the current point cloud in the scene.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        fileSaveAction->setShortcut(QApplication::translate("MainWindow", "Shift+S", 0, QApplication::UnicodeUTF8));
        fileSaveAsAction->setText(QApplication::translate("MainWindow", "Save As", 0, QApplication::UnicodeUTF8));
        exitAction->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        exitAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        selectPointsAction->setText(QApplication::translate("MainWindow", "Select Points", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        selectPointsAction->setStatusTip(QApplication::translate("MainWindow", "Start selecting points by press the left button and draw a rectangle.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        selectPointsAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
        setBackGroundAction->setText(QApplication::translate("MainWindow", "BackGround Color", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        setBackGroundAction->setToolTip(QApplication::translate("MainWindow", "set BackGround Color", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        helpAboutAction->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        resetAction->setText(QApplication::translate("MainWindow", "Reset", 0, QApplication::UnicodeUTF8));
        extractPlaneBoundaryAction->setText(QApplication::translate("MainWindow", "Extract Boundary", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        extractPlaneBoundaryAction->setStatusTip(QApplication::translate("MainWindow", "Extract ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        displayAllPlanesAction->setText(QApplication::translate("MainWindow", "Display All Planes Points", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        displayAllPlanesAction->setStatusTip(QApplication::translate("MainWindow", "Display all the planes.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        displaySinglePlaneAction->setText(QApplication::translate("MainWindow", "Display Single Plane Points", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        displaySinglePlaneAction->setStatusTip(QApplication::translate("MainWindow", "Display single plane.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        displayAllPolygonsAction->setText(QApplication::translate("MainWindow", "Display All Polygons", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        displayAllPolygonsAction->setStatusTip(QApplication::translate("MainWindow", "Display all Polygons.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        displaySinglePolygonAction->setText(QApplication::translate("MainWindow", "Display  Single Polygon", 0, QApplication::UnicodeUTF8));
        displayPointCloudAction->setText(QApplication::translate("MainWindow", "Display Point Cloud", 0, QApplication::UnicodeUTF8));
        exportOFFFileAction->setText(QApplication::translate("MainWindow", "Export OFF File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        exportOFFFileAction->setStatusTip(QApplication::translate("MainWindow", "Not implemented!", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        delaunary_TriangulationAction->setText(QApplication::translate("MainWindow", "Delaunary Triangulation", 0, QApplication::UnicodeUTF8));
        displayDelaunaryTriangulationAction->setText(QApplication::translate("MainWindow", "Display All Plane Delaunary Triangulation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        displayDelaunaryTriangulationAction->setStatusTip(QApplication::translate("MainWindow", "Display Delaunary Triangulation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        displayDelaunaryTriangulationAction->setWhatsThis(QApplication::translate("MainWindow", "Seleting Points", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        texture_MappingAction->setText(QApplication::translate("MainWindow", "Texture&Mapping", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        texture_MappingAction->setStatusTip(QApplication::translate("MainWindow", "Texture Mapping", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        loadImageAction->setText(QApplication::translate("MainWindow", "Load Images", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        loadImageAction->setStatusTip(QApplication::translate("MainWindow", "Load texture images", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        loadProjectionMatrixAction->setText(QApplication::translate("MainWindow", "Load Projection Matrix", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        loadProjectionMatrixAction->setStatusTip(QApplication::translate("MainWindow", "Load camera projection matrix", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        displayPolygonOnImageAction->setText(QApplication::translate("MainWindow", "Display Polygon on Image", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        displayPolygonOnImageAction->setStatusTip(QApplication::translate("MainWindow", "Polygons projection on image. (Region in cyan rectangle is the valid texture region.)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        triangulationBrokenAction->setText(QApplication::translate("MainWindow", "Triangulation Broken", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        triangulationBrokenAction->setWhatsThis(QApplication::translate("MainWindow", "Action to broke a large triangulation in to some small ones", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        displayAxisAction->setText(QApplication::translate("MainWindow", "Display World Axis", 0, QApplication::UnicodeUTF8));
        displayGridAction->setText(QApplication::translate("MainWindow", "Display Grid in the XZ Plane", 0, QApplication::UnicodeUTF8));
        attitudeAdjustmentAction->setText(QApplication::translate("MainWindow", "Attitude Adjustment", 0, QApplication::UnicodeUTF8));
        exportProjectionMatrixFileAction->setText(QApplication::translate("MainWindow", "Export Projection Matrix File", 0, QApplication::UnicodeUTF8));
        structureOptimizationAction->setText(QApplication::translate("MainWindow", "structureOptimization", 0, QApplication::UnicodeUTF8));
        InconsistentDetectionAction->setText(QApplication::translate("MainWindow", "Inconsistent Region Detection", 0, QApplication::UnicodeUTF8));
        RANSACFittingPlanesAction->setText(QApplication::translate("MainWindow", "RANSAC Fitting Planes", 0, QApplication::UnicodeUTF8));
        ExportPlaneInfromationAction->setText(QApplication::translate("MainWindow", "Export Plane Infromation", 0, QApplication::UnicodeUTF8));
        LoadPlaneInformationAction->setText(QApplication::translate("MainWindow", "Load Plane Information", 0, QApplication::UnicodeUTF8));
        TestPhotoConsistencyAction->setText(QApplication::translate("MainWindow", "TestPhotoConsistency", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        TestPhotoConsistencyAction->setToolTip(QApplication::translate("MainWindow", "TestPhotoConsistency", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        LoadVisibilityAction->setText(QApplication::translate("MainWindow", "Load Visibility", 0, QApplication::UnicodeUTF8));
        LoadCameraInformationAction->setText(QApplication::translate("MainWindow", "Load Camera Information", 0, QApplication::UnicodeUTF8));
        ExportCameraInformationAction->setText(QApplication::translate("MainWindow", "Export Camera Information", 0, QApplication::UnicodeUTF8));
        loadPointCloudFromXMLAction->setText(QApplication::translate("MainWindow", "Load Point Cloud From XML", 0, QApplication::UnicodeUTF8));
        savePointCloudAsXMLAction->setText(QApplication::translate("MainWindow", "Save Point Cloud As XML", 0, QApplication::UnicodeUTF8));
        displayCamrerasAction->setText(QApplication::translate("MainWindow", "Display Camreras", 0, QApplication::UnicodeUTF8));
        floorPlanReconstructionAction->setText(QApplication::translate("MainWindow", "Floor Plan Reconstruction", 0, QApplication::UnicodeUTF8));
        SuperPixelsAction->setText(QApplication::translate("MainWindow", "SuperPixels", 0, QApplication::UnicodeUTF8));
        loadModelAction->setText(QApplication::translate("MainWindow", "Load Model From OFF", 0, QApplication::UnicodeUTF8));
        saveModelAction->setText(QApplication::translate("MainWindow", "Save Model As", 0, QApplication::UnicodeUTF8));
        displaySinglePlaneDTsAction->setText(QApplication::translate("MainWindow", "Display Single Plane Delaunary Triangulation", 0, QApplication::UnicodeUTF8));
        displayModelResultsAction->setText(QApplication::translate("MainWindow", "Display Model Results", 0, QApplication::UnicodeUTF8));
        actionDisplay_Texture_Model->setText(QApplication::translate("MainWindow", "Display Texture Model", 0, QApplication::UnicodeUTF8));
        actionLoad_Texure_Model->setText(QApplication::translate("MainWindow", "Load Texure Model", 0, QApplication::UnicodeUTF8));
        actionSave_Texture_Model->setText(QApplication::translate("MainWindow", "Save Texture Model", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionLaplacianDeformation->setText(QApplication::translate("MainWindow", "Laplacian Deformation", 0, QApplication::UnicodeUTF8));
        actionUnion->setText(QApplication::translate("MainWindow", "Union", 0, QApplication::UnicodeUTF8));
        actionDisplayVertices->setText(QApplication::translate("MainWindow", "Vertices", 0, QApplication::UnicodeUTF8));
        actionDisplayWireFrame->setText(QApplication::translate("MainWindow", "WireFrame", 0, QApplication::UnicodeUTF8));
        actionDisplayFlatLine->setText(QApplication::translate("MainWindow", "FlatLine", 0, QApplication::UnicodeUTF8));
        actionCloseAll->setText(QApplication::translate("MainWindow", "Close All", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationIdentifyPotentialToothBoundary->setText(QApplication::translate("MainWindow", "Identify potential tooth boundary", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationAutomaticCuttingOfGingiva->setText(QApplication::translate("MainWindow", "Automatic cutting of gingiva", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationBoundarySkeletonExtraction->setText(QApplication::translate("MainWindow", "Boundary skeleton extraction", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationRefineToothBoundary->setText(QApplication::translate("MainWindow", "Refine tooth boundary", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationFindCuttingPoints->setText(QApplication::translate("MainWindow", "Find cutting points", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setText(QApplication::translate("MainWindow", "Flip cutting plane", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setText(QApplication::translate("MainWindow", "Move cutting plane up", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setText(QApplication::translate("MainWindow", "Move cutting plane down", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationManuallyAddBoundaryVertex->setText(QApplication::translate("MainWindow", "Add boundary vertex", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationManuallyDeleteBoundaryVertex->setText(QApplication::translate("MainWindow", "Delete boundary vertex", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationManuallyShowVertexProperties->setText(QApplication::translate("MainWindow", "Show vertex properties", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationManuallyDeleteErrorToothRegion->setText(QApplication::translate("MainWindow", "Delete error tooth region", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationManuallyDeleteErrorContourSection->setText(QApplication::translate("MainWindow", "Delete error contour section", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationEnableManualOperation->setText(QApplication::translate("MainWindow", "Enable manual operation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionToothSegmentationEnableManualOperation->setToolTip(QApplication::translate("MainWindow", "Enable manual operation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionToothSegmentationProgramControl->setText(QApplication::translate("MainWindow", "Program control", 0, QApplication::UnicodeUTF8));
        actionToothSegmentationTour->setText(QApplication::translate("MainWindow", "Tooth Segmentation Tour", 0, QApplication::UnicodeUTF8));
        fileMenu->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        editMenu->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
        toothSegmentationMenu->setTitle(QApplication::translate("MainWindow", "Tooth Segmentation", 0, QApplication::UnicodeUTF8));
        settingMenu->setTitle(QApplication::translate("MainWindow", "&Setting", 0, QApplication::UnicodeUTF8));
        displayMenu->setTitle(QApplication::translate("MainWindow", "&Display", 0, QApplication::UnicodeUTF8));
        helpMenu->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        toolBar->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
