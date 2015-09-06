#include "ToothSegmentation.h"
#include "Mesh.h"
#include "MainWindow.h"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

//#include <igl/read_triangle_mesh.h>
//#include <igl/cotmatrix.h>
//#include <igl/massmatrix.h>
//#include <igl/invert_diag.h>
//#include <igl/principal_curvature.h>
#include "CurvatureComputer.h"

#include <QProgressDialog>
#include <QTime>
#include <QMessageBox>
#include <QFile>
#include <QVector>

#include <math.h>

#include <gsl/gsl_spline.h>
#include <gsl/gsl_statistics_float.h>

#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

using namespace SW;
using namespace std;
using namespace Eigen;

const string ToothSegmentation::mVPropHandleCurvatureName = "vprop_curvature";
const string ToothSegmentation::mVPropHandleCurvatureComputedName = "vprop_curvature_computed";
const string ToothSegmentation::mVPropHandleIsToothBoundaryName = "vprop_is_tooth_boundary";
const string ToothSegmentation::mVPropHandleBoundaryVertexTypeName = "vprop_boundary_vertex_type";
const string ToothSegmentation::mVPropHandleNonBoundaryRegionTypeName = "vprop_non_boundary_region_type";
const string ToothSegmentation::mVPropHandleRegionGrowingVisitedName = "vprop_region_growing_visited";
const string ToothSegmentation::mVPropHandleBoundaryTypeName = "vprop_boundary_type";
const string ToothSegmentation::mVPropHandleSearchContourSectionVisitedName = "vprop_search_contour_section_visited";

ToothSegmentation::ToothSegmentation(QWidget *parentWidget, const Mesh &toothMesh)
{
    mParentWidget = parentWidget;
    mProgress = new QProgressDialog(mParentWidget);
    //mProgress->setWindowTitle("");
    mProgress->setMinimumSize(400, 120);
    mProgress->setCancelButtonText("cancel");
    mProgress->setMinimumDuration(0);
    mProgress->setWindowModality(Qt::WindowModal);
    mProgress->setAutoClose(false);

    setToothMesh(toothMesh);

    mShouldShowExtraMesh = false;
    mGingivaCuttingPlaneComputed = false;
    mProgramSchedule = SCHEDULE_START;
    mCursorType = CURSOR_DEFAULT;
    mCircleCursorRadius = 10;
}

ToothSegmentation::ToothSegmentation()
{

}

ToothSegmentation::ToothSegmentation(const ToothSegmentation &toothSegmentation)
{
    mParentWidget = toothSegmentation.mParentWidget;
    mProgress = toothSegmentation.mProgress;

    mToothMesh = toothSegmentation.mToothMesh;
    mTempToothMesh = toothSegmentation.mTempToothMesh;
    mExtraMesh = toothSegmentation.mExtraMesh;

    mProgramSchedule = toothSegmentation.mProgramSchedule;

    mVPropHandleCurvature = toothSegmentation.mVPropHandleCurvature;
    mVPropHandleCurvatureComputed = toothSegmentation.mVPropHandleCurvatureComputed;
    mVPropHandleIsToothBoundary = toothSegmentation.mVPropHandleIsToothBoundary;
    mVPropHandleBoundaryVertexType = toothSegmentation.mVPropHandleBoundaryVertexType;
    mVPropHandleNonBoundaryRegionType = toothSegmentation.mVPropHandleNonBoundaryRegionType;
    mVPropHandleRegionGrowingVisited = toothSegmentation.mVPropHandleRegionGrowingVisited;
    mVPropHandleBoundaryType = toothSegmentation.mVPropHandleBoundaryType;
    mVPropHandleSearchContourSectionVisited = toothSegmentation.mVPropHandleSearchContourSectionVisited;

    mBoundaryVertexNum = toothSegmentation.mBoundaryVertexNum;

    mGingivaCuttingPlanePoint = toothSegmentation.mGingivaCuttingPlanePoint;
    mGingivaCuttingPlaneNormal = toothSegmentation.mGingivaCuttingPlaneNormal;
    mGingivaCuttingPlaneComputed = toothSegmentation.mGingivaCuttingPlaneComputed;

    mToothNum = toothSegmentation.mToothNum;

    mCuttingPointHandles = toothSegmentation.mCuttingPointHandles;
    mJointPointHandles = toothSegmentation.mJointPointHandles;
    mContourSections = toothSegmentation.mContourSections;

    mToothMeshVertices = toothSegmentation.mToothMeshVertices;
    mToothMeshVertexHandles = toothSegmentation.mToothMeshVertexHandles;

    mErrorRegionVertexHandles = toothSegmentation.mErrorRegionVertexHandles;

    mCursorType = toothSegmentation.mCursorType;
    mCircleCursorRadius = toothSegmentation.mCircleCursorRadius;

    mMouseTrack = toothSegmentation.mMouseTrack;
}

ToothSegmentation& ToothSegmentation::operator=(const ToothSegmentation &toothSegmentation)
{
    *this = ToothSegmentation(toothSegmentation);
    return *this;
}

void ToothSegmentation::copyFrom(const ToothSegmentation &toothSegmentation)
{
    mToothMesh = toothSegmentation.mToothMesh;
    mTempToothMesh = toothSegmentation.mTempToothMesh;
    mExtraMesh = toothSegmentation.mExtraMesh;
    mShouldShowExtraMesh = toothSegmentation.mShouldShowExtraMesh;

    mProgramSchedule = toothSegmentation.mProgramSchedule;

    mBoundaryVertexNum = toothSegmentation.mBoundaryVertexNum;

    mGingivaCuttingPlanePoint = toothSegmentation.mGingivaCuttingPlanePoint;
    mGingivaCuttingPlaneNormal = toothSegmentation.mGingivaCuttingPlaneNormal;
    mGingivaCuttingPlaneComputed = toothSegmentation.mGingivaCuttingPlaneComputed;

    mToothNum = toothSegmentation.mToothNum;

    mCuttingPointHandles = toothSegmentation.mCuttingPointHandles;
    mJointPointHandles = toothSegmentation.mJointPointHandles;
    mContourSections = toothSegmentation.mContourSections;

    mToothMeshVertices = toothSegmentation.mToothMeshVertices;
    mToothMeshVertexHandles = toothSegmentation.mToothMeshVertexHandles;

    mErrorRegionVertexHandles = toothSegmentation.mErrorRegionVertexHandles;

    mCursorType = toothSegmentation.mCursorType;
    mCircleCursorRadius = toothSegmentation.mCircleCursorRadius;

    mMouseTrack = toothSegmentation.mMouseTrack;
}

void ToothSegmentation::setToothMesh(const Mesh &toothMesh)
{
    mToothMesh = toothMesh;

    //在Mesh添加自定义属性
    if(!mToothMesh.get_property_handle(mVPropHandleCurvature, mVPropHandleCurvatureName))
    {
        mToothMesh.add_property(mVPropHandleCurvature, mVPropHandleCurvatureName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleCurvatureComputed, mVPropHandleCurvatureComputedName))
    {
        mToothMesh.add_property(mVPropHandleCurvatureComputed, mVPropHandleCurvatureComputedName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleIsToothBoundary, mVPropHandleIsToothBoundaryName))
    {
        mToothMesh.add_property(mVPropHandleIsToothBoundary, mVPropHandleIsToothBoundaryName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleBoundaryVertexType, mVPropHandleBoundaryVertexTypeName))
    {
        mToothMesh.add_property(mVPropHandleBoundaryVertexType, mVPropHandleBoundaryVertexTypeName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleNonBoundaryRegionType, mVPropHandleNonBoundaryRegionTypeName))
    {
        mToothMesh.add_property(mVPropHandleNonBoundaryRegionType, mVPropHandleNonBoundaryRegionTypeName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleRegionGrowingVisited, mVPropHandleRegionGrowingVisitedName))
    {
        mToothMesh.add_property(mVPropHandleRegionGrowingVisited, mVPropHandleRegionGrowingVisitedName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleBoundaryType, mVPropHandleBoundaryTypeName))
    {
        mToothMesh.add_property(mVPropHandleBoundaryType, mVPropHandleBoundaryTypeName);
    }
    if(!mToothMesh.get_property_handle(mVPropHandleSearchContourSectionVisited, mVPropHandleSearchContourSectionVisitedName))
    {
        mToothMesh.add_property(mVPropHandleSearchContourSectionVisited, mVPropHandleSearchContourSectionVisitedName);
    }

    //如果Mesh中没有顶点颜色这个属性，则添加之
    if(!mToothMesh.has_vertex_colors())
    {
        mToothMesh.request_vertex_colors();
    }

    //将所有顶点涂白
    mProgress->setWindowTitle("Setup tooth mesh...");
    paintAllVerticesWhite();
    mProgress->close();

    //建立mesh顶点的线性索引
    mToothMeshVertices.clear();
    mToothMeshVertexHandles.clear();
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mToothMeshVertices.push_back(mToothMesh.point(*vertexIter));
        mToothMeshVertexHandles.push_back(*vertexIter);
    }
}

Mesh ToothSegmentation::getToothMesh() const
{
    return mToothMesh;
}

Mesh ToothSegmentation::getExtraMesh() const
{
    return mExtraMesh;
}

void ToothSegmentation::identifyPotentialToothBoundary(bool loadStateFromFile)
{
    mProgress->setWindowTitle("Identify potential tooth boundary...");

    //如果存在之前保存的状态，则读取之
    if(loadStateFromFile && loadState("IdentifyPotentialToothBoundary"))
    {
        return;
    }

    identifyPotentialToothBoundary();

    paintAllVerticesWhite();
    paintBoundaryVertices();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".IdentifyPotentialToothBoundary.off");

    //测试，将曲率值转换成伪彩色显示在模型上
    //curvature2PseudoColor();
    //saveToothMesh(mToothMesh.MeshName.toStdString() + ".IdentifyPotentialToothBoundary.ShowCurvatureByPseudoColor.off");

    //保存当前状态（节省调试时间）
    if(loadStateFromFile)
    {
        saveState("IdentifyPotentialToothBoundary");
    }

    //关闭进度条
    mProgress->close();

    mShouldShowExtraMesh = false;
    mProgramSchedule = SCHEDULE_IdentifyPotentialToothBoundary_FINISHED;
}

void ToothSegmentation::identifyPotentialToothBoundary()
{
    //计算顶点处曲率
    computeCurvature();

    //测试，计算曲率直方图，去掉两侧奇异点
    computeCurvatureHistogram();

    //根据曲率阈值判断初始边界点
    float curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    //测试，输出曲率最大最小值
    cout << "曲率最小值：" << curvatureMin << "，曲率最大值：" << curvatureMax << endl;
    float curvatureThreshold = curvatureMin * 0.02; //TODO 经肉眼观察，对于模型36293X_Zhenkan_070404.obj，0.01这个值最合适。
    mBoundaryVertexNum = 0;
    int vertexIndex = 0;
    mProgress->setLabelText("Finding boundary by curvature...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter)) //跳过未被正确计算出曲率的顶点
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            continue;
        }
        if(mToothMesh.property(mVPropHandleCurvature, *vertexIter) < curvatureThreshold) //如果该顶点处的曲率小于某个阈值，则确定为初始边界点
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = true;
            mBoundaryVertexNum++;
        }
        else
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
        }
        vertexIndex++;
    }

    /*//根据邻域曲率变化判断初始边界
    float curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    float allCurvatureRange = curvatureMax - curvatureMin;
    float ringCurvatureMin, ringCurvatureMax;
    float tempCurvature;
    mBoundaryVertexNum = 0;
    int vertexIndex = 0;
    QVector<Mesh::VertexHandle> ringVertexHandles;
    int k = 3;//ceil((float)mToothMesh.mVertexNum / 50000);
    int neighborNumMax = k * k * 20;
    float *ringCurvatures = new float[neighborNumMax]; //预分配足够的内存
    float ringCurvaturesVariance;
    mProgress->setLabelText("Finding boundary by curvature...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);

        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter)) //跳过未被正确计算出曲率的顶点
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            continue;
        }

        //计算邻域中各点曲率变化
        ringVertexHandles.clear();
        getKRing(*vertexIter, k, ringVertexHandles);
        assert(ringVertexHandles.size() <= neighborNumMax);
        for(int i = 0; i < ringVertexHandles.size(); i++)
        {
            ringCurvatures[i] = mToothMesh.property(mVPropHandleCurvature, ringVertexHandles.at(i));
        }
        ringCurvaturesVariance = gsl_stats_float_variance(ringCurvatures, 1, ringVertexHandles.size());
        if(ringCurvaturesVariance > 0.02 && ringCurvatures[0] < 0) //TODO 这个阈值是臆想的
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = true;
            mBoundaryVertexNum++;
        }
        else
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
        }

        vertexIndex++;
    }
    delete[] ringCurvatures;*/

    //连接断开处
    //connectBoundary(3);

    //测试，显示边界点数目
    //QMessageBox::information(mParentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(mBoundaryVertexNum).arg(mToothMesh.mVertexNum));

    //形态学操作
    dilateBoundary();
    //dilateBoundary();
    //dilateBoundary();
    //corrodeBoundary();
    //corrodeBoundary();

    //测试，显示形态学操作后边界点数目
    //QMessageBox::information(mParentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(mBoundaryVertexNum).arg(mToothMesh.mVertexNum));
}

void ToothSegmentation::computeCurvature()
{
    QTime time;
    time.start();

    QVector<float> curvature(mToothMesh.mVertexNum); //平均曲率
    QVector<bool> curvatureComputed(mToothMesh.mVertexNum); //记录每个顶点是否被正确计算得到曲率

    //计算平均曲率
    CurvatureComputer curvatureComputer(mToothMesh);
    curvatureComputer.computeCurvature(mProgress);
    curvatureComputer.getResult(curvature, curvatureComputed);

    cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "计算平均曲率" << " ended." << endl;

    //显示计算曲率出错点坐标及数量
    int curvatureComputeFailedNum = 0;
    cout << "曲率计算出错点坐标：" << endl;
    int vertexIndex = 0;
    for(vertexIndex = 0; vertexIndex < mToothMesh.mVertexNum; vertexIndex++)
    {
        if(!curvatureComputed[vertexIndex])
        {
            curvatureComputeFailedNum++;
            //printf("%.6f %.6f %.6f\n", V(vertexIndex, 0), V(vertexIndex, 1), V(vertexIndex, 2));
        }
    }
    cout << "Compute curvature finished!\n" << curvatureComputeFailedNum << "/" << mToothMesh.mVertexNum << " vertices failed." << endl;

    //将计算得到的曲率信息写入到Mesh
    vertexIndex = 0;
    mProgress->setLabelText("Adding curvature to mesh...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter) = curvatureComputed[vertexIndex];
        mToothMesh.property(mVPropHandleCurvature, *vertexIter) = curvature[vertexIndex]; //可通过curvature_computed判断该顶点处曲率是否已被正确计算
        vertexIndex++;
    }
    cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "将曲率信息写入到Mesh" << " ended." << endl;
}

void ToothSegmentation::curvature2PseudoColor()
{
    //计算曲率的最大值和最小值
    float curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    cout << "curvatureMin: " << curvatureMin << ", curvatureMax: " << curvatureMax << endl;

    //计算对应的伪彩色
    //curvatureMin *= 0.8; curvatureMax *= 0.8;
    Mesh::Color colorPseudoRGB;
    float colorGray;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter)) //将未被正确计算出曲率的顶点颜色设置为紫色（因为伪彩色中没有紫色）
        {
            colorPseudoRGB[0] = 1.0;
            colorPseudoRGB[1] = 0.0;
            colorPseudoRGB[2] = 1.0;
        }
        else
        {
            colorGray = (mToothMesh.property(mVPropHandleCurvature, *vertexIter) - curvatureMin) / (curvatureMax - curvatureMin);
            //colorGray = (mToothMesh.property(mVPropHandleCurvature, *vertexIter) + 8.0) / 16.0;
            gray2PseudoColor(colorGray, colorPseudoRGB);
        }
        mToothMesh.set_color(*vertexIter, colorPseudoRGB);
    }
}

void ToothSegmentation::computeCurvatureMinAndMax(float &curvatureMin, float &curvatureMax)
{
    curvatureMin = 1000000.0; //TODO 初始化最小值为某个足够大的值（因为第一个顶点不确定是否被正确计算出曲率）
    curvatureMax = -1000000.0;
    float tempCurvature;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter)) //跳过未被正确计算出曲率的顶点
        {
            continue;
        }
        tempCurvature = mToothMesh.property(mVPropHandleCurvature, *vertexIter);
        if(tempCurvature > curvatureMax)
        {
            curvatureMax = tempCurvature;
        }
        else if(tempCurvature < curvatureMin)
        {
            curvatureMin = tempCurvature;
        }
    }
}

void ToothSegmentation::corrodeBoundary()
{
    int neighborNotBoundaryVertexNum; //邻域中非边界点的个数
    int boundaryVertexIndex = 0;
    bool *boundaryVertexEliminated = new bool[mBoundaryVertexNum]; //标记对应边界点是否应被剔除
    mProgress->setLabelText("Corroding boundary...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum * 2);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点（包括未被正确计算出曲率的点，因为在上一步根据曲率阈值确定初始边界的过程中，未被正确计算出曲率的点全部被标记为非初始边界点）
        {
            continue;
        }
        mProgress->setValue(boundaryVertexIndex);
        //计算邻域中非边界点的个数
        neighborNotBoundaryVertexNum = 0;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborNotBoundaryVertexNum++;
            }
        }
        //邻域中非边界点的个数大于某值的标记剔除
        boundaryVertexEliminated[boundaryVertexIndex] = (neighborNotBoundaryVertexNum > 0);
        boundaryVertexIndex++;
    }
    boundaryVertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点（包括未被正确计算出曲率的点，因为在上一步根据曲率阈值确定初始边界的过程中，未被正确计算出曲率的点全部被标记为非初始边界点）
        {
            continue;
        }
        mProgress->setValue(mBoundaryVertexNum + boundaryVertexIndex);
        //剔除被标记为应删除的边界点
        if(boundaryVertexEliminated[boundaryVertexIndex])
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mBoundaryVertexNum--;
        }
        boundaryVertexIndex++;
    }
    delete []boundaryVertexEliminated;
}

