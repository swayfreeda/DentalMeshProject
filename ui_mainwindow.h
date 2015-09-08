/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
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
    QAction *actionIntersection;
    QAction *actionUnion_2;
    QAction *actionDifference;
    QAction *actionToothSegmentations;
    QAction *actionSelectPoints;
    QAction *actionDoDeformation;
    QAction *actionBooleanOperation;
    QAction *actionReset;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    SW::GLViewer *gv;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *settingMenu;
    QMenu *displayMenu;
    QMenu *helpMenu;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(830, 753);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setStyleSheet(QStringLiteral(""));
        fileOpenAction = new QAction(MainWindow);
        fileOpenAction->setObjectName(QStringLiteral("fileOpenAction"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/ccOpen.png"), QSize(), QIcon::Normal, QIcon::Off);
        fileOpenAction->setIcon(icon);
        fileSaveAction = new QAction(MainWindow);
        fileSaveAction->setObjectName(QStringLiteral("fileSaveAction"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/filesave"), QSize(), QIcon::Normal, QIcon::Off);
        fileSaveAction->setIcon(icon1);
        fileSaveAsAction = new QAction(MainWindow);
        fileSaveAsAction->setObjectName(QStringLiteral("fileSaveAsAction"));
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QStringLiteral("exitAction"));
        selectPointsAction = new QAction(MainWindow);
        selectPointsAction->setObjectName(QStringLiteral("selectPointsAction"));
        selectPointsAction->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/pointsSlection.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectPointsAction->setIcon(icon2);
        setBackGroundAction = new QAction(MainWindow);
        setBackGroundAction->setObjectName(QStringLiteral("setBackGroundAction"));
        helpAboutAction = new QAction(MainWindow);
        helpAboutAction->setObjectName(QStringLiteral("helpAboutAction"));
        resetAction = new QAction(MainWindow);
        resetAction->setObjectName(QStringLiteral("resetAction"));
        extractPlaneBoundaryAction = new QAction(MainWindow);
        extractPlaneBoundaryAction->setObjectName(QStringLiteral("extractPlaneBoundaryAction"));
        extractPlaneBoundaryAction->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/boundaryExtraction.png"), QSize(), QIcon::Normal, QIcon::Off);
        extractPlaneBoundaryAction->setIcon(icon3);
        displayAllPlanesAction = new QAction(MainWindow);
        displayAllPlanesAction->setObjectName(QStringLiteral("displayAllPlanesAction"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/showALLPlanes.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAllPlanesAction->setIcon(icon4);
        displaySinglePlaneAction = new QAction(MainWindow);
        displaySinglePlaneAction->setObjectName(QStringLiteral("displaySinglePlaneAction"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/showSinglePlane.png"), QSize(), QIcon::Normal, QIcon::Off);
        displaySinglePlaneAction->setIcon(icon5);
        displayAllPolygonsAction = new QAction(MainWindow);
        displayAllPolygonsAction->setObjectName(QStringLiteral("displayAllPolygonsAction"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/showAllPolygons.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAllPolygonsAction->setIcon(icon6);
        displaySinglePolygonAction = new QAction(MainWindow);
        displaySinglePolygonAction->setObjectName(QStringLiteral("displaySinglePolygonAction"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/showSinglePolygon.png"), QSize(), QIcon::Normal, QIcon::Off);
        displaySinglePolygonAction->setIcon(icon7);
        displayPointCloudAction = new QAction(MainWindow);
        displayPointCloudAction->setObjectName(QStringLiteral("displayPointCloudAction"));
        displayPointCloudAction->setCheckable(true);
        displayPointCloudAction->setChecked(true);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/MeshLive1.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        displayPointCloudAction->setIcon(icon8);
        exportOFFFileAction = new QAction(MainWindow);
        exportOFFFileAction->setObjectName(QStringLiteral("exportOFFFileAction"));
        delaunary_TriangulationAction = new QAction(MainWindow);
        delaunary_TriangulationAction->setObjectName(QStringLiteral("delaunary_TriangulationAction"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/flat.png"), QSize(), QIcon::Normal, QIcon::Off);
        delaunary_TriangulationAction->setIcon(icon9);
        displayDelaunaryTriangulationAction = new QAction(MainWindow);
        displayDelaunaryTriangulationAction->setObjectName(QStringLiteral("displayDelaunaryTriangulationAction"));
        displayDelaunaryTriangulationAction->setCheckable(true);
        displayDelaunaryTriangulationAction->setChecked(false);
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/empty_sphere.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        displayDelaunaryTriangulationAction->setIcon(icon10);
        texture_MappingAction = new QAction(MainWindow);
        texture_MappingAction->setObjectName(QStringLiteral("texture_MappingAction"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/textureMapping.png"), QSize(), QIcon::Normal, QIcon::Off);
        texture_MappingAction->setIcon(icon11);
        loadImageAction = new QAction(MainWindow);
        loadImageAction->setObjectName(QStringLiteral("loadImageAction"));
        loadProjectionMatrixAction = new QAction(MainWindow);
        loadProjectionMatrixAction->setObjectName(QStringLiteral("loadProjectionMatrixAction"));
        displayPolygonOnImageAction = new QAction(MainWindow);
        displayPolygonOnImageAction->setObjectName(QStringLiteral("displayPolygonOnImageAction"));
        displayPolygonOnImageAction->setCheckable(true);
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/show_facet.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        displayPolygonOnImageAction->setIcon(icon12);
        triangulationBrokenAction = new QAction(MainWindow);
        triangulationBrokenAction->setObjectName(QStringLiteral("triangulationBrokenAction"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/triangulation_devide.png"), QSize(), QIcon::Normal, QIcon::Off);
        triangulationBrokenAction->setIcon(icon13);
        displayAxisAction = new QAction(MainWindow);
        displayAxisAction->setObjectName(QStringLiteral("displayAxisAction"));
        displayAxisAction->setCheckable(true);
        displayAxisAction->setChecked(true);
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/axis.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayAxisAction->setIcon(icon14);
        displayGridAction = new QAction(MainWindow);
        displayGridAction->setObjectName(QStringLiteral("displayGridAction"));
        displayGridAction->setCheckable(true);
        displayGridAction->setChecked(false);
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/grid.png"), QSize(), QIcon::Normal, QIcon::Off);
        displayGridAction->setIcon(icon15);
        attitudeAdjustmentAction = new QAction(MainWindow);
        attitudeAdjustmentAction->setObjectName(QStringLiteral("attitudeAdjustmentAction"));
        attitudeAdjustmentAction->setCheckable(true);
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/qtlogo.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        attitudeAdjustmentAction->setIcon(icon16);
        exportProjectionMatrixFileAction = new QAction(MainWindow);
        exportProjectionMatrixFileAction->setObjectName(QStringLiteral("exportProjectionMatrixFileAction"));
        structureOptimizationAction = new QAction(MainWindow);
        structureOptimizationAction->setObjectName(QStringLiteral("structureOptimizationAction"));
        structureOptimizationAction->setCheckable(true);
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/structureOptimization.png"), QSize(), QIcon::Normal, QIcon::Off);
        structureOptimizationAction->setIcon(icon17);
        InconsistentDetectionAction = new QAction(MainWindow);
        InconsistentDetectionAction->setObjectName(QStringLiteral("InconsistentDetectionAction"));
        InconsistentDetectionAction->setCheckable(true);
        RANSACFittingPlanesAction = new QAction(MainWindow);
        RANSACFittingPlanesAction->setObjectName(QStringLiteral("RANSACFittingPlanesAction"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/\351\241\271\347\233\25618.ico"), QSize(), QIcon::Normal, QIcon::Off);
        RANSACFittingPlanesAction->setIcon(icon18);
        ExportPlaneInfromationAction = new QAction(MainWindow);
        ExportPlaneInfromationAction->setObjectName(QStringLiteral("ExportPlaneInfromationAction"));
        LoadPlaneInformationAction = new QAction(MainWindow);
        LoadPlaneInformationAction->setObjectName(QStringLiteral("LoadPlaneInformationAction"));
        TestPhotoConsistencyAction = new QAction(MainWindow);
        TestPhotoConsistencyAction->setObjectName(QStringLiteral("TestPhotoConsistencyAction"));
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/preferences.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        TestPhotoConsistencyAction->setIcon(icon19);
        LoadVisibilityAction = new QAction(MainWindow);
        LoadVisibilityAction->setObjectName(QStringLiteral("LoadVisibilityAction"));
        LoadCameraInformationAction = new QAction(MainWindow);
        LoadCameraInformationAction->setObjectName(QStringLiteral("LoadCameraInformationAction"));
        ExportCameraInformationAction = new QAction(MainWindow);
        ExportCameraInformationAction->setObjectName(QStringLiteral("ExportCameraInformationAction"));
        loadPointCloudFromXMLAction = new QAction(MainWindow);
        loadPointCloudFromXMLAction->setObjectName(QStringLiteral("loadPointCloudFromXMLAction"));
        savePointCloudAsXMLAction = new QAction(MainWindow);
        savePointCloudAsXMLAction->setObjectName(QStringLiteral("savePointCloudAsXMLAction"));
        displayCamrerasAction = new QAction(MainWindow);
        displayCamrerasAction->setObjectName(QStringLiteral("displayCamrerasAction"));
        displayCamrerasAction->setCheckable(true);
        displayCamrerasAction->setChecked(false);
        floorPlanReconstructionAction = new QAction(MainWindow);
        floorPlanReconstructionAction->setObjectName(QStringLiteral("floorPlanReconstructionAction"));
        floorPlanReconstructionAction->setCheckable(true);
        SuperPixelsAction = new QAction(MainWindow);
        SuperPixelsAction->setObjectName(QStringLiteral("SuperPixelsAction"));
        loadModelAction = new QAction(MainWindow);
        loadModelAction->setObjectName(QStringLiteral("loadModelAction"));
        saveModelAction = new QAction(MainWindow);
        saveModelAction->setObjectName(QStringLiteral("saveModelAction"));
        displaySinglePlaneDTsAction = new QAction(MainWindow);
        displaySinglePlaneDTsAction->setObjectName(QStringLiteral("displaySinglePlaneDTsAction"));
        displaySinglePlaneDTsAction->setCheckable(true);
        displayModelResultsAction = new QAction(MainWindow);
        displayModelResultsAction->setObjectName(QStringLiteral("displayModelResultsAction"));
        displayModelResultsAction->setCheckable(true);
        actionDisplay_Texture_Model = new QAction(MainWindow);
        actionDisplay_Texture_Model->setObjectName(QStringLiteral("actionDisplay_Texture_Model"));
        actionDisplay_Texture_Model->setCheckable(true);
        actionLoad_Texure_Model = new QAction(MainWindow);
        actionLoad_Texure_Model->setObjectName(QStringLiteral("actionLoad_Texure_Model"));
        actionSave_Texture_Model = new QAction(MainWindow);
        actionSave_Texture_Model->setObjectName(QStringLiteral("actionSave_Texture_Model"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionOpen->setIcon(icon);
        actionLaplacianDeformation = new QAction(MainWindow);
        actionLaplacianDeformation->setObjectName(QStringLiteral("actionLaplacianDeformation"));
        actionLaplacianDeformation->setCheckable(true);
        actionDisplayVertices = new QAction(MainWindow);
        actionDisplayVertices->setObjectName(QStringLiteral("actionDisplayVertices"));
        actionDisplayWireFrame = new QAction(MainWindow);
        actionDisplayWireFrame->setObjectName(QStringLiteral("actionDisplayWireFrame"));
        actionDisplayFlatLine = new QAction(MainWindow);
        actionDisplayFlatLine->setObjectName(QStringLiteral("actionDisplayFlatLine"));
        actionCloseAll = new QAction(MainWindow);
        actionCloseAll->setObjectName(QStringLiteral("actionCloseAll"));
        QIcon icon20;
        icon20.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/ccDelete.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCloseAll->setIcon(icon20);
        actionToothSegmentationIdentifyPotentialToothBoundary = new QAction(MainWindow);
        actionToothSegmentationIdentifyPotentialToothBoundary->setObjectName(QStringLiteral("actionToothSegmentationIdentifyPotentialToothBoundary"));
        actionToothSegmentationAutomaticCuttingOfGingiva = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingiva->setObjectName(QStringLiteral("actionToothSegmentationAutomaticCuttingOfGingiva"));
        actionToothSegmentationBoundarySkeletonExtraction = new QAction(MainWindow);
        actionToothSegmentationBoundarySkeletonExtraction->setObjectName(QStringLiteral("actionToothSegmentationBoundarySkeletonExtraction"));
        actionToothSegmentationRefineToothBoundary = new QAction(MainWindow);
        actionToothSegmentationRefineToothBoundary->setObjectName(QStringLiteral("actionToothSegmentationRefineToothBoundary"));
        actionToothSegmentationFindCuttingPoints = new QAction(MainWindow);
        actionToothSegmentationFindCuttingPoints->setObjectName(QStringLiteral("actionToothSegmentationFindCuttingPoints"));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setObjectName(QStringLiteral("actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane"));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setEnabled(false);
        QIcon icon21;
        icon21.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_flip_cutting_plane.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setIcon(icon21);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setObjectName(QStringLiteral("actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp"));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setEnabled(false);
        QIcon icon22;
        icon22.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_move_cutting_plane_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setIcon(icon22);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown = new QAction(MainWindow);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setObjectName(QStringLiteral("actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown"));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setEnabled(false);
        QIcon icon23;
        icon23.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_move_cutting_plane_down.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setIcon(icon23);
        actionToothSegmentationManuallyAddBoundaryVertex = new QAction(MainWindow);
        actionToothSegmentationManuallyAddBoundaryVertex->setObjectName(QStringLiteral("actionToothSegmentationManuallyAddBoundaryVertex"));
        actionToothSegmentationManuallyAddBoundaryVertex->setCheckable(true);
        actionToothSegmentationManuallyAddBoundaryVertex->setEnabled(false);
        QIcon icon24;
        icon24.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_add_boundary_vertex.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyAddBoundaryVertex->setIcon(icon24);
        actionToothSegmentationManuallyDeleteBoundaryVertex = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setObjectName(QStringLiteral("actionToothSegmentationManuallyDeleteBoundaryVertex"));
        actionToothSegmentationManuallyDeleteBoundaryVertex->setCheckable(true);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setEnabled(false);
        QIcon icon25;
        icon25.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_delete_boundary_vertex.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteBoundaryVertex->setIcon(icon25);
        actionToothSegmentationManuallyShowVertexProperties = new QAction(MainWindow);
        actionToothSegmentationManuallyShowVertexProperties->setObjectName(QStringLiteral("actionToothSegmentationManuallyShowVertexProperties"));
        actionToothSegmentationManuallyShowVertexProperties->setCheckable(true);
        actionToothSegmentationManuallyShowVertexProperties->setChecked(false);
        actionToothSegmentationManuallyShowVertexProperties->setEnabled(false);
        QIcon icon26;
        icon26.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_show_vertex_properties.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyShowVertexProperties->setIcon(icon26);
        actionToothSegmentationManuallyDeleteErrorToothRegion = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setObjectName(QStringLiteral("actionToothSegmentationManuallyDeleteErrorToothRegion"));
        actionToothSegmentationManuallyDeleteErrorToothRegion->setCheckable(true);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setEnabled(false);
        QIcon icon27;
        icon27.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_delete_error_tooth_region.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteErrorToothRegion->setIcon(icon27);
        actionToothSegmentationManuallyDeleteErrorContourSection = new QAction(MainWindow);
        actionToothSegmentationManuallyDeleteErrorContourSection->setObjectName(QStringLiteral("actionToothSegmentationManuallyDeleteErrorContourSection"));
        actionToothSegmentationManuallyDeleteErrorContourSection->setCheckable(true);
        actionToothSegmentationManuallyDeleteErrorContourSection->setEnabled(false);
        QIcon icon28;
        icon28.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_delete_error_contour_section.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationManuallyDeleteErrorContourSection->setIcon(icon28);
        actionToothSegmentationEnableManualOperation = new QAction(MainWindow);
        actionToothSegmentationEnableManualOperation->setObjectName(QStringLiteral("actionToothSegmentationEnableManualOperation"));
        actionToothSegmentationEnableManualOperation->setCheckable(true);
        actionToothSegmentationEnableManualOperation->setChecked(false);
        actionToothSegmentationEnableManualOperation->setEnabled(false);
        QIcon icon29;
        icon29.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_enable_manual_operation_normal.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon29.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_enable_manual_operation_checked.png"), QSize(), QIcon::Normal, QIcon::On);
        actionToothSegmentationEnableManualOperation->setIcon(icon29);
        actionToothSegmentationProgramControl = new QAction(MainWindow);
        actionToothSegmentationProgramControl->setObjectName(QStringLiteral("actionToothSegmentationProgramControl"));
        actionToothSegmentationProgramControl->setEnabled(false);
        QIcon icon30;
        icon30.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/toolbar_program_control_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToothSegmentationProgramControl->setIcon(icon30);
        actionToothSegmentationTour = new QAction(MainWindow);
        actionToothSegmentationTour->setObjectName(QStringLiteral("actionToothSegmentationTour"));
        actionIntersection = new QAction(MainWindow);
        actionIntersection->setObjectName(QStringLiteral("actionIntersection"));
        actionIntersection->setEnabled(false);
        QIcon icon31;
        icon31.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/jiao.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionIntersection->setIcon(icon31);
        actionUnion_2 = new QAction(MainWindow);
        actionUnion_2->setObjectName(QStringLiteral("actionUnion_2"));
        actionUnion_2->setEnabled(false);
        QIcon icon32;
        icon32.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/bing.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUnion_2->setIcon(icon32);
        actionDifference = new QAction(MainWindow);
        actionDifference->setObjectName(QStringLiteral("actionDifference"));
        actionDifference->setEnabled(false);
        QIcon icon33;
        icon33.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/cha.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDifference->setIcon(icon33);
        actionToothSegmentations = new QAction(MainWindow);
        actionToothSegmentations->setObjectName(QStringLiteral("actionToothSegmentations"));
        actionToothSegmentations->setCheckable(true);
        actionSelectPoints = new QAction(MainWindow);
        actionSelectPoints->setObjectName(QStringLiteral("actionSelectPoints"));
        actionSelectPoints->setCheckable(true);
        actionSelectPoints->setEnabled(false);
        QIcon icon34;
        icon34.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/selectpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectPoints->setIcon(icon34);
        actionDoDeformation = new QAction(MainWindow);
        actionDoDeformation->setObjectName(QStringLiteral("actionDoDeformation"));
        actionDoDeformation->setCheckable(true);
        actionDoDeformation->setEnabled(false);
        QIcon icon35;
        icon35.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/movePoint.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDoDeformation->setIcon(icon35);
        actionBooleanOperation = new QAction(MainWindow);
        actionBooleanOperation->setObjectName(QStringLiteral("actionBooleanOperation"));
        actionBooleanOperation->setCheckable(true);
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        actionReset->setEnabled(true);
        QIcon icon36;
        icon36.addFile(QStringLiteral(":/toolbar/ToothSegmentation/image/reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset->setIcon(icon36);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        gv = new SW::GLViewer(centralwidget);
        gv->setObjectName(QStringLiteral("gv"));
        gv->setStyleSheet(QLatin1String("QFrame{\n"
"background:rgb(207, 207, 207)\n"
"}"));
        gv->setFrameShape(QFrame::StyledPanel);
        gv->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(gv);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 830, 25));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QStringLiteral("fileMenu"));
        editMenu = new QMenu(menubar);
        editMenu->setObjectName(QStringLiteral("editMenu"));
        settingMenu = new QMenu(menubar);
        settingMenu->setObjectName(QStringLiteral("settingMenu"));
        settingMenu->setEnabled(false);
        displayMenu = new QMenu(menubar);
        displayMenu->setObjectName(QStringLiteral("displayMenu"));
        helpMenu = new QMenu(menubar);
        helpMenu->setObjectName(QStringLiteral("helpMenu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setEnabled(true);
        toolBar->setStyleSheet(QLatin1String("QMainWindow{\n"
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
        editMenu->addAction(actionToothSegmentations);
        editMenu->addAction(actionBooleanOperation);
        editMenu->addSeparator();
        editMenu->addAction(actionIntersection);
        editMenu->addAction(actionDifference);
        editMenu->addAction(actionUnion_2);
        editMenu->addSeparator();
        editMenu->addAction(actionSelectPoints);
        editMenu->addAction(actionDoDeformation);
        displayMenu->addAction(actionDisplayVertices);
        displayMenu->addAction(actionDisplayWireFrame);
        displayMenu->addAction(actionDisplayFlatLine);
        helpMenu->addAction(actionToothSegmentationTour);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionCloseAll);
        toolBar->addAction(actionReset);
        toolBar->addSeparator();
        toolBar->addAction(actionToothSegmentationProgramControl);
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane);
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp);
        toolBar->addAction(actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown);
        toolBar->addAction(actionToothSegmentationManuallyShowVertexProperties);
        toolBar->addAction(actionToothSegmentationManuallyAddBoundaryVertex);
        toolBar->addAction(actionToothSegmentationManuallyDeleteBoundaryVertex);
        toolBar->addAction(actionToothSegmentationManuallyDeleteErrorToothRegion);
        toolBar->addAction(actionToothSegmentationManuallyDeleteErrorContourSection);
        toolBar->addSeparator();
        toolBar->addAction(actionIntersection);
        toolBar->addAction(actionUnion_2);
        toolBar->addAction(actionDifference);
        toolBar->addSeparator();
        toolBar->addAction(actionSelectPoints);
        toolBar->addAction(actionDoDeformation);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        fileOpenAction->setText(QApplication::translate("MainWindow", "Load Point Cloud From PLY", 0));
#ifndef QT_NO_TOOLTIP
        fileOpenAction->setToolTip(QApplication::translate("MainWindow", "Open a PLY file to Load point cloud.", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        fileOpenAction->setStatusTip(QApplication::translate("MainWindow", "Load point cloud from PLY file", 0));
#endif // QT_NO_STATUSTIP
        fileOpenAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        fileSaveAction->setText(QApplication::translate("MainWindow", "Save", 0));
#ifndef QT_NO_TOOLTIP
        fileSaveAction->setToolTip(QApplication::translate("MainWindow", "Save a PLY file", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        fileSaveAction->setStatusTip(QApplication::translate("MainWindow", "Save the current point cloud in the scene.", 0));
#endif // QT_NO_STATUSTIP
        fileSaveAction->setShortcut(QApplication::translate("MainWindow", "Shift+S", 0));
        fileSaveAsAction->setText(QApplication::translate("MainWindow", "Save As", 0));
        exitAction->setText(QApplication::translate("MainWindow", "Exit", 0));
        exitAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0));
        selectPointsAction->setText(QApplication::translate("MainWindow", "Select Points", 0));
#ifndef QT_NO_STATUSTIP
        selectPointsAction->setStatusTip(QApplication::translate("MainWindow", "Start selecting points by press the left button and draw a rectangle.", 0));
#endif // QT_NO_STATUSTIP
        selectPointsAction->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0));
        setBackGroundAction->setText(QApplication::translate("MainWindow", "BackGround Color", 0));
#ifndef QT_NO_TOOLTIP
        setBackGroundAction->setToolTip(QApplication::translate("MainWindow", "set BackGround Color", 0));
#endif // QT_NO_TOOLTIP
        helpAboutAction->setText(QApplication::translate("MainWindow", "About", 0));
        resetAction->setText(QApplication::translate("MainWindow", "Reset", 0));
        extractPlaneBoundaryAction->setText(QApplication::translate("MainWindow", "Extract Boundary", 0));
#ifndef QT_NO_STATUSTIP
        extractPlaneBoundaryAction->setStatusTip(QApplication::translate("MainWindow", "Extract ", 0));
#endif // QT_NO_STATUSTIP
        displayAllPlanesAction->setText(QApplication::translate("MainWindow", "Display All Planes Points", 0));
#ifndef QT_NO_STATUSTIP
        displayAllPlanesAction->setStatusTip(QApplication::translate("MainWindow", "Display all the planes.", 0));
#endif // QT_NO_STATUSTIP
        displaySinglePlaneAction->setText(QApplication::translate("MainWindow", "Display Single Plane Points", 0));
#ifndef QT_NO_STATUSTIP
        displaySinglePlaneAction->setStatusTip(QApplication::translate("MainWindow", "Display single plane.", 0));
#endif // QT_NO_STATUSTIP
        displayAllPolygonsAction->setText(QApplication::translate("MainWindow", "Display All Polygons", 0));
#ifndef QT_NO_STATUSTIP
        displayAllPolygonsAction->setStatusTip(QApplication::translate("MainWindow", "Display all Polygons.", 0));
#endif // QT_NO_STATUSTIP
        displaySinglePolygonAction->setText(QApplication::translate("MainWindow", "Display  Single Polygon", 0));
        displayPointCloudAction->setText(QApplication::translate("MainWindow", "Display Point Cloud", 0));
        exportOFFFileAction->setText(QApplication::translate("MainWindow", "Export OFF File", 0));
#ifndef QT_NO_STATUSTIP
        exportOFFFileAction->setStatusTip(QApplication::translate("MainWindow", "Not implemented!", 0));
#endif // QT_NO_STATUSTIP
        delaunary_TriangulationAction->setText(QApplication::translate("MainWindow", "Delaunary Triangulation", 0));
        displayDelaunaryTriangulationAction->setText(QApplication::translate("MainWindow", "Display All Plane Delaunary Triangulation", 0));
#ifndef QT_NO_STATUSTIP
        displayDelaunaryTriangulationAction->setStatusTip(QApplication::translate("MainWindow", "Display Delaunary Triangulation", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        displayDelaunaryTriangulationAction->setWhatsThis(QApplication::translate("MainWindow", "Seleting Points", 0));
#endif // QT_NO_WHATSTHIS
        texture_MappingAction->setText(QApplication::translate("MainWindow", "Texture&Mapping", 0));
#ifndef QT_NO_STATUSTIP
        texture_MappingAction->setStatusTip(QApplication::translate("MainWindow", "Texture Mapping", 0));
#endif // QT_NO_STATUSTIP
        loadImageAction->setText(QApplication::translate("MainWindow", "Load Images", 0));
#ifndef QT_NO_STATUSTIP
        loadImageAction->setStatusTip(QApplication::translate("MainWindow", "Load texture images", 0));
#endif // QT_NO_STATUSTIP
        loadProjectionMatrixAction->setText(QApplication::translate("MainWindow", "Load Projection Matrix", 0));
#ifndef QT_NO_STATUSTIP
        loadProjectionMatrixAction->setStatusTip(QApplication::translate("MainWindow", "Load camera projection matrix", 0));
#endif // QT_NO_STATUSTIP
        displayPolygonOnImageAction->setText(QApplication::translate("MainWindow", "Display Polygon on Image", 0));
#ifndef QT_NO_STATUSTIP
        displayPolygonOnImageAction->setStatusTip(QApplication::translate("MainWindow", "Polygons projection on image. (Region in cyan rectangle is the valid texture region.)", 0));
#endif // QT_NO_STATUSTIP
        triangulationBrokenAction->setText(QApplication::translate("MainWindow", "Triangulation Broken", 0));
#ifndef QT_NO_WHATSTHIS
        triangulationBrokenAction->setWhatsThis(QApplication::translate("MainWindow", "Action to broke a large triangulation in to some small ones", 0));
#endif // QT_NO_WHATSTHIS
        displayAxisAction->setText(QApplication::translate("MainWindow", "Display World Axis", 0));
        displayGridAction->setText(QApplication::translate("MainWindow", "Display Grid in the XZ Plane", 0));
        attitudeAdjustmentAction->setText(QApplication::translate("MainWindow", "Attitude Adjustment", 0));
        exportProjectionMatrixFileAction->setText(QApplication::translate("MainWindow", "Export Projection Matrix File", 0));
        structureOptimizationAction->setText(QApplication::translate("MainWindow", "structureOptimization", 0));
        InconsistentDetectionAction->setText(QApplication::translate("MainWindow", "Inconsistent Region Detection", 0));
        RANSACFittingPlanesAction->setText(QApplication::translate("MainWindow", "RANSAC Fitting Planes", 0));
        ExportPlaneInfromationAction->setText(QApplication::translate("MainWindow", "Export Plane Infromation", 0));
        LoadPlaneInformationAction->setText(QApplication::translate("MainWindow", "Load Plane Information", 0));
        TestPhotoConsistencyAction->setText(QApplication::translate("MainWindow", "TestPhotoConsistency", 0));
#ifndef QT_NO_TOOLTIP
        TestPhotoConsistencyAction->setToolTip(QApplication::translate("MainWindow", "TestPhotoConsistency", 0));
#endif // QT_NO_TOOLTIP
        LoadVisibilityAction->setText(QApplication::translate("MainWindow", "Load Visibility", 0));
        LoadCameraInformationAction->setText(QApplication::translate("MainWindow", "Load Camera Information", 0));
        ExportCameraInformationAction->setText(QApplication::translate("MainWindow", "Export Camera Information", 0));
        loadPointCloudFromXMLAction->setText(QApplication::translate("MainWindow", "Load Point Cloud From XML", 0));
        savePointCloudAsXMLAction->setText(QApplication::translate("MainWindow", "Save Point Cloud As XML", 0));
        displayCamrerasAction->setText(QApplication::translate("MainWindow", "Display Camreras", 0));
        floorPlanReconstructionAction->setText(QApplication::translate("MainWindow", "Floor Plan Reconstruction", 0));
        SuperPixelsAction->setText(QApplication::translate("MainWindow", "SuperPixels", 0));
        loadModelAction->setText(QApplication::translate("MainWindow", "Load Model From OFF", 0));
        saveModelAction->setText(QApplication::translate("MainWindow", "Save Model As", 0));
        displaySinglePlaneDTsAction->setText(QApplication::translate("MainWindow", "Display Single Plane Delaunary Triangulation", 0));
        displayModelResultsAction->setText(QApplication::translate("MainWindow", "Display Model Results", 0));
        actionDisplay_Texture_Model->setText(QApplication::translate("MainWindow", "Display Texture Model", 0));
        actionLoad_Texure_Model->setText(QApplication::translate("MainWindow", "Load Texure Model", 0));
        actionSave_Texture_Model->setText(QApplication::translate("MainWindow", "Save Texture Model", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0));
        actionLaplacianDeformation->setText(QApplication::translate("MainWindow", "Laplacian Deformation", 0));
        actionDisplayVertices->setText(QApplication::translate("MainWindow", "Vertices", 0));
        actionDisplayWireFrame->setText(QApplication::translate("MainWindow", "WireFrame", 0));
        actionDisplayFlatLine->setText(QApplication::translate("MainWindow", "FlatLine", 0));
        actionCloseAll->setText(QApplication::translate("MainWindow", "Close All", 0));
        actionToothSegmentationIdentifyPotentialToothBoundary->setText(QApplication::translate("MainWindow", "Identify potential tooth boundary", 0));
        actionToothSegmentationAutomaticCuttingOfGingiva->setText(QApplication::translate("MainWindow", "Automatic cutting of gingiva", 0));
        actionToothSegmentationBoundarySkeletonExtraction->setText(QApplication::translate("MainWindow", "Boundary skeleton extraction", 0));
        actionToothSegmentationRefineToothBoundary->setText(QApplication::translate("MainWindow", "Refine tooth boundary", 0));
        actionToothSegmentationFindCuttingPoints->setText(QApplication::translate("MainWindow", "Find cutting points", 0));
        actionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane->setText(QApplication::translate("MainWindow", "Flip cutting plane", 0));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp->setText(QApplication::translate("MainWindow", "Move cutting plane up", 0));
        actionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown->setText(QApplication::translate("MainWindow", "Move cutting plane down", 0));
        actionToothSegmentationManuallyAddBoundaryVertex->setText(QApplication::translate("MainWindow", "Add boundary vertex", 0));
        actionToothSegmentationManuallyDeleteBoundaryVertex->setText(QApplication::translate("MainWindow", "Delete boundary vertex", 0));
        actionToothSegmentationManuallyShowVertexProperties->setText(QApplication::translate("MainWindow", "Show vertex properties", 0));
        actionToothSegmentationManuallyDeleteErrorToothRegion->setText(QApplication::translate("MainWindow", "Delete error tooth region", 0));
        actionToothSegmentationManuallyDeleteErrorContourSection->setText(QApplication::translate("MainWindow", "Delete error contour section", 0));
        actionToothSegmentationEnableManualOperation->setText(QApplication::translate("MainWindow", "Enable manual operation", 0));
#ifndef QT_NO_TOOLTIP
        actionToothSegmentationEnableManualOperation->setToolTip(QApplication::translate("MainWindow", "Enable manual operation", 0));
#endif // QT_NO_TOOLTIP
        actionToothSegmentationProgramControl->setText(QApplication::translate("MainWindow", "Program control", 0));
        actionToothSegmentationTour->setText(QApplication::translate("MainWindow", "Tooth Segmentation Tour", 0));
        actionIntersection->setText(QApplication::translate("MainWindow", "Intersection", 0));
        actionUnion_2->setText(QApplication::translate("MainWindow", "Union", 0));
        actionDifference->setText(QApplication::translate("MainWindow", "Difference", 0));
        actionToothSegmentations->setText(QApplication::translate("MainWindow", "Tooth Segmentation", 0));
        actionSelectPoints->setText(QApplication::translate("MainWindow", "Select Points", 0));
        actionDoDeformation->setText(QApplication::translate("MainWindow", "Do Deformation", 0));
        actionBooleanOperation->setText(QApplication::translate("MainWindow", "Boolean Operation", 0));
        actionReset->setText(QApplication::translate("MainWindow", "Reset", 0));
        fileMenu->setTitle(QApplication::translate("MainWindow", "&File", 0));
        editMenu->setTitle(QApplication::translate("MainWindow", "&Edit", 0));
        settingMenu->setTitle(QApplication::translate("MainWindow", "&Setting", 0));
        displayMenu->setTitle(QApplication::translate("MainWindow", "&Display", 0));
        helpMenu->setTitle(QApplication::translate("MainWindow", "&Help", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
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
