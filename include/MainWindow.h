#ifndef SW_MAINWINDOW_H
#define SW_MAINWINDOW_H

//
//  BoundingBox.h
//  Reconstructor
//
//  Created by sway on 6/13/15.
//  Copyright (c) 2015 None. All rights reserved.
//


#include "GLViewer.h"
#include "ui_mainwindow.h"
#include "include/Mesh.h"

#include <QMainWindow>
#include "qlayout.h"
#include "qcursor.h"
#include "qevent.h"
#include <QWidget>
#include "qnamespace.h"
#include <QFrame>
#include <QSlider>
#include <QCheckBox>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QListWidget>

#include "ToothSegmentation.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem; //
//class QActionGroup;
//class QToolBar;
//class QMenu;
//class QSlider;
//class QFrame;
//class QCheckBox;
class QPushButton;
class QTimer;
class QKeyEvent;
class GLViewer;

namespace SW
{
    class MainWindow: public QMainWindow, public Ui::MainWindow
    {
        Q_OBJECT

    public:

         MainWindow();
        ~MainWindow(); // to be implemented

    protected:

        virtual void keyPressEvent(QKeyEvent *);

    protected slots:
        void doActionOpen();
        void doActionCloseAll();
        void doActionLaplacianDeformation();
        void doActionUnion();

        void doActionToothSegmentationIdentifyPotentialToothBoundary();
        void doActionToothSegmentationAutomaticCuttingOfGingiva();
        void doActionToothSegmentationAutomaticCuttingOfGingivaFlipCuttingPlane();
        void doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneUp();
        void doActionToothSegmentationAutomaticCuttingOfGingivaMoveCuttingPlaneDown();
        void doActionToothSegmentationBoundarySkeletonExtraction();
        void doActionToothSegmentationFindCuttingPoints();
        void doActionToothSegmentationRefineToothBoundary();
        void doActionToothSegmentationManuallyShowVertexProperties(bool checked);
        void doActionToothSegmentationManuallyAddBoundaryVertex(bool checked);
        void doActionToothSegmentationManuallyDeleteBoundaryVertex(bool checked);
        void doActionToothSegmentationManuallyDeleteErrorToothRegion(bool checked);
        void doActionToothSegmentationManuallyDeleteErrorContourSection(bool checked);
        void doActionToothSegmentationEnableManualOperation(bool enable);
        void doActionToothSegmentationProgramControl();

    public slots:
        void saveToothSegmentationHistory();
        void changeToolbarButtonStatusAccordingToToothSegmentationProgramSchedule(int programSchedule);

    private:
        void setAllManualOperationActionUnChecked();
        void setOtherManualOperationActionUnChecked(QAction *checkedAction);

    public:
        //////////////////////定义窗口部件/////////////////////////////////////

    signals:

    private:
        ToothSegmentation *mToothSegmentation;
        QVector<QAction *> mToothSegmentationManualOperationActions;
        QVector<ToothSegmentation> mToothSegmentationHistory;
        int mToothSegmentationUsingIndexInHistory;

    };

}



#endif // SW_MAINWINDOW_H