void ToothSegmentation::dilateBoundary()
{
    int neighborBoundaryVertexNum; //邻域中边界点的个数
    int notBoundaryVertexIndex = 0;
    bool *boundaryVertexAdded = new bool[mToothMesh.mVertexNum - mBoundaryVertexNum]; //标记对应非边界点是否应被添加为边界点
    mProgress->setLabelText("Dilating boundary...");
    mProgress->setMinimum(0);
    mProgress->setMaximum((mToothMesh.mVertexNum - mBoundaryVertexNum) * 2);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过初始边界点
        {
            continue;
        }
        mProgress->setValue(notBoundaryVertexIndex);
        //计算邻域中边界点的个数
        neighborBoundaryVertexNum = 0;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborBoundaryVertexNum++;
            }
        }
        //邻域中边界点的个数大于某值的标记添加
        boundaryVertexAdded[notBoundaryVertexIndex] = (neighborBoundaryVertexNum > 0);
        notBoundaryVertexIndex++;
    }
    notBoundaryVertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过初始边界点
        {
            continue;
        }
        mProgress->setValue((mToothMesh.mVertexNum - mBoundaryVertexNum) + notBoundaryVertexIndex);
        //添加被标记为应添加的非边界点
        if(boundaryVertexAdded[notBoundaryVertexIndex])
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = true;
            mBoundaryVertexNum++;
        }
        notBoundaryVertexIndex++;
    }
    delete []boundaryVertexAdded;
}

void ToothSegmentation::paintBoundaryVertices()
{
    int vertexIndex = 0;
    Mesh::Color colorRed(1.0, 0.0, 0.0), colorWhite(1.0, 1.0, 1.0);
    mProgress->setLabelText("Painting boundary vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            mToothMesh.set_color(*vertexIter, colorRed);
        }
        else
        {
            //mToothMesh.set_color(*vertexIter, colorWhite);
        }
        vertexIndex++;
    }
}

void ToothSegmentation::automaticCuttingOfGingiva(bool loadStateFromFile, bool flipCuttingPlane, float moveCuttingPlaneDistance)
{
    mProgress->setWindowTitle("Automatic cutting Of gingiva...");

    //如果存在之前保存的状态，则读取之
    if(loadStateFromFile && loadState("AutomaticCuttingOfGingiva"))
    {
        return;
    }

    if(!mGingivaCuttingPlaneComputed)
    {
        mTempToothMesh = mToothMesh;
        automaticCuttingOfGingiva();
        mGingivaCuttingPlaneComputed = true;
    }
    else
    {
        mToothMesh = mTempToothMesh;
    }

    //翻转牙龈分割平面
    if(flipCuttingPlane)
    {
        mGingivaCuttingPlaneNormal = -mGingivaCuttingPlaneNormal;
    }

    //手动调整（沿法向量方向平移）牙龈分割平面
    double boundingBoxMinEdgeLength = mToothMesh.BBox.size.x; //BoundingBox的最小边长
    if(mToothMesh.BBox.size.y < boundingBoxMinEdgeLength)
    {
        boundingBoxMinEdgeLength = mToothMesh.BBox.size.y;
    }
    if(mToothMesh.BBox.size.z < boundingBoxMinEdgeLength)
    {
        boundingBoxMinEdgeLength = mToothMesh.BBox.size.z;
    }
    mGingivaCuttingPlanePoint += mGingivaCuttingPlaneNormal * boundingBoxMinEdgeLength * moveCuttingPlaneDistance;

    //剔除牙龈上的初始边界点
    removeBoundaryVertexOnGingiva();

    //标记非边界区域
    int gingivaRegionNum = markNonBoundaryRegion();

    //如果牙龈区域个数过多，则可能是牙龈分割平面方向不正确，自动进行平面翻转
    static bool planeAutoFliped = false;
    if(gingivaRegionNum > 5 && !planeAutoFliped) //TODO 此阈值是臆想的
    {
        planeAutoFliped = true;
        automaticCuttingOfGingiva(false, true, moveCuttingPlaneDistance * 2);
        return;
    }

    paintClassifiedNonBoundaryRegions();
    paintBoundaryVertices();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".AutomaticCuttingOfGingiva.off");

    //创建牙龈分割平面mesh
    mToothMesh.computeBoundingBox();
    double boundingBoxMaxEdgeLength = mToothMesh.BBox.size.x; //BoundingBox的最大边长
    if(mToothMesh.BBox.size.y > boundingBoxMaxEdgeLength)
    {
        boundingBoxMaxEdgeLength = mToothMesh.BBox.size.y;
    }
    if(mToothMesh.BBox.size.z > boundingBoxMaxEdgeLength)
    {
        boundingBoxMaxEdgeLength = mToothMesh.BBox.size.z;
    }
    float gingivaCuttingPlaneSize = boundingBoxMaxEdgeLength * 1.5; //TODO 1.5的意义是使画出来的分割平面比模型稍大一点
    createPlaneInExtraMesh(mGingivaCuttingPlanePoint, mGingivaCuttingPlaneNormal, gingivaCuttingPlaneSize);
    saveExtraMesh(mToothMesh.MeshName.toStdString() + ".AutomaticCuttingOfGingiva.Extra.GingivaCuttingPlane.off");

    //保存当前状态
    if(loadStateFromFile)
    {
        saveState("AutomaticCuttingOfGingiva");
    }

    //关闭进度条
    mProgress->close();

    mShouldShowExtraMesh = true;
    mProgramSchedule = SCHEDULE_AutomaticCuttingOfGingiva_FINISHED;
}

void ToothSegmentation::automaticCuttingOfGingiva()
{
    //计算初始边界点质心
    mGingivaCuttingPlanePoint = Mesh::Point(0.0, 0.0, 0.0); //质心点
    float tempCurvature, curvatureSum = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        tempCurvature = mToothMesh.property(mVPropHandleCurvature, *vertexIter);
        mGingivaCuttingPlanePoint += mToothMesh.point(*vertexIter) * tempCurvature; //将该点曲率作为加权
        curvatureSum += tempCurvature;
    }
    mGingivaCuttingPlanePoint /= curvatureSum;

    //测试，输出质心点
    cout << "质心点：\n" << mGingivaCuttingPlanePoint << endl;

    //计算协方差矩阵
    Matrix3f covarMat;
//    covarMat << 0, 0, 0,
//            0, 0, 0,
//            0, 0, 0; //还可以用covarMat=MatrixXf::Zero(3, 3);或covarMat.setZero();实现初始化为0
    covarMat.setZero(3, 3);
    Matrix3f tempMat;
    Mesh::Point tempVertex;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        tempVertex = mToothMesh.point(*vertexIter);
        tempVertex -= mGingivaCuttingPlanePoint;
        tempMat << tempVertex[0] * tempVertex[0], tempVertex[0] * tempVertex[1], tempVertex[0] * tempVertex[2],
                tempVertex[1] * tempVertex[0], tempVertex[1] * tempVertex[1], tempVertex[1] * tempVertex[2],
                tempVertex[2] * tempVertex[0], tempVertex[2] * tempVertex[1], tempVertex[2] * tempVertex[2];
        covarMat += tempMat;
    }
    covarMat /= mBoundaryVertexNum;

    //测试，输出协方差矩阵
    cout << "协方差矩阵：\n" << covarMat << endl;

    //计算分割平面法向量（即协方差矩阵对应最小特征值的特征向量）
    EigenSolver<Matrix3f> eigenSolver(covarMat, true);
    EigenSolver<Matrix3f>::EigenvalueType eigenvalues = eigenSolver.eigenvalues();
    EigenSolver<Matrix3f>::EigenvectorsType eigenvectors = eigenSolver.eigenvectors();
    float eigenvalueMin = eigenvalues(0).real();
    int eigenvalueMinIndex = 0;
    for(int i = 1; i < 3; i++)
    {
        if(eigenvalues(i).real() < eigenvalueMin)
        {
            eigenvalueMin = eigenvalues(i).real();
            eigenvalueMinIndex = i;
        }
    }
    //测试，输出所有特征值和特征向量
    cout << "所有特征值：\n" << eigenvalues << endl;
    cout << "所有特征向量：\n" << eigenvectors << endl;
    mGingivaCuttingPlaneNormal = Mesh::Normal(eigenvectors(0, eigenvalueMinIndex).real(), eigenvectors(1, eigenvalueMinIndex).real(), eigenvectors(2, eigenvalueMinIndex).real()); //eigenvectors的每一列为一个特征向量
    //mGingivaCuttingPlaneNormal = Mesh::Normal(0.0, 1.0, 0.0); //假设的值
}

void ToothSegmentation::boundarySkeletonExtraction(bool loadStateFromFile)
{
    mProgress->setWindowTitle("Boundary skeleton extraction...");

    //如果存在之前保存的状态，则读取之
    if(loadStateFromFile && loadState("BoundarySkeletonExtraction"))
    {
        return;
    }

    boundarySkeletonExtraction();

    //测试，将迭代后剩下的非单点宽度边界点（由于算法bug导致）保存到文件
    /*mExtraMesh = Mesh();
    if(!mExtraMesh.has_vertex_colors())
    {
        mExtraMesh.request_vertex_colors();
    }
    Mesh::VertexHandle tempVertexHandle;
    Mesh::Color colorBlue(0.0, 0.0, 1.0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            continue;
        }
        if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) == COMPLEX_VERTEX)
        {
            continue;
        }
        tempVertexHandle = mExtraMesh.add_vertex(mToothMesh.point(*vertexIter));
        mExtraMesh.set_color(tempVertexHandle, colorBlue);
    }
    saveExtraMesh(mToothMesh.MeshName.toStdString() + ".BoundarySkeletonExtraction.Extra.ErrorVertices.off");*/

    paintClassifiedBoundaryVertices();
    paintClassifiedNonBoundaryRegions();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".BoundarySkeletonExtraction.off");

    //保存当前状态
    if(loadStateFromFile)
    {
        saveState("BoundarySkeletonExtraction");
    }

    //关闭进度条
    mProgress->close();

    mShouldShowExtraMesh = false;
    mProgramSchedule = SCHEDULE_BoundarySkeletonExtraction_FINISHED;
}

void ToothSegmentation::boundarySkeletonExtraction()
{
    mErrorRegionVertexHandles.clear();

    //逐步删除某一类外围点
    int *classifiedBoundaryVertexNum = new int[mToothNum + DISK_VERTEX_TOOTH];
    int deleteIterTimes = 0; //迭代次数
    bool deleteIterationFinished = false;

    //边界点分类
    classifyBoundaryVertex(classifiedBoundaryVertexNum);

    //测试，保存中间结果
//    paintAllVerticesWhite();
//    paintClassifiedBoundaryVertices();
//    saveToothMesh(mToothMesh.MeshName.toStdString() + ".BoundarySkeletonExtraction.temp.off");

    int diskVertexTypeIndex, diskVertexTypeIndex2;
    int startCenterAndDiskVertexNum = 0; //迭代前内部点和外围点总数
    startCenterAndDiskVertexNum += classifiedBoundaryVertexNum[CENTER_VERTEX];
    for(diskVertexTypeIndex = 0; diskVertexTypeIndex < mToothNum + 1; diskVertexTypeIndex++)
    {
        startCenterAndDiskVertexNum += classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA + diskVertexTypeIndex];
    }
    int centerAndDiskVertexNum;
    int centerVertexNum;
    int diskVertexNum;

    mProgress->setLabelText("Deleting disk vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(startCenterAndDiskVertexNum);
    mProgress->setValue(0);
    while(true)
    {
        for(diskVertexTypeIndex = 0; diskVertexTypeIndex < mToothNum + 1; diskVertexTypeIndex++)
        {
            //如果存在此类外围点，则将所有此类外围点删除，然后重新分类
            if(classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA + diskVertexTypeIndex] != 0)
            {
                for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
                {
                    if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
                    {
                        continue;
                    }
                    if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) == (DISK_VERTEX_GINGIVA + diskVertexTypeIndex))
                    {
                        mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
                        mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = (GINGIVA_REGION + diskVertexTypeIndex);
                        mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = true;
                        mBoundaryVertexNum--;
                    }
                }

                classifyBoundaryVertex(classifiedBoundaryVertexNum);

                //测试，保存中间结果
//                paintAllVerticesWhite();
//                paintClassifiedBoundaryVertices();
//                stringstream ss;
//                ss << deleteIterTimes;
//                string s = ss.str();
//                saveToothMesh(mToothMesh.MeshName.toStdString() + ".BoundarySkeletonExtraction.temp" + s + ".off");
//                QMessageBox::information(mParentWidget, "Info", QString("Temp mesh saved! %1").arg(deleteIterTimes));

                //计算内部点和外围点数
                centerVertexNum = classifiedBoundaryVertexNum[CENTER_VERTEX];
                diskVertexNum = 0;
                for(diskVertexTypeIndex2 = 0; diskVertexTypeIndex2 < mToothNum + 1; diskVertexTypeIndex2++)
                {
                    diskVertexNum += classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA + diskVertexTypeIndex2];
                }
                centerAndDiskVertexNum = centerVertexNum + diskVertexNum;

                deleteIterTimes++;
                mProgress->setLabelText(QString("Deleting disk vertices...\nNo.%1 iteration.\n%2 center vertices left;\n%3 disk vertices left.").arg(deleteIterTimes).arg(centerVertexNum).arg(diskVertexNum));
                mProgress->setValue(startCenterAndDiskVertexNum - centerAndDiskVertexNum);

                //判断迭代结束条件
                if(centerAndDiskVertexNum == 0)
                {
                    QMessageBox::information(mParentWidget, "Info", QString("Deleting disk vertices ended!\nTotal %1 iterations.\n%2 center vertices left;\n%3 disk vertices left.").arg(deleteIterTimes).arg(centerVertexNum).arg(diskVertexNum));
                    deleteIterationFinished = true;
                    break;
                }
                else if(diskVertexNum == 0) //如果disk vertex已迭代删除完毕，但还残留center vertex，那么将剩下的center vertex设置为非边界点
                {
                    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
                    {
                        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
                        {
                            continue;
                        }
                        if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) != CENTER_VERTEX) //跳过非内部点
                        {
                            continue;
                        }
                        mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
                        mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = ERROR_REGION; //TODO 因为其邻域点均为complex vertex，所以无法判断该点属于哪个非边界区域，若将该点设置为GINGIVA_REGION会影响cutting point的判断，因此暂将该点设置为ERROR_REGION
                        mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = true;
                        mErrorRegionVertexHandles.push_back(*vertexIter);
                        cout << "残余center point：" << mToothMesh.point(*vertexIter) << endl;
                        mBoundaryVertexNum--;
                    }
                    QMessageBox::information(mParentWidget, "Info", QString("Deleting disk vertices ended!\nTotal %1 iterations.\n%2 center vertices left;\n%3 disk vertices left.\nAll center vertex left have been changed to nonboundary.").arg(deleteIterTimes).arg(centerVertexNum).arg(diskVertexNum));
                    deleteIterationFinished = true;
                    break;
                }
            }
        }
        if(deleteIterationFinished)
        {
            break;
        }
    }
}

void ToothSegmentation::classifyBoundaryVertex(int *classifiedBoundaryVertexNum)
{
    for(int i = 0; i < mToothNum + DISK_VERTEX_TOOTH; i++)
    {
        classifiedBoundaryVertexNum[i] = 0;
    }

    int neighborVertexTypeChangeTimes; //某边界点邻域点是否属于边界点这个属性改变（从边界点到非边界点或从非边界点到边界点）的次数
    int neighborBoundaryVertexNum; //某边界点邻域中边界点数量
    Mesh::VertexVertexIter tempVvIterBegin; //由于在遍历邻域顶点时需要使用2个迭代器，因此保存初始邻域点
    int boundaryVertexIndex = 0;
//    mProgress->setLabelText("Classifying boundary vertices...");
//    mProgress->setMinimum(0);
//    mProgress->setMaximum(mBoundaryVertexNum);
//    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
//        mProgress->setValue(boundaryVertexIndex);

        if(mToothMesh.is_boundary(*vertexIter)) //跳过模型边界点（后面单独处理）
        {
            //mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_GINGIVA;
            //classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA]++;
            boundaryVertexIndex++;
            continue;
        }

        neighborVertexTypeChangeTimes = 0;
        neighborBoundaryVertexNum = 0;
        tempVvIterBegin = mToothMesh.vv_iter(*vertexIter);
        for(Mesh::VertexVertexIter vertexVertexIter = tempVvIterBegin; vertexVertexIter.is_valid(); )
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter) != mToothMesh.property(mVPropHandleIsToothBoundary, *((++vertexVertexIter).is_valid() ? vertexVertexIter : tempVvIterBegin)))
            {
                neighborVertexTypeChangeTimes++;
            }
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborBoundaryVertexNum++;
            }
        }

        //如果为孤立点，则任意判断其为DISK_VERTEX_GINGIVA或DISK_VERTEX_TOOTH（总之要被剔除）（注意：不能直接将其设置为非边界点，因为在删除边界点之后要更新其所属的非边界区域）
        if(neighborBoundaryVertexNum == 0)
        {
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_GINGIVA;
            classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA]++;
            boundaryVertexIndex++;
            continue;
        }

        switch(neighborVertexTypeChangeTimes)
        {
        case 0:
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = CENTER_VERTEX;
            classifiedBoundaryVertexNum[CENTER_VERTEX]++;
            break;
        case 2:
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_GINGIVA;
            //classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA]++;
            break;
        case 4:
        default:
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = COMPLEX_VERTEX;
            classifiedBoundaryVertexNum[COMPLEX_VERTEX]++;
            break;
        }
        boundaryVertexIndex++;
    }

    //将外围点分类
    boundaryVertexIndex = 0;
    int regionType;
    int vertexType;
//    mProgress->setLabelText("Classifying boundary vertices(Disk vertices)...");
//    mProgress->setMinimum(0);
//    mProgress->setMaximum(mBoundaryVertexNum);
//    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        if(mToothMesh.is_boundary(*vertexIter)) //跳过模型边界点（后面单独处理）
        {
            continue;
        }
//        mProgress->setValue(boundaryVertexIndex);
        if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) != DISK_VERTEX_GINGIVA) //跳过非外围点
        {
            boundaryVertexIndex++;
            continue;
        }
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过初始边界点
            {
                continue;
            }
            if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) == GINGIVA_REGION)
            {
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_GINGIVA;
                classifiedBoundaryVertexNum[DISK_VERTEX_GINGIVA]++;
                break;
            }
            else
            {
                regionType = mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter);
                vertexType = regionType - TOOTH_REGION + DISK_VERTEX_TOOTH;
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = vertexType;
                classifiedBoundaryVertexNum[vertexType]++;
                break;
            }
        }
        boundaryVertexIndex++;
    }

    //模型边界点处理（模型存在空洞时需要此步骤）
    bool neighborHasNonBoundaryVertex; //某点邻域中是否存在非边界点
    bool neighborHasDiskVertex; //某点邻域中是否存在disk vertex
    bool neighborHasComplexVertexNotOnMeshBoundary; //某点邻域中是否存在不位于模型边界的complex vertex
    bool neighborHasComplexVertexOnMeshBoundary; //某点2邻域中是否存在位于模型边界的complex vertex，TODO 不知道2够不够
    QVector<Mesh::VertexHandle> neighbor2RingVertexHandles;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.is_boundary(*vertexIter)) //跳过非模型边界点
        {
            continue;
        }
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }

        neighborHasNonBoundaryVertex = false;
        neighborHasDiskVertex = false;
        neighborHasComplexVertexNotOnMeshBoundary = false;
        neighborHasComplexVertexOnMeshBoundary = false;
        neighbor2RingVertexHandles.clear();
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborHasNonBoundaryVertex = true;
            }
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)
                    && mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexVertexIter) >= DISK_VERTEX_GINGIVA)
            {
                neighborHasDiskVertex = true;
            }
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)
                    && !mToothMesh.is_boundary(*vertexVertexIter)
                    && mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexVertexIter) == COMPLEX_VERTEX)
            {
                neighborHasComplexVertexNotOnMeshBoundary = true;
            }
//            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)
//                    && mToothMesh.is_boundary(*vertexVertexIter)
//                    && mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexVertexIter) == COMPLEX_VERTEX)
//            {
//                neighborHasComplexVertexOnMeshBoundary = true;
//            }
        }
        getKRing(*vertexIter, 2, neighbor2RingVertexHandles);
        neighbor2RingVertexHandles.pop_front();
        for(int i = 0; i < neighbor2RingVertexHandles.size(); i++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, neighbor2RingVertexHandles.at(i))
                    && mToothMesh.is_boundary(neighbor2RingVertexHandles.at(i))
                    && mToothMesh.property(mVPropHandleBoundaryVertexType, neighbor2RingVertexHandles.at(i)) == COMPLEX_VERTEX)
            {
                neighborHasComplexVertexOnMeshBoundary = true;
            }
        }

        if(neighborHasComplexVertexNotOnMeshBoundary && !neighborHasComplexVertexOnMeshBoundary)
        {
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = COMPLEX_VERTEX;
            classifiedBoundaryVertexNum[COMPLEX_VERTEX]++;
        }
        else if(!neighborHasNonBoundaryVertex)
        {
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = CENTER_VERTEX;
            classifiedBoundaryVertexNum[CENTER_VERTEX]++;
        }
        else if(neighborHasDiskVertex) //如果邻域中存在disk vertex，则将该点的BoundaryVertexType设置成与该disk vertex相同
        {
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
                {
                    continue;
                }
                if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexVertexIter) < DISK_VERTEX_GINGIVA) //跳过非disk vertex点
                {
                    continue;
                }
                vertexType = mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexVertexIter);
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = vertexType;
                classifiedBoundaryVertexNum[vertexType]++;
                break;
            }
        }
        else //如果以上条件都不满足，则根据该点相邻的非边界区域设置其BoundaryVertexType
        {
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
                {
                    continue;
                }
                regionType = mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter);
                vertexType = regionType - TOOTH_REGION + DISK_VERTEX_TOOTH;
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = vertexType;
                classifiedBoundaryVertexNum[vertexType]++;
                break;
            }
        }
    }
}

void ToothSegmentation::paintClassifiedBoundaryVertices()
{
    int vertexIndex = 0;
    Mesh::Color colorWhite(1.0, 1.0, 1.0), colorGreen(0.0, 1.0, 0.0), colorKelly(0.5, 1.0, 0.0), colorOrange(1.0, 0.5, 0.0), colorRed(1.0, 0.0, 0.0);
    mProgress->setLabelText("Painting classified boundary vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            //mToothMesh.set_color(*vertexIter, colorWhite);
        }
        else
        {
            switch(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter))
            {
            case CENTER_VERTEX:
                mToothMesh.set_color(*vertexIter, colorGreen);
                break;
            case COMPLEX_VERTEX:
                mToothMesh.set_color(*vertexIter, colorRed);
                break;
            case DISK_VERTEX_GINGIVA:
                mToothMesh.set_color(*vertexIter, colorKelly);
                break;
            case DISK_VERTEX_TOOTH:
            default:
                mToothMesh.set_color(*vertexIter, colorOrange);
                break;
            }
        }
        vertexIndex++;
    }
}

void ToothSegmentation::removeBoundaryVertexOnGingiva()
{
    float x0, y0, z0; //牙龈分割平面中心点
    x0 = mGingivaCuttingPlanePoint[0];
    y0 = mGingivaCuttingPlanePoint[1];
    z0 = mGingivaCuttingPlanePoint[2];
    float x1, y1, z1; //牙龈分割平面法向量
    x1 = mGingivaCuttingPlaneNormal[0];
    y1 = mGingivaCuttingPlaneNormal[1];
    z1 = mGingivaCuttingPlaneNormal[2];

    int boundaryVertexIndex = 0;
    Mesh::Point tempBoundaryVertex;
    mProgress->setLabelText("Removing boundary vertices on gingiva...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        mProgress->setValue(boundaryVertexIndex);
        tempBoundaryVertex = mToothMesh.point(*vertexIter);
        //如果该初始边界点位于牙龈分割平面的上方（牙龈方向），则剔除此边界点
        if(x1 * (tempBoundaryVertex[0] - x0) + y1 * (tempBoundaryVertex[1] - y0) + z1 * (tempBoundaryVertex[2] - z0) < 0)
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mBoundaryVertexNum--;
        }
        boundaryVertexIndex++;
    }
}

int ToothSegmentation::markNonBoundaryRegion()
{
    //初始化所有非边界点的NonBoundaryRegionType属性为TOOTH_REGION，RegionGrowingVisited属性为false
    int vertexIndex = 0;
    mProgress->setLabelText("Init marking region...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = TOOTH_REGION;
        mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = false;
        vertexIndex++;
    }

    //如果之前已经执行过单点宽度边界提取，并且存在属于ERROR_REGION的点，则保留这些点的ERROR_REGION属性
    if(!mErrorRegionVertexHandles.empty())
    {
        for(int i = 0; i < mErrorRegionVertexHandles.size(); i++)
        {
            mToothMesh.property(mVPropHandleNonBoundaryRegionType, mErrorRegionVertexHandles.at(i)) = ERROR_REGION;
        }
    }

    //标记牙龈区域
    Mesh::Point tempVertex;
    Mesh::VertexIter vertexIter;
    int gingivaRegionNum = 0; //记录生长得到的牙龈区域个数，据此判断是否需要对牙龈分割平面进行翻转
    float x0, y0, z0; //牙龈分割平面中心点
    x0 = mGingivaCuttingPlanePoint[0];
    y0 = mGingivaCuttingPlanePoint[1];
    z0 = mGingivaCuttingPlanePoint[2];
    float x1, y1, z1; //牙龈分割平面法向量
    x1 = mGingivaCuttingPlaneNormal[0];
    y1 = mGingivaCuttingPlaneNormal[1];
    z1 = mGingivaCuttingPlaneNormal[2];
    for(vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            continue;
        }
        tempVertex = mToothMesh.point(*vertexIter);
        if(x1 * (tempVertex[0] - x0) + y1 * (tempVertex[1] - y0) + z1 * (tempVertex[2] - z0) < 0
                && mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) != GINGIVA_REGION)
        {
            regionGrowing(*vertexIter, GINGIVA_REGION);
            gingivaRegionNum++;
        }
    }

    //去除噪声区域+分别标记牙齿区域
    mToothNum = 0; //牙齿标号（数量）
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)
                || mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter)
                || mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) == ERROR_REGION)
        {
            continue;
        }
        int regionVertexNum = regionGrowing(*vertexIter, TEMP_REGION);
        //测试，输出该区域顶点数量
        //cout << "区域顶点数量: " << regionVertexNum << endl;
        if(regionVertexNum < mToothMesh.mVertexNum * 0.001) //TODO 这个阈值是臆想的，但是达到了效果
        {
            regionGrowing(*vertexIter, FILL_BOUNDARY_REGION); //如果区域小于某个阈值，则将其填充为边界
        }
        else
        {
            regionGrowing(*vertexIter, TOOTH_REGION + mToothNum);
            mToothNum++;
        }
    }

    return gingivaRegionNum;
}

int ToothSegmentation::regionGrowing(Mesh::VertexHandle seedVertexHandle, int regionType)
{
    mToothMesh.property(mVPropHandleRegionGrowingVisited, seedVertexHandle) = true;
    if(regionType == FILL_BOUNDARY_REGION) //如果regionType为FILL_BOUNDARY_REGION，则将此区域填充为边界
    {
        mToothMesh.property(mVPropHandleIsToothBoundary, seedVertexHandle) = true;
    }
    else
    {
        mToothMesh.property(mVPropHandleNonBoundaryRegionType, seedVertexHandle) = regionType;
    }

    int regionVertexNum = 1; //该区域中顶点数量
    list<Mesh::VertexHandle> seeds; //种子点列表
    seeds.push_back(seedVertexHandle);
    while(!seeds.empty())
    {
        Mesh::VertexHandle vertexHandle = seeds.front();
        seeds.pop_front();
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(vertexHandle); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                continue;
            }
            if(mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexVertexIter))
            {
                if(regionType == TEMP_REGION)
                {
                    if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) == regionType)
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }
            mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexVertexIter) = true;
            if(regionType == FILL_BOUNDARY_REGION) //如果regionType为FILL_BOUNDARY_REGION，则将此区域填充为边界
            {
                mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter) = true;
            }
            else
            {
                mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) = regionType;
            }
            seeds.push_back(*vertexVertexIter);
            regionVertexNum++;
        }
    }

    //如果regionType为TEMP_REGION，则还原该区域所有点的visited为false
    if(regionType == TEMP_REGION)
    {
        seeds.clear();
        seeds.push_back(seedVertexHandle);
        while(!seeds.empty())
        {
            Mesh::VertexHandle vertexHandle = seeds.front();
            seeds.pop_front();
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(vertexHandle); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
                {
                    continue;
                }
                if(!mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexVertexIter))
                {
                    continue;
                }
                mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexVertexIter) = false;
                seeds.push_back(*vertexVertexIter);
            }
        }
    }

    return regionVertexNum;
}

void ToothSegmentation::paintClassifiedNonBoundaryRegions()
{
    int vertexIndex = 0;
    int regionType;
    Mesh::Color colorBlue(0.0, 0.0, 1.0), colorGreen(0.0, 1.0, 0.0), colorWhite(1.0, 1.0, 1.0);
    mProgress->setLabelText("Painting classified nonboundary regions...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            //mToothMesh.set_color(*vertexIter, colorGreen);
        }
        else
        {
            regionType = mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter);
            switch(regionType)
            {
            case ERROR_REGION:
                mToothMesh.set_color(*vertexIter, colorWhite);
                break;
            case GINGIVA_REGION:
                mToothMesh.set_color(*vertexIter, colorBlue);
                break;
            default:
                float grayValue;
                Mesh::Color pseudoColor;
                int toothIndex = regionType - TOOTH_REGION;
                grayValue = (float)(toothIndex + 1) / (mToothNum + 1); //toothIndex+1是因为灰度为0时得到的伪彩色是蓝色，和牙龈的颜色一样，所以跳过这个颜色；mToothNum+1是因为灰度为1时得到的伪彩色是红色，和边界的颜色一样，所以跳过这个颜色

                //测试
//                grayValue = 0.5;

                gray2PseudoColor(grayValue, pseudoColor);
                mToothMesh.set_color(*vertexIter, pseudoColor);
                break;
            }
        }
        vertexIndex++;
    }
}

void ToothSegmentation::saveToothMesh(string filename)
{
    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;
    options += OpenMesh::IO::Options::ColorFloat;
    if(!OpenMesh::IO::write_mesh(mToothMesh, filename, options))
    {
        cerr << "Failed to save tooth mesh to file: " + filename << endl;
    }
}

void ToothSegmentation::saveExtraMesh(string filename)
{
    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;
    options += OpenMesh::IO::Options::ColorFloat;
    if(!OpenMesh::IO::write_mesh(mExtraMesh, filename, options))
    {
        cerr << "Failed to save extra mesh to file: " + filename << endl;
    }
}

void ToothSegmentation::gray2PseudoColor(float grayValue, Mesh::Color &pseudoColor)
{
    //将grayValue规范化到0～1之间
    if(grayValue < 0.0)
    {
        grayValue = 0.0;
    }
    if(grayValue > 1.0)
    {
        grayValue = 1.0;
    }

    if(grayValue < 0.25)
    {
        pseudoColor[0] = 0.0;
        pseudoColor[1] = grayValue * 4.0;
        pseudoColor[2] = 1.0;
    }
    else if(grayValue < 0.5)
    {
        pseudoColor[0] = 0.0;
        pseudoColor[1] = 1.0;
        pseudoColor[2] = 1.0 - (grayValue - 0.25) * 4.0;
    }
    else if(grayValue < 0.75)
    {
        pseudoColor[0] = (grayValue - 0.5) * 4.0;
        pseudoColor[1] = 1.0;
        pseudoColor[2] = 0.0;
    }
    else
    {
        pseudoColor[0] = 1.0;
        pseudoColor[1] = 1.0 - (grayValue - 0.75) * 4.0;
        pseudoColor[2] = 0.0;
    }
}

void ToothSegmentation::refineToothBoundary(bool loadStateFromFile)
{
    mProgress->setWindowTitle("Refine tooth boundary...");

    refineToothBoundary();

    paintAllVerticesWhite();
    paintClassifiedBoundary();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.off");
    paintClassifiedNonBoundaryRegions();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithRegionGrowing.off");

    //关闭进度条
    mProgress->close();

    mShouldShowExtraMesh = false;
    mProgramSchedule = SCHEDULE_RefineToothBoundary_FINISHED;
}

void ToothSegmentation::refineToothBoundary()
{
    /*//分别处理每个牙齿的轮廓（改成了对由cutting point分成的每段轮廓分别进行插值）
    for(int toothIndex = 0; toothIndex < mToothNum; toothIndex++)
    {
        QVector<Mesh::VertexHandle> toothBoundaryContour; //轮廓点集
        bool found; //是否已找到一个挨着该牙齿的边界点

        //寻找该牙齿轮廓上的第1个点
        found = false;
        for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
            {
                continue;
            }
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++) //寻找邻域中有没有属于该牙齿区域的非边界点
            {
                if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
                {
                    continue;
                }
                if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) == (TOOTH_REGION + toothIndex))
                {
                    found = true;
                    break;
                }
            }
            if(found)
            {
                toothBoundaryContour.push_back(*vertexIter);

                //测试，输出找到的轮廓点坐标并涂红
                cout << "轮廓点：" << mToothMesh.point(*vertexIter) << endl;
                mToothMesh.set_color(*vertexIter, colorRed);

                break;
            }
        }

        //寻找第2个轮廓点（第1个轮廓点邻域中挨着该牙齿的边界点）
        found = false;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(toothBoundaryContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
            {
                continue;
            }
            for(Mesh::VertexVertexIter vertexVertexVertexIter = mToothMesh.vv_iter(*vertexVertexIter); vertexVertexVertexIter.is_valid(); vertexVertexVertexIter++) //寻找邻域中有没有属于该牙齿区域的非边界点
            {
                if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexVertexIter))
                {
                    continue;
                }
                if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexVertexIter) == (TOOTH_REGION + toothIndex))
                {
                    found = true;
                    break;
                }
            }
            if(found)
            {
                toothBoundaryContour.push_back(*vertexVertexIter);

                //测试，输出找到的轮廓点坐标
                cout << "轮廓点：" << mToothMesh.point(*vertexVertexIter) << endl;
                mToothMesh.set_color(*vertexVertexIter, colorRed);

                break;
            }
        }

        //寻找第3个轮廓点（第2个轮廓点邻域中除第1个轮廓点之外的挨着该牙齿的边界点）
        Mesh::Point firstContourVertex = mToothMesh.point(toothBoundaryContour.front()); //第1个轮廓点
        found = false;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(toothBoundaryContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
            {
                continue;
            }
            for(Mesh::VertexVertexIter vertexVertexVertexIter = mToothMesh.vv_iter(*vertexVertexIter); vertexVertexVertexIter.is_valid(); vertexVertexVertexIter++) //寻找邻域中有没有属于该牙齿区域的非边界点
            {
                if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexVertexIter))
                {
                    continue;
                }
                if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexVertexIter) == (TOOTH_REGION + toothIndex))
                {
                    found = true;
                    break;
                }
            }
            if(found && mToothMesh.point(*vertexVertexIter) != firstContourVertex) //找到并且不是第1个轮廓点
            {
                toothBoundaryContour.push_back(*vertexVertexIter);

                //测试，输出找到的轮廓点坐标
                cout << "轮廓点：" << mToothMesh.point(*vertexVertexIter) << endl;
                mToothMesh.set_color(*vertexVertexIter, colorRed);

                break;
            }
        }

        //深度搜索获取轮廓点集（现在已找到3个轮廓点）
        Mesh::Point previousContourVertex; //目前找到的倒数第2个轮廓点
        Mesh::Point tempContourVertex; //刚刚找到但未确定的轮廓点
        bool contourFinished = false; //该牙齿的轮廓搜索完毕
        while(true)
        {
            found = false;
            previousContourVertex = mToothMesh.point(toothBoundaryContour.at(toothBoundaryContour.size() - 2));
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(toothBoundaryContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
                {
                    continue;
                }
                for(Mesh::VertexVertexIter vertexVertexVertexIter = mToothMesh.vv_iter(*vertexVertexIter); vertexVertexVertexIter.is_valid(); vertexVertexVertexIter++) //寻找邻域中有没有属于该牙齿区域的非边界点
                {
                    if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexVertexIter))
                    {
                        continue;
                    }
                    if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexVertexIter) == (TOOTH_REGION + toothIndex))
                    {
                        found = true;
                        break;
                    }
                }
                if(found)
                {
                    tempContourVertex = mToothMesh.point(*vertexVertexIter);
                    if(tempContourVertex == previousContourVertex)
                    {
                        continue;
                    }
                    else if(tempContourVertex == firstContourVertex) //该牙齿的轮廓搜索完毕
                    {
                        contourFinished = true;
                        break;
                    }
                    else
                    {
                        toothBoundaryContour.push_back(*vertexVertexIter);

                        //测试，输出找到的轮廓点坐标
                        cout << "轮廓点：" << mToothMesh.point(*vertexVertexIter) << endl;
                        mToothMesh.set_color(*vertexVertexIter, colorRed);

                        break;
                    }
                }
            }
            if(contourFinished)
            {
                break;
            }
        }

        *//*//计算投影平面
        //计算质心
        Mesh::Point centerPoint(0.0, 0.0, 0.0);
        int contourVertexNum = toothBoundaryContour.size();
        int contourVertexIndex;
        for(contourVertexIndex = 0; contourVertexIndex < contourVertexNum; contourVertexIndex++)
        {
            centerPoint += mToothMesh.point(toothBoundaryContour.at(contourVertexIndex));
        }
        centerPoint /= contourVertexNum;

        //测试，输出质心点
        cout << "单个牙齿轮廓质心点：\n" << centerPoint << endl;

        //计算协方差矩阵
        Matrix3f covarMat;
        covarMat.setZero(3, 3);
        Matrix3f tempMat;
        Mesh::Point tempVertex;
        for(contourVertexIndex = 0; contourVertexIndex < contourVertexNum; contourVertexIndex++)
        {
            tempVertex = mToothMesh.point(toothBoundaryContour.at(contourVertexIndex));
            tempVertex -= centerPoint;
            tempMat << tempVertex[0] * tempVertex[0], tempVertex[0] * tempVertex[1], tempVertex[0] * tempVertex[2],
                    tempVertex[1] * tempVertex[0], tempVertex[1] * tempVertex[1], tempVertex[1] * tempVertex[2],
                    tempVertex[2] * tempVertex[0], tempVertex[2] * tempVertex[1], tempVertex[2] * tempVertex[2];
            covarMat += tempMat;
        }
        covarMat /= contourVertexNum;

        //测试，输出协方差矩阵
        cout << "单个牙齿轮廓协方差矩阵：\n" << covarMat << endl;

        //计算分割平面法向量（即协方差矩阵对应最小特征值的特征向量）
        EigenSolver<Matrix3f> eigenSolver(covarMat, true);
        EigenSolver<Matrix3f>::EigenvalueType eigenvalues = eigenSolver.eigenvalues();
        EigenSolver<Matrix3f>::EigenvectorsType eigenvectors = eigenSolver.eigenvectors();
        float eigenvalueMin = eigenvalues(0).real();
        int eigenvalueMinIndex = 0;
        for(int i = 1; i < 3; i++)
        {
            if(eigenvalues(i).real() < eigenvalueMin)
            {
                eigenvalueMin = eigenvalues(i).real();
                eigenvalueMinIndex = i;
            }
        }
        Mesh::Normal planeNormal = -Mesh::Normal(eigenvectors(0, eigenvalueMinIndex).real(), eigenvectors(1, eigenvalueMinIndex).real(), eigenvectors(2, eigenvalueMinIndex).real());

        //测试，输出法向量
        cout << "单个牙齿轮廓法向量：\n" << planeNormal << endl;

        //创建投影平面
        float contourBoundingBoxMaxLength = 0;
        tempVertex = mToothMesh.point(toothBoundaryContour.front());
        float minX = tempVertex[0], maxX = tempVertex[0];
        float minY = tempVertex[1], maxY = tempVertex[1];
        float minZ = tempVertex[2], maxZ = tempVertex[2];
        for(contourVertexIndex = 0; contourVertexIndex < contourVertexNum; contourVertexIndex++)
        {
            tempVertex = mToothMesh.point(toothBoundaryContour.at(contourVertexIndex));
            if(tempVertex[0] < minX)
            {
                minX = tempVertex[0];
            }
            else if(tempVertex[0] > maxX)
            {
                maxX = tempVertex[0];
            }
            if(tempVertex[1] < minY)
            {
                minY = tempVertex[1];
            }
            else if(tempVertex[1] > maxY)
            {
                maxY = tempVertex[1];
            }
            if(tempVertex[2] < minZ)
            {
                minZ = tempVertex[2];
            }
            else if(tempVertex[2] > maxZ)
            {
                maxZ = tempVertex[2];
            }
        }
        contourBoundingBoxMaxLength = maxX - minX;
        if(maxY - minY > contourBoundingBoxMaxLength)
        {
            contourBoundingBoxMaxLength = maxY - minY;
        }
        if(maxZ - minZ > contourBoundingBoxMaxLength)
        {
            contourBoundingBoxMaxLength = maxZ - minZ;
        }
        float ContourProjectPlaneSize = contourBoundingBoxMaxLength * 5;
        createPlaneInExtraMesh(centerPoint, planeNormal, ContourProjectPlaneSize);

        //测试，保存该牙齿的轮廓和投影平面mesh
        stringstream ss;
        ss << toothIndex;
        string toothIndexString = ss.str();
        saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Tooth" + toothIndexString + "Contour.off");
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.Tooth" + toothIndexString + "ContourProjectPlane.off");*//*

        //测试，保存显示该牙齿轮廓的牙齿模型
        stringstream ss;
        ss << toothIndex;
        string toothIndexString = ss.str();
        saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithTooth" + toothIndexString + "Contour.off");

        //测试，将轮廓点单独保存到文件
        mExtraMesh = Mesh();
        if(!mExtraMesh.has_vertex_colors())
        {
            mExtraMesh.request_vertex_colors();
        }
        Mesh::VertexHandle tempVertexHandle;
        for(int contourVertexIndex = 0; contourVertexIndex < toothBoundaryContour.size(); contourVertexIndex++)
        {
            tempVertexHandle = mExtraMesh.add_vertex(mToothMesh.point(toothBoundaryContour.at(contourVertexIndex)));
            mExtraMesh.set_color(tempVertexHandle, colorRed);
        }
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.Tooth" + toothIndexString + "Contour.off");

        //选取插值控制点
        QVector<Mesh::VertexHandle> contourControlVertices; //控制点集
        int contourVertexNum = toothBoundaryContour.size();
        for(int contourVertexIndex = 0; contourVertexIndex < contourVertexNum; contourVertexIndex++)
        {
            if(contourVertexIndex % 10 == 0) //TODO 暂时按照等距离选取控制点
            {
                contourControlVertices.push_back(toothBoundaryContour.at(contourVertexIndex));
            }
        }

        //将插值控制点从Mesh::Point格式转换为GSL库插值函数需要的格式
        int contourControlVertexNum = contourControlVertices.size();
        contourControlVertexNum += 1; //+1是为了在序列最后添加序列的第1个值，保证插值结果是首尾相连并且平滑
        double *t = new double[contourControlVertexNum];
        double *x = new double[contourControlVertexNum];
        double *y = new double[contourControlVertexNum];
        double *z = new double[contourControlVertexNum];
        Mesh::Point tempContourControlVertex;
        for(int contourControlVertexIndex = 0; contourControlVertexIndex < contourControlVertexNum - 1; contourControlVertexIndex++)
        {
            t[contourControlVertexIndex] = contourControlVertexIndex;
            tempContourControlVertex = mToothMesh.point(contourControlVertices.at(contourControlVertexIndex));
            x[contourControlVertexIndex] = tempContourControlVertex[0];
            y[contourControlVertexIndex] = tempContourControlVertex[1];
            z[contourControlVertexIndex] = tempContourControlVertex[2];
        }
        t[contourControlVertexNum - 1] = contourControlVertexNum - 1;
        tempContourControlVertex = mToothMesh.point(contourControlVertices.front());
        x[contourControlVertexNum - 1] = tempContourControlVertex[0];
        y[contourControlVertexNum - 1] = tempContourControlVertex[1];
        z[contourControlVertexNum - 1] = tempContourControlVertex[2];

        //开始插值
        gsl_interp_accel *acc = gsl_interp_accel_alloc();
        gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, contourControlVertexNum);
        int contourInterpPointNum = (contourControlVertexNum - 1) * 100; //TODO 此处插值密度有待自动化计算
        int contourInterpPointIndex;
        double *tInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tInterp[contourInterpPointIndex] = contourInterpPointIndex * 0.01;
        }
        gsl_spline_init(spline, t, x, contourControlVertexNum);
        double *xInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            xInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }
        gsl_spline_init(spline, t, y, contourControlVertexNum);
        double *yInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            yInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }
        gsl_spline_init(spline, t, z, contourControlVertexNum);
        double *zInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            zInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }

        gsl_spline_free(spline);
        gsl_interp_accel_free(acc);
        delete[] t;
        delete[] x;
        delete[] y;
        delete[] z;
        delete[] tInterp;

        //将插值得到的数据转换回Mesh::Point格式
        QVector<Mesh::Point> contourInterpPoints; //插值得到的轮廓点集
        Mesh::Point tempContourInterpVertex;
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tempContourInterpVertex[0] = xInterp[contourInterpPointIndex];
            tempContourInterpVertex[1] = yInterp[contourInterpPointIndex];
            tempContourInterpVertex[2] = zInterp[contourInterpPointIndex];
            contourInterpPoints.push_back(tempContourInterpVertex);
        }

        delete[] xInterp;
        delete[] yInterp;
        delete[] zInterp;

        //测试，将插值得到的轮廓点保存到文件
        mExtraMesh = Mesh();
        if(!mExtraMesh.has_vertex_colors())
        {
            mExtraMesh.request_vertex_colors();
        }
        Mesh::Color colorBlue(0.0, 0.0, 1.0);
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tempVertexHandle = mExtraMesh.add_vertex(contourInterpPoints.at(contourInterpPointIndex));
            mExtraMesh.set_color(tempVertexHandle, colorBlue);
        }
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.Tooth" + toothIndexString + "ContourInterp.off");

        *//*////根据插值结果在mesh上重新搜寻轮廓点
        QVector<Mesh::VertexHandle> interpContour; //插值后映射回mesh的轮廓点集

        //第1个轮廓点
        interpContour.push_back(contourControlVertices.front());

        //第2个轮廓点
        Mesh::VertexHandle nearestVertexHandle; //距离插值曲线最近的顶点handle
        float distanceToInterpCurve, minDistanceToInterpCurve; //邻域定点到插值曲线的距离和最小距离
        float tempDistanceToInterpPoint;
        Mesh::Point tempInterpContourVertex, tempInterpPoint;
        minDistanceToInterpCurve = 1e10;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(interpContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            tempInterpContourVertex = mToothMesh.point(*vertexVertexIter);
            distanceToInterpCurve = 1e10; //TODO 设置一个足够大的数，或者设置为其与第1个插值点的距离
            //计算每个邻域顶点到插值曲线的距离
            for(contourInterpVertexIndex = 0; contourInterpVertexIndex < contourInterpVertexNum; contourInterpVertexIndex++)
            {
                tempInterpPoint = contourInterpPoints.at(contourInterpVertexIndex);
                tempDistanceToInterpPoint = (tempInterpContourVertex[0] - tempInterpPoint[0]) * (tempInterpContourVertex[0] - tempInterpPoint[0]) + (tempInterpContourVertex[1] - tempInterpPoint[1]) * (tempInterpContourVertex[1] - tempInterpPoint[1]) + (tempInterpContourVertex[2] - tempInterpPoint[2]) * (tempInterpContourVertex[2] - tempInterpPoint[2]);
                if(tempDistanceToInterpPoint < distanceToInterpCurve)
                {
                    distanceToInterpCurve = tempDistanceToInterpPoint;
                }
            }
            if(distanceToInterpCurve < minDistanceToInterpCurve)
            {
                minDistanceToInterpCurve = distanceToInterpCurve;
                nearestVertexHandle = *vertexVertexIter;
            }
        }
        interpContour.push_back(nearestVertexHandle);

        //第3个轮廓点
        minDistanceToInterpCurve = 1e10;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(interpContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            tempInterpContourVertex = mToothMesh.point(*vertexVertexIter);
            if(tempInterpContourVertex == mToothMesh.point(interpContour.front())) //跳过第1个轮廓点
            {
                continue;
            }
            if(cos(tempInterpContourVertex - mToothMesh.point(interpContour.back()), mToothMesh.point(interpContour.at(interpContour.size() - 2)) - mToothMesh.point(interpContour.back())) > -0.5) //限制只能往前搜索而不能后退
            {
                continue;
            }
            distanceToInterpCurve = 1e10;
            //计算每个邻域顶点到插值曲线的距离
            for(contourInterpVertexIndex = 0; contourInterpVertexIndex < contourInterpVertexNum; contourInterpVertexIndex++)
            {
                tempInterpPoint = contourInterpPoints.at(contourInterpVertexIndex);
                tempDistanceToInterpPoint = (tempInterpContourVertex[0] - tempInterpPoint[0]) * (tempInterpContourVertex[0] - tempInterpPoint[0]) + (tempInterpContourVertex[1] - tempInterpPoint[1]) * (tempInterpContourVertex[1] - tempInterpPoint[1]) + (tempInterpContourVertex[2] - tempInterpPoint[2]) * (tempInterpContourVertex[2] - tempInterpPoint[2]);
                if(tempDistanceToInterpPoint < distanceToInterpCurve)
                {
                    distanceToInterpCurve = tempDistanceToInterpPoint;
                }
            }
            if(distanceToInterpCurve < minDistanceToInterpCurve)
            {
                minDistanceToInterpCurve = distanceToInterpCurve;
                nearestVertexHandle = *vertexVertexIter;
            }
        }
        interpContour.push_back(nearestVertexHandle);

        //深度搜索获取轮廓点集（现在已找到3个轮廓点）
        contourFinished = false;
        bool duplicated;
        while(true)
        {
            minDistanceToInterpCurve = 1e10;
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(interpContour.back()); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                tempInterpContourVertex = mToothMesh.point(*vertexVertexIter);
                if(tempInterpContourVertex == mToothMesh.point(interpContour.at(interpContour.size() - 2))) //跳过前1个轮廓点
                {
                    continue;
                }
                if(tempInterpContourVertex == mToothMesh.point(interpContour.front())) //如果邻域中找到了第1个轮廓点，说明轮廓已搜寻完毕
                {
                    contourFinished = true;
                    break;
                }
                if(cos(tempInterpContourVertex - mToothMesh.point(interpContour.back()), mToothMesh.point(interpContour.at(interpContour.size() - 2)) - mToothMesh.point(interpContour.back())) > -0.5) //限制只能往前搜索而不能后退
                {
                    continue;
                }

                //不能添加重复的顶点
                duplicated = false;
                for(int interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
                {
                    if(tempInterpContourVertex == mToothMesh.point(interpContour.at(interpContourVertexIndex)))
                    {
                        duplicated = true;
                        break;
                    }
                }
                if(duplicated)
                {
                    continue;
                }

                distanceToInterpCurve = 1e10;
                //计算每个邻域顶点到插值曲线的距离
                for(contourInterpVertexIndex = 0; contourInterpVertexIndex < contourInterpVertexNum; contourInterpVertexIndex++)
                {
                    tempInterpPoint = contourInterpPoints.at(contourInterpVertexIndex);
                    tempDistanceToInterpPoint = (tempInterpContourVertex[0] - tempInterpPoint[0]) * (tempInterpContourVertex[0] - tempInterpPoint[0]) + (tempInterpContourVertex[1] - tempInterpPoint[1]) * (tempInterpContourVertex[1] - tempInterpPoint[1]) + (tempInterpContourVertex[2] - tempInterpPoint[2]) * (tempInterpContourVertex[2] - tempInterpPoint[2]);
                    if(tempDistanceToInterpPoint < distanceToInterpCurve)
                    {
                        distanceToInterpCurve = tempDistanceToInterpPoint;
                    }
                }
                if(distanceToInterpCurve < minDistanceToInterpCurve)
                {
                    minDistanceToInterpCurve = distanceToInterpCurve;
                    nearestVertexHandle = *vertexVertexIter;
                }
            }
            if(contourFinished)
            {
                break;
            }
            else
            {
                interpContour.push_back(nearestVertexHandle);
            }

            //测试
            if(interpContour.size() > 200)
                break;
        }*//*

        //利用K近邻搜索获取mesh上距离插值轮廓最近的顶点集合
        int knn = 4; //对于插值轮廓上的每一个点，在mesh上寻找前k个与其最近的顶点
        QVector< QVector<int> > searchResult = kNearestNeighbours(knn, contourInterpPoints, mToothMeshVertices);
        QVector<Mesh::VertexHandle> interpContour; //插值后映射回mesh的轮廓点集
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            for(int k = 0; k < knn; k++)
            {
                tempVertexHandle = mToothMeshVertexHandles.at(searchResult[contourInterpPointIndex][k]);
                if(!interpContour.contains(tempVertexHandle))
                {
                    interpContour.push_back(tempVertexHandle);
                }
            }
        }

        //测试，将插值后的轮廓点单独保存到文件
        mExtraMesh = Mesh();
        if(!mExtraMesh.has_vertex_colors())
        {
            mExtraMesh.request_vertex_colors();
        }
        Mesh::Color colorYellow(1.0, 1.0, 0.0);
        int interpContourVertexIndex;
        for(interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
        {
            tempVertexHandle = mExtraMesh.add_vertex(mToothMesh.point(interpContour.at(interpContourVertexIndex)));
            mExtraMesh.set_color(tempVertexHandle, colorYellow);
        }
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.Tooth" + toothIndexString + "InterpContour.off");

        //测试，保存显示该牙齿轮廓（插值后）的牙齿模型
        paintAllVerticesWhite();
        for(interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
        {
            mToothMesh.set_color(interpContour.at(interpContourVertexIndex), colorRed);
        }
        saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithTooth" + toothIndexString + "InterpContour.off");

        //测试
        break;

    }*/

    Mesh::Color colorRed(1.0, 0.0, 0.0);

    //分别处理每一个contour section，选取控制点、插值、找近邻区域、细化
    int contourSectionIndex;
    mProgress->setLabelText("Interpolating all contour sections...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mContourSections.size());
    mProgress->setValue(0);
    for(contourSectionIndex = 0; contourSectionIndex < mContourSections.size(); contourSectionIndex++)
    {
        mProgress->setValue(contourSectionIndex);

        //选取插值控制点
        QVector<Mesh::VertexHandle> contourControlVertices; //控制点集
        int contourVertexNum = mContourSections[contourSectionIndex].size();
        int controlVertexDistance = 10; //每两个控制点之间的距离
        if(contourVertexNum < controlVertexDistance * 2 + 1) //如果轮廓太短，则不进行处理
        {
            continue;
        }
        for(int contourVertexIndex = 0; contourVertexIndex < contourVertexNum; contourVertexIndex++)
        {
            if(contourVertexIndex % controlVertexDistance == 0 || contourVertexIndex == contourVertexNum - 1) //TODO 暂时按照等距离选取控制点
            {
                contourControlVertices.push_back(mContourSections[contourSectionIndex].at(contourVertexIndex));
            }
        }

        //将插值控制点从Mesh::Point格式转换为GSL库插值函数需要的格式
        int contourControlVertexNum = contourControlVertices.size();
        double *t = new double[contourControlVertexNum];
        double *x = new double[contourControlVertexNum];
        double *y = new double[contourControlVertexNum];
        double *z = new double[contourControlVertexNum];
        Mesh::Point tempContourControlVertex;
        for(int contourControlVertexIndex = 0; contourControlVertexIndex < contourControlVertexNum; contourControlVertexIndex++)
        {
            t[contourControlVertexIndex] = contourControlVertexIndex;
            tempContourControlVertex = mToothMesh.point(contourControlVertices.at(contourControlVertexIndex));
            x[contourControlVertexIndex] = tempContourControlVertex[0];
            y[contourControlVertexIndex] = tempContourControlVertex[1];
            z[contourControlVertexIndex] = tempContourControlVertex[2];
        }

        //开始插值
        gsl_interp_accel *acc = gsl_interp_accel_alloc();
        gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, contourControlVertexNum);
        int contourInterpPointNum = (contourControlVertexNum - 1) * 100; //TODO 此处插值密度有待自动化计算
        int contourInterpPointIndex;
        double *tInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tInterp[contourInterpPointIndex] = contourInterpPointIndex * 0.01;
        }
        gsl_spline_init(spline, t, x, contourControlVertexNum);
        double *xInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            xInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }
        gsl_spline_init(spline, t, y, contourControlVertexNum);
        double *yInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            yInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }
        gsl_spline_init(spline, t, z, contourControlVertexNum);
        double *zInterp = new double[contourInterpPointNum];
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            zInterp[contourInterpPointIndex] = gsl_spline_eval(spline, tInterp[contourInterpPointIndex], acc);
        }

        gsl_spline_free(spline);
        gsl_interp_accel_free(acc);
        delete[] t;
        delete[] x;
        delete[] y;
        delete[] z;
        delete[] tInterp;

        //将插值得到的数据转换回Mesh::Point格式
        QVector<Mesh::Point> contourInterpPoints; //插值得到的轮廓点集
        Mesh::Point tempContourInterpVertex;
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tempContourInterpVertex[0] = xInterp[contourInterpPointIndex];
            tempContourInterpVertex[1] = yInterp[contourInterpPointIndex];
            tempContourInterpVertex[2] = zInterp[contourInterpPointIndex];
            contourInterpPoints.push_back(tempContourInterpVertex);
        }

        delete[] xInterp;
        delete[] yInterp;
        delete[] zInterp;

        Mesh::VertexHandle tempVertexHandle;

        //测试，将插值得到的轮廓点保存到文件
        /*mExtraMesh = Mesh();
        if(!mExtraMesh.has_vertex_colors())
        {
            mExtraMesh.request_vertex_colors();
        }
        Mesh::Color colorBlue(0.0, 0.0, 1.0);
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            tempVertexHandle = mExtraMesh.add_vertex(contourInterpPoints.at(contourInterpPointIndex));
            mExtraMesh.set_color(tempVertexHandle, colorBlue);
        }
        stringstream ss;
        ss << contourSectionIndex;
        string contourSectionIndexString = ss.str();
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.ContourSection" + contourSectionIndexString + "Interp.off");*/

        //利用K近邻搜索获取mesh上距离插值轮廓最近的顶点集合
        int knn = 2; //对于插值轮廓上的每一个点，在mesh上寻找前k个与其最近的顶点
        QVector< QVector<int> > searchResult = kNearestNeighbours(knn, contourInterpPoints, mToothMeshVertices);
        QVector<Mesh::VertexHandle> interpContour; //插值后映射回mesh的轮廓点集
        for(contourInterpPointIndex = 0; contourInterpPointIndex < contourInterpPointNum; contourInterpPointIndex++)
        {
            for(int k = 0; k < knn; k++)
            {
                tempVertexHandle = mToothMeshVertexHandles.at(searchResult[contourInterpPointIndex][k]);
                if(!interpContour.contains(tempVertexHandle))
                {
                    interpContour.push_back(tempVertexHandle);
                }
            }
        }

        //将所有近邻顶点设置为边界点
        int interpContourVertexIndex;
        for(interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, interpContour.at(interpContourVertexIndex)) = true;
        }

        //测试，将插值后的轮廓点单独保存到文件
        /*mExtraMesh = Mesh();
        if(!mExtraMesh.has_vertex_colors())
        {
            mExtraMesh.request_vertex_colors();
        }
        Mesh::Color colorYellow(1.0, 1.0, 0.0);
        for(interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
        {
            tempVertexHandle = mExtraMesh.add_vertex(mToothMesh.point(interpContour.at(interpContourVertexIndex)));
            mExtraMesh.set_color(tempVertexHandle, colorYellow);
        }
        saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.ContourSection" + contourSectionIndexString + "InterpNearestRegion.off");*/

        //测试，保存显示该轮廓（插值后）的牙齿模型
        /*paintAllVerticesWhite();
        for(interpContourVertexIndex = 0; interpContourVertexIndex < interpContour.size(); interpContourVertexIndex++)
        {
            mToothMesh.set_color(interpContour.at(interpContourVertexIndex), colorRed);
        }
        saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithContourSection" + contourSectionIndexString + "InterpNearestRegion.off");*/
    }

    //测试，保存带平滑轮廓（非单点宽度）的牙齿模型到文件
//    paintAllVerticesWhite();
//    paintBoundaryVertices();
//    saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithInterpNearestRegion.off");

    //重新进行区域生长
    markNonBoundaryRegion();

    //测试，保存带平滑轮廓（非单点宽度）并进行区域生长标记后的牙齿模型到文件
//    paintClassifiedNonBoundaryRegions();
//    saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithInterpNearestRegionAndRegionGrowing.off");

    //重新进行单点宽度边界提取
    boundarySkeletonExtraction();

    //测试，保存重新提取单点宽度边界后的牙齿模型到文件
//    paintClassifiedNonBoundaryRegions();
//    paintBoundaryVertices();
//    saveToothMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.WithInterpNearestRegionSkeleton.off");

    //重新建立轮廓点索引
    findCuttingPoints();
    indexContourSectionsVertices();

    //模板平滑（此步骤通过移动轮廓点的位置，来使得轮廓变得平滑）
    Mesh::Point tempPoint;
    const int halfWindowSize = 5; //平滑窗口半边长，窗口长度为其2倍加1，TODO 此值要根据模型顶点总数自动调节
    const int windowSize = halfWindowSize * 2 + 1;
    int realHalfWindowSize; //实际的窗口半边长，轮廓两端处窗口放不下时使用
    int realWindowSize;
    mProgress->setLabelText("Smoothing all contour sections...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mContourSections.size());
    mProgress->setValue(0);
    for(contourSectionIndex = 0; contourSectionIndex < mContourSections.size(); contourSectionIndex++)
    {
        mProgress->setValue(contourSectionIndex);
        if(mContourSections[contourSectionIndex].size() < windowSize)
        {
            continue;
        }
        for(int contourSectionVertexIndex = 1; contourSectionVertexIndex < mContourSections[contourSectionIndex].size() - 1; contourSectionVertexIndex++)
        {
            tempPoint[0] = 0.0; tempPoint[1] = 0.0; tempPoint[2] = 0.0;

            if(contourSectionVertexIndex < halfWindowSize)
            {
                realHalfWindowSize = contourSectionVertexIndex;
            }
            else if(contourSectionVertexIndex > mContourSections[contourSectionIndex].size() - 1 - halfWindowSize)
            {
                realHalfWindowSize = mContourSections[contourSectionIndex].size() - 1 - contourSectionVertexIndex;
            }
            realWindowSize = realHalfWindowSize * 2 + 1;

            for(int i = contourSectionVertexIndex - realHalfWindowSize; i <= contourSectionVertexIndex + realHalfWindowSize; i++)
            {
                tempPoint += mToothMesh.point(mContourSections[contourSectionIndex].at(i));
            }
            tempPoint /= realWindowSize;
            mToothMesh.set_point(mContourSections[contourSectionIndex].at(contourSectionVertexIndex), tempPoint);
        }
    }

    //因为轮廓点坐标改变，所以需要更新mesh顶点的线性索引
    mToothMeshVertices.clear();
    mToothMeshVertexHandles.clear();
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mToothMeshVertices.push_back(mToothMesh.point(*vertexIter));
        mToothMeshVertexHandles.push_back(*vertexIter);
    }

    //由于移动了轮廓顶点的位置，其周围可能出现较大的凹凸，因此需要对其周围顶点做平滑
    bool *smoothContourNeighborVisited = (bool*)calloc(mToothMesh.mVertexNum, sizeof(bool));
    QVector<Mesh::VertexHandle> neighborVertexHandles;
    bool isContourNeighborVertex;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            continue;
        }
        if(smoothContourNeighborVisited[vertexIter->idx()])
        {
            continue;
        }

        //判断是否是轮廓周围点
        isContourNeighborVertex = false;
        neighborVertexHandles.clear();
        getKRing(*vertexIter, 2, neighborVertexHandles);
        for(int i = 0; i < neighborVertexHandles.size(); i++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, neighborVertexHandles.at(i)))
            {
                isContourNeighborVertex = true;
            }
        }

        if(!isContourNeighborVertex)
        {
            continue;
        }

        //平滑
        tempPoint[0] = 0.0; tempPoint[1] = 0.0; tempPoint[2] = 0.0;
        for(int i = 0; i < neighborVertexHandles.size(); i++)
        {
            tempPoint += mToothMesh.point(neighborVertexHandles.at(i));
        }
        tempPoint /= neighborVertexHandles.size();
        mToothMesh.set_point(*vertexIter, tempPoint);
    }
    free(smoothContourNeighborVisited);
}

void ToothSegmentation::paintAllVerticesWhite()
{
    int vertexIndex = 0;
    mProgress->setLabelText("Painting all vertices white...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    Mesh::Color colorWhite(1.0, 1.0, 1.0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        mToothMesh.set_color(*vertexIter, colorWhite);
        vertexIndex++;
    }
}

void ToothSegmentation::createPlaneInExtraMesh(Mesh::Point point, Mesh::Normal normal, float size)
{
    float halfDiagonalLineLength = size / 1.414; //正方形中心到4个顶点的距离
    float x0, y0, z0; //质心点（牙龈分割平面正方形中心点(记为点O)）
    x0 = point[0];
    y0 = point[1];
    z0 = point[2];
    float x1, y1, z1; //法向量
    x1 = normal[0];
    y1 = normal[1];
    z1 = normal[2];
    float x2, y2, z2; //牙龈分割平面上的另一个点(记为点N)，若平面不与x轴平行，则取该点为平面与x轴的交点，否则按y、z轴依此类推
    if(x1 != 0.0)
    {
        x2 = (y1 * y0 + z1 * z0) / x1 + x0;
        y2 = 0.0;
        z2 = 0.0;
    }
    else if(y1 != 0.0)
    {
        x2 = 0.0;
        y2 = (x1 * x0 + z1 * z0) / y1 + y0;
        z2 = 0;
    }
    else if(z1 != 0.0)
    {
        x2 = 0.0;
        y2 = 0.0;
        z2 = (y1 * y0 + x1 * x0) / z1 + z0;
    }
    float scale1 = halfDiagonalLineLength / sqrt((x2 - x0) * (x2 - x0) + (y2 - y0) * (y2 - y0) + (z2 - z0) * (z2 - z0)); //向量ON乘以此倍数得到长度为halfDiagonalLineLength的向量（从点O指向正方形其中一个顶点(记为点A)）
    float x3, y3, z3; //正方形其中一个顶点(点A)
    x3 = x0 + (x2 - x0) * scale1;
    y3 = y0 + (y2 - y0) * scale1;
    z3 = z0 + (z2 - z0) * scale1;
    float x4, y4, z4; //正方形中与上述顶点相邻的顶点(记为点B)
    float x5, y5, z5; //与向量OB同向的向量(记为向量OM)（由法向量和OA作叉积而得）
    x5 = y1 * (z3 - z0) - z1 * (y3 - y0);
    y5 = z1 * (x3 - x0) - x1 * (z3 - z0);
    z5 = x1 * (y3 - y0) - y1 * (x3 - x0);
    float scale2 = halfDiagonalLineLength / sqrt(x5 * x5 + y5 * y5 + z5 * z5); //向量OM乘以此倍数得到向量OB
    x4 = x0 + x5 * scale2;
    y4 = y0 + y5 * scale2;
    z4 = z0 + z5 * scale2;
    float x6, y6, z6; //正方形中与顶点A相对的顶点(记为点C)（根据A、C两点关于点O对称而得）
    x6 = x0 * 2.0 - x3;
    y6 = y0 * 2.0 - y3;
    z6 = z0 * 2.0 - z3;
    float x7, y7, z7; //正方形中与顶点B相对的顶点(记为点D)（根据B、D两点关于点O对称而得）
    x7 = x0 * 2.0 - x4;
    y7 = y0 * 2.0 - y4;
    z7 = z0 * 2.0 - z4;

    //测试，输出分割平面正方形4个顶点坐标
    cout << "牙龈分割平面正方形顶点：\n"
         << "A: " << x3 << ", " << y3 << ", " << z3 << "\n"
         << "B: " << x4 << ", " << y4 << ", " << z4 << "\n"
         << "C: " << x6 << ", " << y6 << ", " << z6 << "\n"
         << "D: " << x7 << ", " << y7 << ", " << z7 << endl;

    //将分割平面添加到mExtraMesh中，以便显示
    mExtraMesh = Mesh();
    Mesh::VertexHandle vertexHandles[4];
    vertexHandles[0] = mExtraMesh.add_vertex(Mesh::Point(x3, y3, z3));
    vertexHandles[1] = mExtraMesh.add_vertex(Mesh::Point(x4, y4, z4));
    vertexHandles[2] = mExtraMesh.add_vertex(Mesh::Point(x6, y6, z6));
    vertexHandles[3] = mExtraMesh.add_vertex(Mesh::Point(x7, y7, z7));
    vector<Mesh::VertexHandle> faceVertexhandles;
    faceVertexhandles.clear();
    faceVertexhandles.push_back(vertexHandles[0]);
    faceVertexhandles.push_back(vertexHandles[1]);
    faceVertexhandles.push_back(vertexHandles[2]);
    faceVertexhandles.push_back(vertexHandles[3]);
    mExtraMesh.add_face(faceVertexhandles);
    if(!mExtraMesh.has_vertex_normals())
    {
        mExtraMesh.request_vertex_normals();
    }
    mExtraMesh.set_normal(vertexHandles[0], normal);
    mExtraMesh.set_normal(vertexHandles[1], normal);
    mExtraMesh.set_normal(vertexHandles[2], normal);
    mExtraMesh.set_normal(vertexHandles[3], normal);
    Mesh::FaceHandle faceHandle = *(mExtraMesh.vf_iter(vertexHandles[0]));
    if(!mExtraMesh.has_face_normals())
    {
        mExtraMesh.request_face_normals();
    }
    mExtraMesh.set_normal(faceHandle, normal);
    Mesh::Color colorGreen(0.3, 1.0, 0.0); //TODO 将分割平面颜色设置成半透明（经测试，直接在Mesh::draw()方法中将glColor3f()改为glColor4f()并添加alpha参数并不会使的模型显示透明效果）
    if(!mExtraMesh.has_vertex_colors())
    {
        mExtraMesh.request_vertex_colors();
    }
    mExtraMesh.set_color(vertexHandles[0], colorGreen);
    mExtraMesh.set_color(vertexHandles[1], colorGreen);
    mExtraMesh.set_color(vertexHandles[2], colorGreen);
    mExtraMesh.set_color(vertexHandles[3], colorGreen);
    if(!mExtraMesh.has_face_colors())
    {
        mExtraMesh.request_face_colors();
    }
    mExtraMesh.set_color(faceHandle, colorGreen);
}

void ToothSegmentation::findCuttingPoints(bool loadStateFromFile)
{
    mProgress->setWindowTitle("Finding cutting points...");

    findCuttingPoints();

    paintAllVerticesWhite();
    paintClassifiedBoundary();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".FindCuttingPoints.off");
    paintClassifiedNonBoundaryRegions();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".FindCuttingPoints.WithRegionGrowing.off");

    //关闭进度条
    mProgress->close();

    mShouldShowExtraMesh = false;
    mProgramSchedule = SCHEDULE_FindCuttingPoints_FINISHED;
}

bool ToothSegmentation::saveState(string stateSymbol)
{
    string stateFileName = mToothMesh.MeshName.toStdString() + "." + stateSymbol + ".State";
    QFile stateFile(stateFileName.c_str());
    if(!stateFile.open(QIODevice::WriteOnly))
    {
        cout << "Fail to open file \"" << stateFileName << "\" ." << endl;
        return false;
    }

    mProgress->setLabelText("Saving state...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);

    stateFile.write((char *)(&mBoundaryVertexNum), sizeof(mBoundaryVertexNum));
    stateFile.write((char *)(&mGingivaCuttingPlanePoint), sizeof(mGingivaCuttingPlanePoint));
    stateFile.write((char *)(&mGingivaCuttingPlaneNormal), sizeof(mGingivaCuttingPlaneNormal));
    stateFile.write((char *)(&mToothNum), sizeof(mToothNum));

    float tempCurvature;
    bool tempCurvatureComputed;
    bool tempIsToothBoundary;
    int tempBoundaryVertexType;
    int tempNonBoundaryRegionType;
    bool tempRegionGrowingVisited;
    int tempBoundaryType;
    Mesh::Color tempColor;
    int vertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        tempCurvature = mToothMesh.property(mVPropHandleCurvature, *vertexIter);
        stateFile.write((char *)(&tempCurvature), sizeof(tempCurvature));
        tempCurvatureComputed = mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter);
        stateFile.write((char *)(&tempCurvatureComputed), sizeof(tempCurvatureComputed));
        tempIsToothBoundary = mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter);
        stateFile.write((char *)(&tempIsToothBoundary), sizeof(tempIsToothBoundary));
        tempBoundaryVertexType = mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter);
        stateFile.write((char *)(&tempBoundaryVertexType), sizeof(tempBoundaryVertexType));
        tempNonBoundaryRegionType = mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter);
        stateFile.write((char *)(&tempNonBoundaryRegionType), sizeof(tempNonBoundaryRegionType));
        tempRegionGrowingVisited = mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter);
        stateFile.write((char *)(&tempRegionGrowingVisited), sizeof(tempRegionGrowingVisited));
        tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, *vertexIter);
        stateFile.write((char *)(&tempBoundaryType), sizeof(tempBoundaryType));
        tempColor = mToothMesh.color(*vertexIter);
        stateFile.write((char *)(&tempColor), sizeof(tempColor));
        vertexIndex++;
    }
    stateFile.close();
    return true;
}

bool ToothSegmentation::loadState(string stateSymbol)
{
    string stateFileName = mToothMesh.MeshName.toStdString() + "." + stateSymbol + ".State";
    QFile stateFile(stateFileName.c_str());
    if(!stateFile.open(QIODevice::ReadOnly))
    {
        cout << "Fail to open file \"" << stateFileName << "\" ." << endl;
        return false;
    }

    mProgress->setLabelText("Loading state...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);

    stateFile.read((char *)(&mBoundaryVertexNum), sizeof(mBoundaryVertexNum));
    stateFile.read((char *)(&mGingivaCuttingPlanePoint), sizeof(mGingivaCuttingPlanePoint));
    stateFile.read((char *)(&mGingivaCuttingPlaneNormal), sizeof(mGingivaCuttingPlaneNormal));
    stateFile.read((char *)(&mToothNum), sizeof(mToothNum));

    float tempCurvature;
    bool tempCurvatureComputed;
    bool tempIsToothBoundary;
    int tempBoundaryVertexType;
    int tempNonBoundaryRegionType;
    bool tempRegionGrowingVisited;
    int tempBoundaryType;
    Mesh::Color tempColor;
    int vertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        stateFile.read((char *)(&tempCurvature), sizeof(tempCurvature));
        mToothMesh.property(mVPropHandleCurvature, *vertexIter) = tempCurvature;
        stateFile.read((char *)(&tempCurvatureComputed), sizeof(tempCurvatureComputed));
        mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter) = tempCurvatureComputed;
        stateFile.read((char *)(&tempIsToothBoundary), sizeof(tempIsToothBoundary));
        mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = tempIsToothBoundary;
        stateFile.read((char *)(&tempBoundaryVertexType), sizeof(tempBoundaryVertexType));
        mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = tempBoundaryVertexType;
        stateFile.read((char *)(&tempNonBoundaryRegionType), sizeof(tempNonBoundaryRegionType));
        mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = tempNonBoundaryRegionType;
        stateFile.read((char *)(&tempRegionGrowingVisited), sizeof(tempRegionGrowingVisited));
        mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = tempRegionGrowingVisited;
        stateFile.read((char *)(&tempBoundaryType), sizeof(tempBoundaryType));
        mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = tempBoundaryType;
        stateFile.read((char *)(&tempColor), sizeof(tempColor));
        mToothMesh.set_color(*vertexIter, tempColor);
        vertexIndex++;
    }
    stateFile.close();
    return true;
}

QVector< QVector<int> > ToothSegmentation::kNearestNeighbours(int Knn, const QVector<Mesh::Point> &querys, const QVector<Mesh::Point> &points)
{
//    mProgress->setLabelText("Computing k nearest neighbours...");
//    mProgress->setMinimum(0);
//    mProgress->setMaximum(querys.size());
//    mProgress->setValue(0);

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

    // Generate pointcloud data
    cloud->width = points.size();
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);

    for(size_t i = 0; i < cloud->points.size(); i++)
    {
        cloud->points[i].x = points[i][0];
        cloud->points[i].y = points[i][1];
        cloud->points[i].z = points[i][2];
    }

    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
    kdtree.setInputCloud(cloud);

    pcl::PointXYZ query;
    std::vector<int> pointIdxNKNSearch(Knn);
    std::vector<float> pointNKNSquaredDistance(Knn);

    QVector<QVector<int> > neighbours(querys.size());
    for(int i = 0; i < querys.size(); i++)
    {
//        mProgress->setValue(i);
        query.x = querys[i][0];
        query.y = querys[i][1];
        query.z = querys[i][2];
        if(kdtree.nearestKSearch(query, Knn, pointIdxNKNSearch, pointNKNSquaredDistance) > 0)
        {
            for(int j=0; j < pointIdxNKNSearch.size(); j++)
            {
                neighbours[i].append(pointIdxNKNSearch[j]);
            }
        }
    }

    return neighbours;
}

QVector< QVector<int> > ToothSegmentation::kNearestNeighbours(int Knn, const QVector<QPoint> &querys, const QVector<QPoint> &points)
{
    pcl::PointCloud<pcl::PointXY>::Ptr cloud(new pcl::PointCloud<pcl::PointXY>);

    // Generate pointcloud data
    cloud->width = points.size();
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);

    for(size_t i = 0; i < cloud->points.size(); i++)
    {
        cloud->points[i].x = points[i].x();
        cloud->points[i].y = points[i].y();
    }

    pcl::KdTreeFLANN<pcl::PointXY> kdtree;
    kdtree.setInputCloud(cloud);

    pcl::PointXY query;
    std::vector<int> pointIdxNKNSearch(Knn);
    std::vector<float> pointNKNSquaredDistance(Knn);

    QVector<QVector<int> > neighbours(querys.size());
    for(int i = 0; i < querys.size(); i++)
    {
        query.x = querys[i].x();
        query.y = querys[i].y();
        if(kdtree.nearestKSearch(query, Knn, pointIdxNKNSearch, pointNKNSquaredDistance) > 0)
        {
            for(int j=0; j < pointIdxNKNSearch.size(); j++)
            {
                neighbours[i].append(pointIdxNKNSearch[j]);
            }
        }
    }

    return neighbours;
}

void ToothSegmentation::findCuttingPoints()
{
    int boundaryVertexIndex;

    //初始化所有顶点的BoundaryType为除CUTTING_POINT之外的任一类型，因为在保证不能存在两个相邻的cutting point时需要知道某顶点是否属于CUTTING_POINT
    boundaryVertexIndex = 0;
    mProgress->setLabelText("Classifing boundary(init BoundaryType of all boundary vertices)...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
        {
            continue;
        }
        mProgress->setValue(boundaryVertexIndex);
        mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = TOOTH_GINGIVA_BOUNDARY;
        mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexIter) = false;
    }

    mCuttingPointHandles.clear();
    mJointPointHandles.clear();

    bool neighborHasGingivaRegion; //某顶点是否与牙龈区域相邻（若是，则可能为TOOTH_GINGIVA_BOUNDARY或CUTTING_POINT）
    int neighborBoundaryVertexNum; //某顶点邻域中边界点数量（若超过2个，并且其中没有cutting point，则为CUTTING_POINT）
    bool neighbor2RingHasCuttingPoint; //某顶点2邻域中是否已有cutting point（有一种情况是cutting point处边界点构成了一个三角形，这种情况只能取其中1个作为cutting point；考虑到有可能出现某边界点连接多个边界点的情况，此处应取2邻域）
    bool neighbor2RingHasJointPoint; //某顶点2邻域中是否已有joint point（同上）
    bool neighbor2RingHasGingivaRegion; //某顶点2邻域中是否有牙龈区域点（为了去除cutting point附近可能被错判的joint point）
    QVector<Mesh::VertexHandle> neighbor2RingVertexHandles;
    Mesh::VertexHandle tempVertexHandle;
    mProgress->setLabelText("Classifing boundary...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
        {
            continue;
        }

        mProgress->setValue(boundaryVertexIndex);

        neighborHasGingivaRegion = false;
        neighborBoundaryVertexNum = 0;
        neighbor2RingHasCuttingPoint = false;
        neighbor2RingHasJointPoint = false;
        neighbor2RingHasGingivaRegion = false;
        neighbor2RingVertexHandles.clear();
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborBoundaryVertexNum++;
                continue;
            }
            if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) == GINGIVA_REGION)
            {
                neighborHasGingivaRegion = true;
            }
        }
        getKRing(*vertexIter, 2, neighbor2RingVertexHandles);
        for(int i = 0; i < neighbor2RingVertexHandles.size(); i++)
        {
            tempVertexHandle = neighbor2RingVertexHandles.at(i);
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, tempVertexHandle))
            {
                if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, tempVertexHandle) == GINGIVA_REGION)
                {
                    neighbor2RingHasGingivaRegion = true;
                }
                continue;
            }
            if(mToothMesh.property(mVPropHandleBoundaryType, tempVertexHandle) == CUTTING_POINT)
            {
                neighbor2RingHasCuttingPoint = true;
            }
            else if(mToothMesh.property(mVPropHandleBoundaryType, tempVertexHandle) == JOINT_POINT)
            {
                neighbor2RingHasJointPoint = true;
            }
        }

        if(neighborHasGingivaRegion) //如果与牙龈区域相邻，则可能是TOOTH_GINGIVA_BOUNDARY或CUTTING_POINT
        {
            if(neighborBoundaryVertexNum > 2 && !neighbor2RingHasCuttingPoint)
            {
                mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = CUTTING_POINT;
                mCuttingPointHandles.push_back(*vertexIter);
            }
            else
            {
                mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = TOOTH_GINGIVA_BOUNDARY;
            }
        }
        else //如果不与牙龈区域相邻，则可能是TOOTH_TOOTH_BOUNDARY或JOINT_POINT
        {
            if(neighborBoundaryVertexNum > 2 && !neighbor2RingHasJointPoint && !neighbor2RingHasGingivaRegion)
            {
                mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = JOINT_POINT;
                mJointPointHandles.push_back(*vertexIter);
            }
            else
            {
                mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) = TOOTH_TOOTH_BOUNDARY;
            }
        }

        boundaryVertexIndex++;
    }

    //寻找所有轮廓曲线段（两个cutting point或joint point之间的轮廓线）
    indexContourSectionsVertices();
}

void ToothSegmentation::paintClassifiedBoundary()
{
    int boundaryVertexIndex = 0;
    Mesh::Color colorRed(1.0, 0.0, 0.0), colorBlue(0.0, 0.0, 1.0), colorYellow(1.0, 1.0, 0.0), colorPink(1.0, 0.0, 1.0);
    mProgress->setLabelText("Painting classified boundary...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
        {
            continue;
        }

        mProgress->setValue(boundaryVertexIndex);

        switch(mToothMesh.property(mVPropHandleBoundaryType, *vertexIter))
        {
        case TOOTH_GINGIVA_BOUNDARY:
            mToothMesh.set_color(*vertexIter, colorRed);
            break;
        case TOOTH_TOOTH_BOUNDARY:
            mToothMesh.set_color(*vertexIter, colorBlue);
            break;
        case CUTTING_POINT:
            mToothMesh.set_color(*vertexIter, colorYellow);
            break;
        case JOINT_POINT:
            mToothMesh.set_color(*vertexIter, colorPink);
            break;
        }

        boundaryVertexIndex++;
    }
}

void ToothSegmentation::indexContourSectionsVertices()
{
    if(!mContourSections.empty())
    {
        mContourSections.clear();
    }

    int contourSectionIndex = 0;

    //初始化SearchContourSectionVisited属性
    int boundaryVertexIndex = 0;
    mProgress->setLabelText("Init SearchContourSectionVisited of all boundary vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
        {
            continue;
        }
        mProgress->setValue(boundaryVertexIndex);
        mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexIter) = false;
        boundaryVertexIndex++;
    }

    //从每个cutting point或joint point开始查找
    QVector<Mesh::VertexHandle> cuttingPointAndJointPointHandles = mCuttingPointHandles + mJointPointHandles;
    int cuttingPointAndJointPointIndex;
    Mesh::VertexHandle tempCuttingPointHandle;
    mProgress->setLabelText("Finding all contour sections...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(cuttingPointAndJointPointHandles.size());
    mProgress->setValue(0);
    for(cuttingPointAndJointPointIndex = 0; cuttingPointAndJointPointIndex < cuttingPointAndJointPointHandles.size(); cuttingPointAndJointPointIndex++)
    {
        mProgress->setValue(cuttingPointAndJointPointIndex);
        tempCuttingPointHandle = cuttingPointAndJointPointHandles.at(cuttingPointAndJointPointIndex);

        //查找所有以此cutting point为端点的轮廓曲线段
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(tempCuttingPointHandle); vertexVertexIter.is_valid(); vertexVertexIter++) //寻找邻域中有没有属于该牙齿区域的非边界点
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
            {
                continue;
            }
            if(!mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter)) //如果此邻域边界点未被访问过，则从此点开始搜索该轮廓曲线段
            {
                mContourSections.push_back(QVector<Mesh::VertexHandle>());
                mContourSections[contourSectionIndex].push_back(tempCuttingPointHandle); //将该cutting point添加作为该contour section的第1个点
                mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将该邻域边界点添加作为该contour section的第2个点
                if(mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back()) == CUTTING_POINT
                        || mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back()) == JOINT_POINT) //如果这第2个点为cutting point或joint point，则该contour section搜索结束，TODO 但此轮廓段太短，前台应给出提示
                {
                    continue;
                }
                mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter) = true;

                //寻找第3个点（不能是起始点的1邻域点,防止在起始点处为三角形的情况时可能出现某条轮廓搜索无法到达终点）
                bool neighborHasStartVertex = false;
                for(Mesh::VertexVertexIter vvvIter = mToothMesh.vv_iter(*vertexVertexIter); vvvIter.is_valid(); vvvIter++)
                {
                    if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vvvIter)) //跳过非边界点
                    {
                        continue;
                    }
                    if(*vvvIter == tempCuttingPointHandle) //跳过起始点
                    {
                        continue;
                    }
                    for(Mesh::VertexVertexIter vvvvIter = mToothMesh.vv_iter(*vvvIter); vvvvIter.is_valid(); vvvvIter++)
                    {
                        if(*vvvvIter == tempCuttingPointHandle)
                        {
                            neighborHasStartVertex = true;
                        }
                    }
                    if(neighborHasStartVertex) //跳过起始点的1邻域点
                    {
                        continue;
                    }
                    mContourSections[contourSectionIndex].push_back(*vvvIter);
                    mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vvvIter) = true;
                    break;
                }
                if(mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back()) == CUTTING_POINT
                        || mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back()) == JOINT_POINT) //如果这第3个点为cutting point或joint point，则该contour section搜索结束，TODO 但此轮廓段太短，前台应给出提示
                {
                    continue;
                }

                Mesh::Point previousContourVertex; //目前找到的倒数第2个轮廓点
                bool contourSectionFinished = false;
                bool newVertexAdded; //某次迭代是否添加了新的顶点
                while(true)
                {
                    previousContourVertex = mToothMesh.point(mContourSections[contourSectionIndex].at(mContourSections[contourSectionIndex].size() - 2));
                    newVertexAdded = false;

                    //寻找邻域中有没有cutting point或joint point，有则该轮廓段搜索结束
                    for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(mContourSections[contourSectionIndex].back()); vertexVertexIter.is_valid(); vertexVertexIter++)
                    {
                        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
                        {
                            continue;
                        }
                        if(mToothMesh.point(*vertexVertexIter) == previousContourVertex) //跳过之前的轮廓点
                        {
                            continue;
                        }
                        if(mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) == CUTTING_POINT
                                || mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) == JOINT_POINT) //到达另一个cutting point或joint point，该contour section搜索结束
                        {
                            mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将此cutting point添加作为该contour section的最后1个点
                            newVertexAdded = true;
                            contourSectionFinished = true;
                            break;
                        }
                    }
                    if(contourSectionFinished)
                    {
                        break;
                    }

                    //寻找邻域中下一个可以被添加的点
                    for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(mContourSections[contourSectionIndex].back()); vertexVertexIter.is_valid(); vertexVertexIter++)
                    {
                        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
                        {
                            continue;
                        }
                        if(mToothMesh.point(*vertexVertexIter) == previousContourVertex) //跳过之前的轮廓点
                        {
                            continue;
                        }
                        if(mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) == CUTTING_POINT
                                || mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) == JOINT_POINT) //到达另一个cutting point或joint point，该contour section搜索结束
                        {
                            mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将此cutting point添加作为该contour section的最后1个点
                            newVertexAdded = true;
                            contourSectionFinished = true;
                            break;
                        }
                        if(mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter)) //不能重复添加某个点
                        {
                            continue;
                        }
                        if(mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) != mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back())) //某个contour section上除两个端点之外的所有点的BoundaryType应该相同（cutting point处边界连成三角形的情况如果不加此限制，会丢掉一条contour section或迭代达不到终点），TODO 由于上面加了寻找第3个点的控制，此行条件应该可以去掉了
                        {
                            continue;
                        }
                        mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //如果以上条件都不满足，则将此边界点添加作为该contour section的1个点
                        newVertexAdded = true;
                        mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter) = true;
                    }
                    if(contourSectionFinished)
                    {
                        break;
                    }
                    if(!newVertexAdded) //如果没有添加新的顶点，则可能是该边界在模型边缘处中断（由于模型不完整或存在空洞导致）
                    {
                        if(mToothMesh.is_boundary(mContourSections[contourSectionIndex].back()))
                        {
                            break;
                        }
                        else //TODO 此情况为出错，如果到达此步需检查
                        {
                            QMessageBox::information(mParentWidget, "Error", "Error finding contour section.");
                            //throw runtime_error("Error finding contour section.");
                            cout << "轮廓段搜索出错点：" << mToothMesh.point(mContourSections[contourSectionIndex].back()) << endl;
                            break;
                        }
                    }
                }

                //测试，将每段轮廓曲线段保存到文件
//                stringstream ss;
//                ss << contourSectionIndex;
//                string contourSectionIndexString = ss.str();
//                mExtraMesh = Mesh();
//                if(!mExtraMesh.has_vertex_colors())
//                {
//                    mExtraMesh.request_vertex_colors();
//                }
//                Mesh::VertexHandle tempVertexHandle;
//                Mesh::Color colorRed(1.0, 0.0, 0.0);
//                for(int contourSectionVertexIndex = 0; contourSectionVertexIndex < mContourSections[contourSectionIndex].size(); contourSectionVertexIndex++)
//                {
//                    tempVertexHandle = mExtraMesh.add_vertex(mToothMesh.point(mContourSections[contourSectionIndex].at(contourSectionVertexIndex)));
//                    mExtraMesh.set_color(tempVertexHandle, colorRed);
//                }
//                saveExtraMesh(mToothMesh.MeshName.toStdString() + ".RefineToothBoundary.Extra.ContourSection" + contourSectionIndexString + ".off");

                contourSectionIndex++;
            }
        }
    }

    //可能会有单独一颗牙齿，此时该轮廓段并没有cutting point或joint point作为起止点，需要对此情况作处理
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非边界点
        {
            continue;
        }
        if(mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexIter)) //跳过已被搜索过的点
        {
            continue;
        }
        if(mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) == CUTTING_POINT
                || mToothMesh.property(mVPropHandleBoundaryType, *vertexIter) == JOINT_POINT) //跳过cutting point和joint point
        {
            continue;
        }

        //从该点开始搜索轮廓
        mContourSections.push_back(QVector<Mesh::VertexHandle>());
        contourSectionIndex = mContourSections.size() - 1;
        mContourSections[contourSectionIndex].push_back(*vertexIter); //将该点添加作为该contour section的第1个点
        mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexIter) = true;
        bool secondContourVertexAdded = false;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
            {
                continue;
            }
            if(mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter))
            {
                continue;
            }
            mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将该点添加作为该contour section的第2个点
            mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter) = true;
            secondContourVertexAdded = true;
        }
        if(!secondContourVertexAdded)
        {
            QMessageBox::information(mParentWidget, "Error", "Error finding contour section.");
            return;
        }
        bool thirdContourVertexAdded = false;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(mContourSections[contourSectionIndex].back()); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
            {
                continue;
            }
            if(mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter))
            {
                continue;
            }
            if(*vertexVertexIter == mContourSections[contourSectionIndex].front())
            {
                continue;
            }
            mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将该点添加作为该contour section的第3个点
            mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter) = true;
            thirdContourVertexAdded = true;
        }
        if(!thirdContourVertexAdded)
        {
            QMessageBox::information(mParentWidget, "Error", "Error finding contour section.");
            return;
        }
        Mesh::VertexHandle startContourVertex = mContourSections[contourSectionIndex].front(); //目前搜索的轮廓的起始点
        Mesh::VertexHandle previousContourVertex; //目前找到的倒数第2个轮廓点
        bool contourSectionFinished = false;
        bool newVertexAdded; //某次迭代是否添加了新的顶点
        while(true)
        {
            previousContourVertex = mContourSections[contourSectionIndex].at(mContourSections[contourSectionIndex].size() - 2);
            newVertexAdded = false;

            //寻找邻域中有没有该轮廓段的起始点，有则该轮廓段搜索结束
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(mContourSections[contourSectionIndex].back()); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
                {
                    continue;
                }
                if(*vertexVertexIter == previousContourVertex) //跳过之前的轮廓点
                {
                    continue;
                }
                if(*vertexVertexIter == startContourVertex)
                {
                    mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将此cutting point添加作为该contour section的最后1个点
                    newVertexAdded = true;
                    contourSectionFinished = true;
                    break;
                }
            }
            if(contourSectionFinished)
            {
                break;
            }

            //寻找邻域中下一个可以被添加的点
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(mContourSections[contourSectionIndex].back()); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter)) //跳过非边界点
                {
                    continue;
                }
                if(*vertexVertexIter == previousContourVertex) //跳过之前的轮廓点
                {
                    continue;
                }
                if(*vertexVertexIter == startContourVertex)
                {
                    mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //将此cutting point添加作为该contour section的最后1个点
                    newVertexAdded = true;
                    contourSectionFinished = true;
                    break;
                }
                if(mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter)) //不能重复添加某个点
                {
                    continue;
                }
                if(mToothMesh.property(mVPropHandleBoundaryType, *vertexVertexIter) != mToothMesh.property(mVPropHandleBoundaryType, mContourSections[contourSectionIndex].back()))
                {
                    continue;
                }
                mContourSections[contourSectionIndex].push_back(*vertexVertexIter); //如果以上条件都不满足，则将此边界点添加作为该contour section的1个点
                newVertexAdded = true;
                mToothMesh.property(mVPropHandleSearchContourSectionVisited, *vertexVertexIter) = true;
            }
            if(contourSectionFinished)
            {
                break;
            }
            if(!newVertexAdded) //如果没有添加新的顶点，则可能是该边界在模型边缘处中断（由于模型不完整或存在空洞导致）
            {
                if(mToothMesh.is_boundary(mContourSections[contourSectionIndex].back()))
                {
                    break;
                }
                else //TODO 此情况为出错，如果到达此步需检查
                {
                    QMessageBox::information(mParentWidget, "Error", "Error finding contour section.");
                    //throw runtime_error("Error finding contour section.");
                    cout << "轮廓段搜索出错点：" << mToothMesh.point(mContourSections[contourSectionIndex].back()) << endl;
                    break;
                }
            }
        }
    }
}

inline void ToothSegmentation::getKRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles)
{
    int bufSize = mToothMesh.mVertexNum;
    ringVertexHandles.reserve(bufSize);
    bool* visited = (bool*)calloc(bufSize, sizeof(bool));

    QVector< pair<Mesh::VertexHandle, int> > queue; //TODO 换成std::list
    queue.reserve(bufSize);
    queue.push_back(pair<Mesh::VertexHandle, int>(centerVertexHandle, 0));
    visited[centerVertexHandle.idx()] = true;

    Mesh::VertexHandle tempVertexHandle;
    int tempDistance;
    while(!queue.empty())
    {
        tempVertexHandle = queue.front().first;
        tempDistance = queue.front().second;
        queue.pop_front();
        ringVertexHandles.push_back(tempVertexHandle);
        if(tempDistance < k)
        {
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(tempVertexHandle); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!visited[vertexVertexIter->idx()])
                {
                    queue.push_back(pair<Mesh::VertexHandle, int>(*vertexVertexIter, tempDistance + 1));
                    visited[vertexVertexIter->idx()] = true;
                }
            }
        }
    }

    free(visited);
}

inline void ToothSegmentation::getKthRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &ringVertexHandles)
{
    int bufSize = mToothMesh.mVertexNum;
    ringVertexHandles.reserve(bufSize);
    bool* visited = (bool*)calloc(bufSize, sizeof(bool));

    QVector< pair<Mesh::VertexHandle, int> > queue; //TODO 换成std::list
    queue.reserve(bufSize);
    queue.push_back(pair<Mesh::VertexHandle, int>(centerVertexHandle, 0));
    visited[centerVertexHandle.idx()] = true;

    Mesh::VertexHandle tempVertexHandle;
    int tempDistance;
    while(!queue.empty())
    {
        tempVertexHandle = queue.front().first;
        tempDistance = queue.front().second;
        queue.pop_front();
        if(tempDistance == k)
        {
            ringVertexHandles.push_back(tempVertexHandle);
        }
        if(tempDistance < k)
        {
            for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(tempVertexHandle); vertexVertexIter.is_valid(); vertexVertexIter++)
            {
                if(!visited[vertexVertexIter->idx()])
                {
                    queue.push_back(pair<Mesh::VertexHandle, int>(*vertexVertexIter, tempDistance + 1));
                    visited[vertexVertexIter->idx()] = true;
                }
            }
        }
    }

    free(visited);
}

/*void ToothSegmentation::connectBoundary(const int k)
{
    QVector<Mesh::VertexHandle> kthRingVertexHandles;
    QVector<Mesh::VertexHandle> kRingVertexHandles;
    bool tempIsBoundary1, tempIsBoundary2;
    int changeTimes;
    int nonBoundaryVertexIndex = 0;
    mProgress->setLabelText("Connecting boundary...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum - mBoundaryVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            continue;
        }
        mProgress->setValue(nonBoundaryVertexIndex);
        kthRingVertexHandles.clear();
        getKthRing(*vertexIter, k, kthRingVertexHandles);
        kthRingVertexHandles.push_back(kthRingVertexHandles.front());
        changeTimes = 0;
        for(int i= 0; i < kthRingVertexHandles.size() - 1; i++)
        {
            tempIsBoundary1 = mToothMesh.property(mVPropHandleIsToothBoundary, kthRingVertexHandles.at(i + 1));
            tempIsBoundary2 = mToothMesh.property(mVPropHandleIsToothBoundary, kthRingVertexHandles.at(i));
            if(tempIsBoundary1 != tempIsBoundary2)
            {
                changeTimes++;
            }
        }
        if(changeTimes >= 4) //将该圆内所有顶点设置为边界
        {
            kRingVertexHandles.clear();
            getKRing(*vertexIter, k, kRingVertexHandles);
            for(int i = 0; i< kRingVertexHandles.size(); i++)
            {
                mToothMesh.property(mVPropHandleIsToothBoundary, kRingVertexHandles.at(i)) = true;
                mBoundaryVertexNum++;
            }
        }
        nonBoundaryVertexIndex++;
    }
}*/

void ToothSegmentation::computeCurvatureHistogram()
{
    float curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    #define histNum 1000 //区间数量
    float step = (curvatureMax - curvatureMin) / histNum; //区间长度

    int histCounts[histNum]; //各区间顶点数量
    for(int i = 0; i < histNum; i++)
    {
        histCounts[i] = 0;
    }

    float histPoints[histNum + 1]; //区间分解点值
    for(int i = 0; i < histNum; i++)
    {
        histPoints[i] = curvatureMin + step * i;
    }
    histPoints[histNum] = curvatureMax;

    QVector<Mesh::VertexHandle> histVertexs[histNum];
    int vertexIndex = 0;
    float tempCurvature;
    mProgress->setLabelText("Computing curvature histogram...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter))
        {
            continue;
        }
        tempCurvature = mToothMesh.property(mVPropHandleCurvature, *vertexIter);
        for(int i = 0; i < histNum; i++)
        {
            if(tempCurvature >= histPoints[i] && tempCurvature < histPoints[i + 1])
            {
                histCounts[i]++;
                histVertexs[i].push_back(*vertexIter);
                break;
            }
        }
        vertexIndex++;
    }

    //测试，输出曲率直方图数据
//    cout << "曲率直方图：" << endl;
//    for(int i = 0; i < histNum; i++)
//    {
//        cout << histCounts[i] << endl;
//    }

    //剔除负曲率奇异点
    int count = 0;
    int countThreshold = mToothMesh.mVertexNum * 0.001;
    for(int i = 0; i < histNum; i++)
    {
        count += histCounts[i];
        if(count > countThreshold)
        {
            break;
        }
        for(int j = 0; j < histVertexs[i].size(); j++)
        {
            mToothMesh.property(mVPropHandleCurvatureComputed, histVertexs[i].at(j)) = false;
        }
    }

    //剔除郑曲率奇异点
    count = 0;
    for(int i = histNum - 1; i >= 0 ; i--)
    {
        count += histCounts[i];
        if(count > countThreshold)
        {
            break;
        }
        for(int j = 0; j < histVertexs[i].size(); j++)
        {
            mToothMesh.property(mVPropHandleCurvatureComputed, histVertexs[i].at(j)) = false;
        }
    }
}

/*void ToothSegmentation::checkCustomMeshPropertiesExistence()
{
    if(!mToothMesh.get_property_handle(mVPropHandleCurvature, mVPropHandleCurvatureName)
            || !mToothMesh.get_property_handle(mVPropHandleCurvatureComputed, mVPropHandleCurvatureComputedName)
            || !mToothMesh.get_property_handle(mVPropHandleIsToothBoundary, mVPropHandleIsToothBoundaryName)
            || !mToothMesh.get_property_handle(mVPropHandleBoundaryVertexType, mVPropHandleBoundaryVertexTypeName)
            || !mToothMesh.get_property_handle(mVPropHandleNonBoundaryRegionType, mVPropHandleNonBoundaryRegionTypeName)
            || !mToothMesh.get_property_handle(mVPropHandleRegionGrowingVisited, mVPropHandleRegionGrowingVisitedName)
            || !mToothMesh.get_property_handle(mVPropHandleBoundaryType, mVPropHandleBoundaryTypeName)
            || !mToothMesh.get_property_handle(mVPropHandleSearchContourSectionVisited, mVPropHandleSearchContourSectionVisitedName))
    {
        throw runtime_error("There should be these properties in mToothMesh:\n"
                            + mVPropHandleCurvatureName + "\n"
                            + mVPropHandleCurvatureComputedName + "\n"
                            + mVPropHandleIsToothBoundaryName + "\n"
                            + mVPropHandleBoundaryVertexTypeName + "\n"
                            + mVPropHandleNonBoundaryRegionTypeName + "\n"
                            + mVPropHandleRegionGrowingVisitedName + "\n"
                            + mVPropHandleBoundaryTypeName + "\n"
                            + mVPropHandleSearchContourSectionVisitedName);
    }
}

float ToothSegmentation::cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const
{
    float a = vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2];
    float b = (vector1[0] * vector1[0] + vector1[1] * vector1[1] + vector1[2] * vector1[2]) * (vector2[0] * vector2[0] + vector2[1] * vector2[1] + vector2[2] * vector2[2]);
    return a / sqrt(b);
}

float ToothSegmentation::cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const
{
    float a = cos(vector1, vector2);
    return sqrt(1 - a * a) / a;
}*/

inline Mesh::Point ToothSegmentation::screenCoordinate2Model3DCoordinate(const int screenX, const int screenY)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ; //屏幕二维坐标和深度坐标
    GLdouble posX, posY, posZ; //模型三维坐标
    glPushMatrix();
    GLfloat originalMatrix[16] = { 0 };
    glGetFloatv(GL_MODELVIEW_MATRIX, originalMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(originalMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glPopMatrix();
    winX = screenX;
    winY = viewport[3] - (float)screenY;
    glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    return Mesh::Point((float)posX, (float)posY, (float)posZ);
}

inline void ToothSegmentation::model3DCoordinate2ScreenCoordinate(const Mesh::Point modelPoint, int &screenX, int &screenY, float &depth)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble winX, winY, winZ; //屏幕二维坐标和深度坐标
    GLdouble posX, posY, posZ; //模型三维坐标
    glPushMatrix();
    GLfloat originalMatrix[16] = { 0 };
    glGetFloatv(GL_MODELVIEW_MATRIX, originalMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(originalMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glPopMatrix();
    posX = modelPoint[0];
    posY = modelPoint[1];
    posZ = modelPoint[2];
    gluProject(posX, posY, posZ, modelview, projection, viewport, &winX, &winY, &winZ);
#define round(x) ((int)((x) + 0.5))
    screenX = round(winX);
    screenY = viewport[3] - round(winY);
    glReadPixels(round(winX), round(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
#undef round(x)
    depth = (float)winZ;
}

inline float ToothSegmentation::distance(Mesh::Point point1, Mesh::Point point2)
{
    float x_ = point1[0] - point2[0];
    float y_ = point1[1] - point2[1];
    float z_ = point1[2] - point2[2];
    return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

inline bool ToothSegmentation::isVisiable(Mesh::VertexHandle vertexHandle)
{
    Mesh::Point originalVertex = mToothMesh.point(vertexHandle);
    int screenX, screenY;
    float depth;
    model3DCoordinate2ScreenCoordinate(originalVertex, screenX, screenY, depth);
    Mesh::Point projectPoint = screenCoordinate2Model3DCoordinate(screenX, screenY);
    if(distance(projectPoint, originalVertex) > (mToothMesh.BBox.size.x + mToothMesh.BBox.size.y + mToothMesh.BBox.size.z) / 300)
    {
        return false;
    }
    else
    {
        return true;
    }
}

QVector<Mesh::VertexHandle> ToothSegmentation::getSelectedVertices()
{
    //计算模型上所有顶点在屏幕上的2维坐标
    mProgress->setLabelText("Computing 2D position of all vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    int vertexIndex = 0;
    Mesh::Point tempVertex;
    int screenX, screenY;
    float depth;
    QVector<QPoint> meshVertices2DPos; //所有顶点在屏幕上的2维坐标
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        tempVertex = mToothMesh.point(*vertexIter);
        model3DCoordinate2ScreenCoordinate(tempVertex, screenX, screenY, depth);
        meshVertices2DPos.push_back(QPoint(screenX, screenY));
        vertexIndex++;
    }

    //对模型上的每个顶点，搜索其屏幕2维坐标到鼠标拖动轨迹的最近距离对应的鼠标位置（用以判断其是否被鼠标选中）
    QVector< QVector<int> > kNearestSearchResult = kNearestNeighbours(1, meshVertices2DPos, mMouseTrack);

    //寻找被画笔包围的顶点
    mProgress->setLabelText("Finding seleted vertices...");
    mProgress->setMinimum(0);
    mProgress->setMaximum(mToothMesh.mVertexNum);
    mProgress->setValue(0);
    vertexIndex = 0;
    QVector<Mesh::VertexHandle> selectedVertices;
    QPoint tempVertex2DPoint;
    QPoint tempMousePoint;
    int tempXX, tempYY;
    int rr = mCircleCursorRadius * mCircleCursorRadius;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mProgress->setValue(vertexIndex);
        tempVertex2DPoint = meshVertices2DPos.at(vertexIndex);
        tempMousePoint = mMouseTrack.at(kNearestSearchResult[vertexIndex][0]);

        tempXX = tempVertex2DPoint.x() - tempMousePoint.x();
        tempYY = tempVertex2DPoint.y() - tempMousePoint.y();
        if(tempXX * tempXX + tempYY * tempYY < rr)
        {
            selectedVertices.push_back(*vertexIter);
        }
        vertexIndex++;
    }

    //去掉不可见的点
    for(int i = 0; i< selectedVertices.size(); i++)
    {
        if(!isVisiable(selectedVertices.at(i)))
        {
            selectedVertices.remove(i);
            i--;
        }
    }

    return selectedVertices;
}

void ToothSegmentation::mousePressEventShowVertexAttributes(QMouseEvent *e)
{
    //只响应鼠标右键点击事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    //鼠标点击位置坐标
    int x = e->x();
    int y = e->y();

    //计算对应的三维坐标
    QVector<Mesh::Point> clickedPoint;
    clickedPoint.push_back(screenCoordinate2Model3DCoordinate(x, y));

    //判断点击的是模型上的哪个点（通过遍历所有模型上的所有点，找到距离点击位置最近的点，如果该点到点击位置的距离小于某个阈值，则认为该点即为点击位置）
    QVector< QVector<int> > searchResult = kNearestNeighbours(1, clickedPoint, mToothMeshVertices);
    Mesh::Point clickedVertex = mToothMeshVertices.at(searchResult[0][0]);
    Mesh::VertexHandle clickedVertexHandle = mToothMeshVertexHandles.at(searchResult[0][0]);
    if(distance(clickedPoint[0], clickedVertex) > (mToothMesh.BBox.size.x + mToothMesh.BBox.size.y + mToothMesh.BBox.size.z) / 300)
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked vertex not found!");
        return;
    }

    QMessageBox::information(mParentWidget, "Info",
                             QString("Clicked vertex found!\n \
    x: %1\n \
    y: %2\n \
    z: %3\n \
    Curvature: %4\n \
    CurvatureComputed: %5\n \
    IsToothBoundary: %6\n \
    BoundaryVertexType: %7\n \
    NonBoundaryRegionType: %8\n \
    RegionGrowingVisited: %9\n \
    BoundaryType: %10\n \
    SearchContourSectionVisited: %11")
    .arg(clickedVertex[0])
    .arg(clickedVertex[1])
    .arg(clickedVertex[2])
    .arg(mToothMesh.property(mVPropHandleCurvature, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleCurvatureComputed, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleIsToothBoundary, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleBoundaryVertexType, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleNonBoundaryRegionType, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleRegionGrowingVisited, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleBoundaryType, clickedVertexHandle))
    .arg(mToothMesh.property(mVPropHandleSearchContourSectionVisited, clickedVertexHandle)));
}

void ToothSegmentation::mousePressEventStartRecordMouseTrack(QMouseEvent *e)
{
    //只响应鼠标右键点击事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    mMouseTrack.clear();

    mMouseTrack.push_back(e->pos());
}

void ToothSegmentation::mouseMoveEventRecordMouseTrack(QMouseEvent *e)
{
    //只响应鼠标右键拖动事件
    if(!(e->buttons() & Qt::RightButton))
    {
        return;
    }

    //记录鼠标拖动轨迹
    mMouseTrack.push_back(e->pos());
}

void ToothSegmentation::mouseReleaseEventAddSelectedBoundaryVertex(QMouseEvent *e)
{
    //只响应鼠标右键释放事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    mProgress->setWindowTitle("Adding boundary vertex...");

    mMouseTrack.push_back(e->pos());

    QVector<Mesh::VertexHandle> selectedVertices = getSelectedVertices();

    mMouseTrack.clear();

    //将鼠标选中的点剔除为非边界点
    for(int i = 0; i< selectedVertices.size(); i++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, selectedVertices.at(i)))
        {
            continue;
        }
        mToothMesh.property(mVPropHandleIsToothBoundary, selectedVertices.at(i)) = true;
        mBoundaryVertexNum++;
    }

    markNonBoundaryRegion();
    paintBoundaryVertices();
    paintClassifiedNonBoundaryRegions();

    //关闭进度条
    mProgress->close();

    //更新显示
    SW::GLViewer *gv = ((MainWindow*)mParentWidget)->gv;
    gv->removeAllMeshes();
    gv->addMesh(mToothMesh);
    if(shouldShowExtraMesh())
    {
        gv->addMesh(mExtraMesh);
    }
    gv->updateGL();

    //保存新的mesh文件
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".ManualAddOrDeleteBoundaryVertex.off");

    onSaveHistory();
}

void ToothSegmentation::mouseReleaseEventDeleteSelectedBoundaryVertex(QMouseEvent *e)
{
    //只响应鼠标右键释放事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    mProgress->setWindowTitle("Deleting boundary vertex...");

    mMouseTrack.push_back(e->pos());

    QVector<Mesh::VertexHandle> selectedVertices = getSelectedVertices();

    mMouseTrack.clear();

    //将鼠标选中的点剔除为非边界点
    for(int i = 0; i< selectedVertices.size(); i++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, selectedVertices.at(i)))
        {
            continue;
        }
        mToothMesh.property(mVPropHandleIsToothBoundary, selectedVertices.at(i)) = false;
        mBoundaryVertexNum--;
    }

    markNonBoundaryRegion();
    paintBoundaryVertices();
    paintClassifiedNonBoundaryRegions();

    //关闭进度条
    mProgress->close();

    //更新显示
    SW::GLViewer *gv = ((MainWindow*)mParentWidget)->gv;
    gv->removeAllMeshes();
    gv->addMesh(mToothMesh);
    if(shouldShowExtraMesh())
    {
        gv->addMesh(mExtraMesh);
    }

    //保存新的mesh文件
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".ManualAddOrDeleteBoundaryVertex.off");

    onSaveHistory();
}

void ToothSegmentation::mousePressEventDeleteErrorToothRegion(QMouseEvent *e)
{
    //只响应鼠标右键点击事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    mProgress->setWindowTitle("Deleting error tooth region...");

    //鼠标点击位置坐标
    int x = e->x();
    int y = e->y();

    //计算对应的三维坐标
    QVector<Mesh::Point> clickedPoint;
    clickedPoint.push_back(screenCoordinate2Model3DCoordinate(x, y));

    //判断点击的是模型上的哪个点（通过遍历所有模型上的所有点，找到距离点击位置最近的点，如果该点到点击位置的距离小于某个阈值，则认为该点即为点击位置）
    QVector< QVector<int> > searchResult = kNearestNeighbours(1, clickedPoint, mToothMeshVertices);
    Mesh::Point clickedVertex = mToothMeshVertices.at(searchResult[0][0]);
    Mesh::VertexHandle clickedVertexHandle = mToothMeshVertexHandles.at(searchResult[0][0]);
    if(distance(clickedPoint[0], clickedVertex) > (mToothMesh.BBox.size.x + mToothMesh.BBox.size.y + mToothMesh.BBox.size.z) / 300)
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked vertex not found!");
        mProgress->close();
        return;
    }

    if(mToothMesh.property(mVPropHandleIsToothBoundary, clickedVertexHandle))
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked vertex is boundary vertex!");
        mProgress->close();
        return;
    }

    regionGrowing(clickedVertexHandle, TEMP_REGION); //将此区域所有点visited属性还原为false
    regionGrowing(clickedVertexHandle, FILL_BOUNDARY_REGION);

    paintBoundaryVertices();

    //关闭进度条
    mProgress->close();

    //更新显示
    SW::GLViewer *gv = ((MainWindow*)mParentWidget)->gv;
    gv->removeAllMeshes();
    gv->addMesh(mToothMesh);
    if(shouldShowExtraMesh())
    {
        gv->addMesh(mExtraMesh);
    }
    gv->updateGL();

    //保存新的mesh文件
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".ManualDeleteErrorToothRegion.off");

    onSaveHistory();
}

void ToothSegmentation::mousePressEventDeleteErrorContourSection(QMouseEvent *e)
{
    //只响应鼠标右键点击事件
    if(e->button() != Qt::RightButton)
    {
        return;
    }

    mProgress->setWindowTitle("Deleting error contour section...");

    mMouseTrack.clear();
    mMouseTrack.push_back(e->pos());

    QVector<Mesh::VertexHandle> selectedVertices = getSelectedVertices();

    //检查选中的边界点是否包含cutting point或joint point，若有，则报错
    QVector<int> selectedContourSectionIndex; //记录被选中的contour section的index（如果选中了多个，则全部记录）
    int tempBoundaryType;
    Mesh::VertexHandle tempSelectedVertexHandle;
    Mesh::VertexHandle tempContourSectionVertexHandle;
    for(int i = 0; i< selectedVertices.size(); i++)
    {
        tempSelectedVertexHandle = selectedVertices.at(i);
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, tempSelectedVertexHandle))
        {
            continue;
        }
        tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, tempSelectedVertexHandle);
        if(tempBoundaryType == CUTTING_POINT || tempBoundaryType == JOINT_POINT)
        {
            QMessageBox::information(mParentWidget, "Error", "Clicked contour vertex should not be cutting point or joint point!");
            mProgress->close();
            return;
        }
        if(tempBoundaryType == TOOTH_GINGIVA_BOUNDARY)
        {
            QMessageBox::information(mParentWidget, "Error", "Clicked contour vertex should not be tooth-gingiva boundary!");
            mProgress->close();
            return;
        }
        if(tempBoundaryType == TOOTH_TOOTH_BOUNDARY)
        {
            //判断该点属于哪个contour section
            bool selectedContourSectionFound = false;
            for(int contourSectionIndex = 0; contourSectionIndex < mContourSections.size(); contourSectionIndex++)
            {
                for(int contourSectionVertexIndex = 0; contourSectionVertexIndex < mContourSections[contourSectionIndex].size(); contourSectionVertexIndex++)
                {
                    tempContourSectionVertexHandle = mContourSections[contourSectionIndex].at(contourSectionVertexIndex);
                    if(mToothMesh.property(mVPropHandleBoundaryType, tempContourSectionVertexHandle) != TOOTH_TOOTH_BOUNDARY)
                    {
                        continue;
                    }
                    if(tempContourSectionVertexHandle == tempSelectedVertexHandle)
                    {
                        selectedContourSectionFound = true;
                    }
                }
                if(selectedContourSectionFound)
                {
                    //如果没有添加过该contourSectionIndex，则将其添加到selectedContourSectionIndex
                    if(!selectedContourSectionIndex.contains(contourSectionIndex))
                    {
                        selectedContourSectionIndex.push_back(contourSectionIndex);
                    }
                    break;
                }
            }
        }
    }

    //如果选中了两条或以上contour section，则报错
    if(selectedContourSectionIndex.size() != 1)
    {
        QMessageBox::information(mParentWidget, "Error", "Should not select more than one contour sections!");
        mProgress->close();
        return;
    }

    /*//鼠标点击位置坐标
    int x = e->x();
    int y = e->y();

    //计算对应的三维坐标
    QVector<Mesh::Point> clickedPoint;
    clickedPoint.push_back(screenCoordinate2Model3DCoordinate(x, y));

    //判断点击的是模型上的哪个点（通过遍历所有模型上的所有点，找到距离点击位置最近的点，如果该点到点击位置的距离小于某个阈值，则认为该点即为点击位置）
    QVector< QVector<int> > searchResult = kNearestNeighbours(1, clickedPoint, mToothMeshVertices);
    Mesh::Point clickedVertex = mToothMeshVertices.at(searchResult[0][0]);
    Mesh::VertexHandle clickedVertexHandle = mToothMeshVertexHandles.at(searchResult[0][0]);
    if(distance(clickedPoint[0], clickedVertex) > (mToothMesh.BBox.size.x + mToothMesh.BBox.size.y + mToothMesh.BBox.size.z) / 300)
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked vertex not found!");
        return;
    }

    //判断点击的轮廓点，之后将删除此轮廓点所属的轮廓段
    Mesh::VertexHandle clickedContourVertexHandle; //点击的轮廓点
    if(mToothMesh.property(mVPropHandleIsToothBoundary, clickedVertexHandle)) //如果点击的顶点正好是边界点，则此点即为要找的“点击的轮廓点”
    {
        clickedContourVertexHandle = clickedVertexHandle;
    }
    else //否则，在点击的点的邻域内搜索边界点作为“点击的轮廓点”
    {
        int r = 2; //笔宽度（半径）
        QVector<Mesh::VertexHandle> kRingVertexHandles;
        getKRing(clickedVertexHandle, r, kRingVertexHandles);
        bool clickedContourVertexFound = false;
        for(int i = 0; i< kRingVertexHandles.size(); i++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, kRingVertexHandles.at(i)))
            {
                clickedContourVertexHandle = kRingVertexHandles.at(i);
                clickedContourVertexFound = true;
                break;
            }
        }
        if(!clickedContourVertexFound)
        {
            QMessageBox::information(mParentWidget, "Error", "Clicked contour vertex not found!");
            return;
        }
    }

    //如果点击的是两条轮廓段的交点，或者是牙齿与牙龈的边界点（不允许删除此类轮廓），则报错
    tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, clickedContourVertexHandle);
    if(tempBoundaryType == CUTTING_POINT || tempBoundaryType == JOINT_POINT)
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked contour vertex should not be cutting point or joint point!");
        return;
    }
    if(tempBoundaryType == TOOTH_GINGIVA_BOUNDARY)
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked contour vertex should not be tooth-gingiva boundary!");
        return;
    }

    //判断“点击的轮廓点”属于哪个轮廓段
    Mesh::VertexHandle tempVertexHandle;
    int clickedContourSection;
    bool clickedContourSectionFound = false;
    for(int contourSectionIndex = 0; contourSectionIndex < mContourSections.size(); contourSectionIndex++)
    {
        for(int contourSectionVertexIndex = 0; contourSectionVertexIndex < mContourSections[contourSectionIndex].size(); contourSectionVertexIndex++)
        {
            tempVertexHandle = mContourSections[contourSectionIndex].at(contourSectionVertexIndex);
            tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, tempVertexHandle);
            if(tempBoundaryType == CUTTING_POINT || tempBoundaryType == JOINT_POINT)
            {
                continue;
            }
            if(tempVertexHandle == clickedContourVertexHandle)
            {
                clickedContourSection = contourSectionIndex;
                clickedContourSectionFound = true;
            }
        }
        if(clickedContourSectionFound)
        {
            break;
        }
    }
    if(!clickedContourSectionFound) //TODO 正常不应该出现此情况
    {
        QMessageBox::information(mParentWidget, "Error", "Clicked contour section not found!");
        return;
    }*/

    int clickedContourSection = selectedContourSectionIndex.at(0);

    //将此轮廓段上除cutting point和joint point之外的所有点剔除为非边界点
    Mesh::VertexHandle tempVertexHandle;
    for(int contourSectionVertexIndex = 0; contourSectionVertexIndex < mContourSections[clickedContourSection].size(); contourSectionVertexIndex++)
    {
        tempVertexHandle = mContourSections[clickedContourSection].at(contourSectionVertexIndex);
        tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, tempVertexHandle);
        if(tempBoundaryType == CUTTING_POINT || tempBoundaryType == JOINT_POINT)
        {
            continue;
        }
        mToothMesh.property(mVPropHandleIsToothBoundary, tempVertexHandle) = false;
        mBoundaryVertexNum--;
    }

    //如果cutting point或joint point剔除为非边界点后，与其相连的剩下的轮廓段还是连通的，那么就真的将其剔除
    for(int contourSectionVertexIndex = 0; contourSectionVertexIndex < mContourSections[clickedContourSection].size(); contourSectionVertexIndex += mContourSections[clickedContourSection].size() - 1) //只考虑首尾两个点
    {
        tempVertexHandle = mContourSections[clickedContourSection].at(contourSectionVertexIndex);
        tempBoundaryType = mToothMesh.property(mVPropHandleBoundaryType, tempVertexHandle);
        if(!(tempBoundaryType == CUTTING_POINT || tempBoundaryType == JOINT_POINT))
        {
            continue;
        }

        int neighborVertexTypeChangeTimes = 0; //邻域点是否属于边界点这个属性改变（从边界点到非边界点或从非边界点到边界点）的次数
        int neighborBoundaryVertexNum = 0; //邻域中边界点数量
        Mesh::VertexVertexIter tempVvIterBegin = mToothMesh.vv_iter(tempVertexHandle);
        for(Mesh::VertexVertexIter vertexVertexIter = tempVvIterBegin; vertexVertexIter.is_valid(); )
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter) != mToothMesh.property(mVPropHandleIsToothBoundary, *((++vertexVertexIter).is_valid() ? vertexVertexIter : tempVvIterBegin)))
            {
                neighborVertexTypeChangeTimes++;
            }
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborBoundaryVertexNum++;
            }
        }

        if(neighborVertexTypeChangeTimes <= 2) //剔除后连通
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, tempVertexHandle) = false;
            mBoundaryVertexNum--;
        }
    }

    //重新标记区域
    markNonBoundaryRegion();

    //重新判断cutting point和joint point
    findCuttingPoints();

    paintAllVerticesWhite();
    paintClassifiedBoundary();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".ManualDeleteErrorContourSection.off");
    paintClassifiedNonBoundaryRegions();
    saveToothMesh(mToothMesh.MeshName.toStdString() + ".ManualDeleteErrorContourSection.WithRegionGrowing.off");

    //关闭进度条
    mProgress->close();

    //更新显示
    SW::GLViewer *gv = ((MainWindow*)mParentWidget)->gv;
    gv->removeAllMeshes();
    gv->addMesh(mToothMesh);
    if(shouldShowExtraMesh())
    {
        gv->addMesh(mExtraMesh);
    }
    gv->updateGL();

    onSaveHistory();
}

void ToothSegmentation::setCustomCursor(CursorType cursorType)
{
    mCursorType = cursorType;

    SW::GLViewer *gv = ((MainWindow*)mParentWidget)->gv;

    switch(cursorType)
    {
    case CURSOR_DEFAULT:
        gv->setCursor(Qt::ArrowCursor);
        break;
    case CURSOR_ADD_BOUNDARY_VERTEX:
    case CURSOR_DELETE_BOUNDARY_VERTEX:
    {
        int size = mCircleCursorRadius * 2 + 1;
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        QColor color = (cursorType == CURSOR_ADD_BOUNDARY_VERTEX) ? QColor(255, 0, 0, 128) : QColor(0, 0, 255, 128);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawEllipse(0, 0, size - 1, size - 1);
        painter.end();
        QCursor cursor(pixmap);
        gv->setCursor(cursor);
        break;
    }
    case CURSOR_DELETE_ERROR_CONTOUR_SECTION:
    {
        int size = mCircleCursorRadius * 2 + 1;
        int penWidth = mCircleCursorRadius / 10;
        int halfPenWidth = (penWidth + 1) / 2;
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        QColor color = QColor(255, 0, 0, 128);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawEllipse(0, 0, size - 1, size - 1);
        color = QColor(255, 0, 0, 255);
        painter.setPen(QPen(color, penWidth));
        painter.setBrush(Qt::transparent);
        painter.drawEllipse(halfPenWidth, halfPenWidth, size - 1 - penWidth, size - 1 - penWidth);
        int l = mCircleCursorRadius - (mCircleCursorRadius - halfPenWidth) * 0.708;
        painter.drawLine(l, l, size - 1 - l, size - 1 - l);
        painter.drawLine(l, size - 1 - l, size - 1 - l, l);
        painter.end();
        QCursor cursor(pixmap);
        gv->setCursor(cursor);
        break;
    }
    }
}

void ToothSegmentation::keyPressEventChangeMouseRadius(QKeyEvent *e)
{
    if((e->modifiers() & Qt::ControlModifier)) //"Ctrl"
    {
        switch(e->key())
        {
        case Qt::Key_Plus: //"+"
        case Qt::Key_Up: //"上"
        case Qt::Key_Equal: //"=" (因为"+"在"="键的上方)
            if(mCursorType != CURSOR_ADD_BOUNDARY_VERTEX
                    && mCursorType != CURSOR_DELETE_BOUNDARY_VERTEX
                    && mCursorType != CURSOR_DELETE_ERROR_CONTOUR_SECTION)
            {
                break;
            }
            mCircleCursorRadius++;
            setCustomCursor(mCursorType);
            break;
        case Qt::Key_Minus: //"-"
        case Qt::Key_Down: //"下"
            if(mCursorType != CURSOR_ADD_BOUNDARY_VERTEX
                    && mCursorType != CURSOR_DELETE_BOUNDARY_VERTEX
                    && mCursorType != CURSOR_DELETE_ERROR_CONTOUR_SECTION)
            {
                break;
            }
            if(mCircleCursorRadius > 2) //最小半径为2
            {
                mCircleCursorRadius--;
            }
            setCustomCursor(mCursorType);
            break;
        }
    }
}

ToothSegmentation::ProgramScheduleValues ToothSegmentation::getProgramSchedule()
{
    return mProgramSchedule;
}

bool ToothSegmentation::shouldShowExtraMesh()
{
    return mShouldShowExtraMesh;
}
