#include "ToothSegmentation.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include <igl/read_triangle_mesh.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/principal_curvature.h>

#include <QProgressDialog>
#include <QTime>
#include <QMessageBox>
#include <QFile>

#include <math.h>

using namespace SW;
using namespace std;
using namespace Eigen;

const string ToothSegmentation::mVPropHandleCurvatureName = "vprop_curvature";
const string ToothSegmentation::mVPropHandleCurvatureComputedName = "vprop_curvature_computed";
const string ToothSegmentation::mVPropHandleIsToothBoundaryName = "vprop_is_tooth_boundary";
const string ToothSegmentation::mVPropHandleBoundaryVertexTypeName = "vprop_boundary_vertex_type";
const string ToothSegmentation::mVPropHandleNonBoundaryRegionTypeName = "vprop_non_boundary_region_type";
const string ToothSegmentation::mVPropHandleRegionGrowingVisitedName = "vprop_region_growing_visited";

ToothSegmentation::ToothSegmentation(const Mesh &toothMesh)
{
    setToothMesh(toothMesh);
}

ToothSegmentation::ToothSegmentation()
{

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
}

Mesh ToothSegmentation::getToothMesh() const
{
    return mToothMesh;
}

Mesh ToothSegmentation::getExtraMesh() const
{
    return mExtraMesh;
}

void ToothSegmentation::identifyPotentialToothBoundary(QWidget *parentWidget)
{
    checkCustomMeshPropertiesExistence();

    QProgressDialog progress(parentWidget);
    progress.setWindowTitle("Identify potential tooth boundary...");
    progress.setMinimumSize(400, 120);
    progress.setCancelButtonText("cancel");
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);

    //如果Mesh中没有顶点颜色这个属性，则添加之（为了标记边界点）
    if(!mToothMesh.has_vertex_colors())
    {
        mToothMesh.request_vertex_colors();
    }

    //计算顶点处曲率
    computeCurvature(parentWidget, progress);

    //如果之前将初始边界信息保存到文件，则从文件中读取之，否则重新计算
    string potentialBoundaryFileName = mToothMesh.MeshName.toStdString() + ".potentialboundary";
    QFile potentialBoundaryFile(potentialBoundaryFileName.c_str());
    if(potentialBoundaryFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(parentWidget, "Info", "Potential boundary file found!\nPotential boundary data will be loaded from it.");
        bool tempIsToothBoundary;
        for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
        {
            potentialBoundaryFile.read((char *)(&tempIsToothBoundary), sizeof(bool));
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = tempIsToothBoundary;
        }
        potentialBoundaryFile.close();
    }
    else
    {
        //计算曲率阈值
        double curvatureMin, curvatureMax;
        computeCurvatureMinAndMax(curvatureMin, curvatureMax);
        double curvatureThreshold = curvatureMin * 0.01; //TODO 经肉眼观察，对于模型36293X_Zhenkan_070404.obj，0.01这个值最合适。

        //根据曲率阈值判断初始边界点
        mBoundaryVertexNum = 0;
        int vertexIndex = 0;
        progress.setLabelText("Finding boundary by curvature...");
        progress.setMinimum(0);
        progress.setMaximum(mToothMesh.mVertexNum);
        progress.setValue(0);
        for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
        {
            progress.setValue(vertexIndex);
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

        QMessageBox::information(parentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(mBoundaryVertexNum).arg(mToothMesh.mVertexNum));

        //形态学操作
        dilateBoundary(progress);
        dilateBoundary(progress);
        dilateBoundary(progress);
//        corrodeBoundary(progress);
//        corrodeBoundary(progress);
        dilateBoundary(progress);
        dilateBoundary(progress);
        dilateBoundary(progress);

        QMessageBox::information(parentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(mBoundaryVertexNum).arg(mToothMesh.mVertexNum));

        //将初始边界点信息保存到文件
        if(potentialBoundaryFile.open(QIODevice::WriteOnly))
        {
            bool tempIsToothBoundary;
            for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
            {
                tempIsToothBoundary = mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter);
                potentialBoundaryFile.write((char *)(&tempIsToothBoundary), sizeof(bool));
            }
            potentialBoundaryFile.close();
        }
        else
        {
            cerr << "Fail to open file \"" << potentialBoundaryFileName << "\" ." << endl;
        }
    }

    //测试
    //removeBoundaryVertexOnGingiva(progress, mToothMesh.BBox.origin.y + mToothMesh.BBox.size.y * 0.5);

    //边界点着色
    paintBoundary(progress);

    //关闭进度条
    progress.setValue(mToothMesh.mVertexNum);

    //测试，将曲率值转换成伪彩色
    //curvature2PseudoColor();

}

void ToothSegmentation::computeCurvature(QWidget *parentWidget, QProgressDialog &progress)
{
    checkCustomMeshPropertiesExistence();

    QTime time;
    time.start();

    VectorXd H2(mToothMesh.mVertexNum); //平均曲率
    vector<bool> currentCurvatureComputed(mToothMesh.mVertexNum); //记录每个顶点是否被正确计算得到曲率

    //如果之前将曲率信息保存到文件，则从文件中读取之，否则重新计算
    string curvatureFileName = mToothMesh.MeshName.toStdString() + ".curvature";
    QFile curvatureFile(curvatureFileName.c_str());
    if(curvatureFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(parentWidget, "Info", "Curvature file found!\nCurvature data will be loaded from it.");
        time.start();
        bool tempCurvatureComputed;
        for(int vertexIndex = 0; vertexIndex < mToothMesh.mVertexNum; vertexIndex++)
        {
            curvatureFile.read((char *)(&(H2(vertexIndex))), sizeof(double));
            curvatureFile.read((char *)(&tempCurvatureComputed), sizeof(bool));
            currentCurvatureComputed[vertexIndex] = tempCurvatureComputed;
        }
        curvatureFile.close();
        cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "从二进制文件中读取曲率信息" << " ended." << endl;
    }
    else
    {
        //从Mesh类中提取用于libIGL库计算的Eigen格式的网格顶点列表和面片列表
        MatrixXd V(mToothMesh.mVertexNum, 3); //网格顶点列表
        MatrixXi F(mToothMesh.mFaceNum, 3); //网格三角片面列表
        Mesh::Point tempVertex;
        int vertexIndex = 0, vertexCoordinateIndex;
        progress.setLabelText("Extracting vertex list...");
        progress.setMinimum(0);
        progress.setMaximum(mToothMesh.mVertexNum);
        progress.setValue(0);
        for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
        {
            progress.setValue(vertexIndex);
            tempVertex = mToothMesh.point(*vertexIter);
            for(vertexCoordinateIndex = 0; vertexCoordinateIndex < 3; vertexCoordinateIndex++)
            {
                V(vertexIndex, vertexCoordinateIndex) = tempVertex[vertexCoordinateIndex];
            }
            vertexIndex++;
        }
        //progress.setValue(mToothMesh.mVertexNum); //注释此行是为了使本次进度条不关闭而直接显示下一个进度条
        cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "提取网格顶点列表" << " ended." << endl;
        int faceIndex = 0, faceVertexIndex;
        progress.setLabelText("Extracting face list...");
        progress.setMinimum(0);
        progress.setMaximum(mToothMesh.mFaceNum);
        progress.setValue(0);
        for(Mesh::FaceIter faceIter = mToothMesh.faces_begin(); faceIter != mToothMesh.faces_end(); faceIter++)
        {
            progress.setValue(faceIndex);
            faceVertexIndex = 0;
            for(Mesh::FaceVertexIter faceVertexIter = mToothMesh.fv_iter(*faceIter); faceVertexIter.is_valid(); faceVertexIter++)
            {
                F(faceIndex, faceVertexIndex) = faceVertexIter->idx();
                faceVertexIndex++;
            }
            faceIndex++;
        }
        //progress.setValue(mToothMesh.mFaceNum);
        cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "提取网格顶点列表" << " ended." << endl;

        //带进度条的计算平均曲率
        MatrixXd PD1, PD2;
        VectorXd PV1, PV2;
        progress.setLabelText("Computing curvature...");
        progress.setMinimum(0);
        progress.setMaximum(mToothMesh.mVertexNum);
        progress.setValue(0);
        igl::principal_curvature(V, F, PD1, PD2, PV1, PV2, currentCurvatureComputed, progress); // Compute curvature directions via quadric fitting
        H2 = 0.5 * (PV1 + PV2); // mean curvature
        //progress.setValue(mToothMesh.mVertexNum);
        cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "计算平均曲率" << " ended." << endl;

        //显示计算曲率出错点数量
        int curvatureComputeFailedNum = 0;
        for(vertexIndex = 0; vertexIndex < mToothMesh.mVertexNum; vertexIndex++)
        {
            if(!currentCurvatureComputed[vertexIndex])
            {
                curvatureComputeFailedNum++;
            }
        }
        QMessageBox::information(parentWidget, "Info", QString("Compute curvature finished!\n%1 / %2 vertices failed.").arg(curvatureComputeFailedNum).arg(mToothMesh.mVertexNum));

        //将曲率信息保存到二进制文件
        if(curvatureFile.open(QIODevice::WriteOnly))
        {
            bool tempCurvatureComputed;
            for(int vertexIndex = 0; vertexIndex < mToothMesh.mVertexNum; vertexIndex++)
            {
                curvatureFile.write((char *)(&(H2(vertexIndex))), sizeof(double));
                tempCurvatureComputed = currentCurvatureComputed[vertexIndex];
                curvatureFile.write((char *)(&tempCurvatureComputed), sizeof(bool));
            }
            curvatureFile.close();
        }
        else
        {
            cerr << "Fail to open file \"" << curvatureFileName << "\" ." << endl;
        }
        cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "将曲率信息保存到二进制文件" << " ended." << endl;
    }

    //将计算得到的曲率信息写入到Mesh
    int vertexIndex = 0;
    progress.setLabelText("Adding curvature to mesh...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter) = currentCurvatureComputed[vertexIndex];
        mToothMesh.property(mVPropHandleCurvature, *vertexIter) = H2(vertexIndex); //可通过curvature_computed判断该顶点处曲率是否已被正确计算
        vertexIndex++;
    }
    //progress.setValue(mToothMesh.mVertexNum);
    cout << "Time elapsed " << time.elapsed() / 1000 << "s. " << "将曲率信息写入到Mesh" << " ended." << endl;
}

void ToothSegmentation::curvature2PseudoColor()
{
    checkCustomMeshPropertiesExistence();

    //计算曲率的最大值和最小值
    double curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    cout << "curvatureMin: " << curvatureMin << ", curvatureMax: " << curvatureMax << endl;

    //计算对应的伪彩色
    curvatureMin /= 8.0; curvatureMax /= 8.0;
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
            if(colorGray < 0.0) colorGray = 0.0; if(colorGray > 1.0) colorGray = 1.0;
            if(colorGray < 0.25)
            {
                colorPseudoRGB[0] = 0.0;
                colorPseudoRGB[1] = colorGray * 4.0;
                colorPseudoRGB[2] = 1.0;
            }
            else if(colorGray < 0.5)
            {
                colorPseudoRGB[0] = 0.0;
                colorPseudoRGB[1] = 1.0;
                colorPseudoRGB[2] = 1.0 - (colorGray - 0.25) * 4.0;
            }
            else if(colorGray < 0.75)
            {
                colorPseudoRGB[0] = (colorGray - 0.5) * 4.0;
                colorPseudoRGB[1] = 1.0;
                colorPseudoRGB[2] = 0.0;
            }
            else
            {
                colorPseudoRGB[0] = 1.0;
                colorPseudoRGB[1] = 1.0 - (colorGray - 0.75) * 4.0;
                colorPseudoRGB[2] = 0.0;
            }
        }
        mToothMesh.set_color(*vertexIter, colorPseudoRGB);
    }
}

void ToothSegmentation::computeCurvatureMinAndMax(double &curvatureMin, double &curvatureMax)
{
    checkCustomMeshPropertiesExistence();

    curvatureMin = 1000000.0; //TODO 初始化最小值为某个足够大的值（因为第一个顶点不确定是否被正确计算出曲率）
    curvatureMax = -1000000.0;
    double tempCurvature;
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

void ToothSegmentation::checkCustomMeshPropertiesExistence()
{
    if(!mToothMesh.get_property_handle(mVPropHandleCurvature, mVPropHandleCurvatureName)
            || !mToothMesh.get_property_handle(mVPropHandleCurvatureComputed, mVPropHandleCurvatureComputedName)
            || !mToothMesh.get_property_handle(mVPropHandleIsToothBoundary, mVPropHandleIsToothBoundaryName)
            || !mToothMesh.get_property_handle(mVPropHandleBoundaryVertexType, mVPropHandleBoundaryVertexTypeName)
            || !mToothMesh.get_property_handle(mVPropHandleNonBoundaryRegionType, mVPropHandleNonBoundaryRegionTypeName)
            || !mToothMesh.get_property_handle(mVPropHandleRegionGrowingVisited, mVPropHandleRegionGrowingVisitedName))
    {
        throw runtime_error("There should be these properties in mToothMesh:\n"
                            + mVPropHandleCurvatureName + "\n"
                            + mVPropHandleCurvatureComputedName + "\n"
                            + mVPropHandleIsToothBoundaryName + "\n"
                            + mVPropHandleBoundaryVertexTypeName + "\n"
                            + mVPropHandleNonBoundaryRegionTypeName + "\n"
                            + mVPropHandleRegionGrowingVisitedName);
    }
}

void ToothSegmentation::corrodeBoundary(QProgressDialog &progress)
{
    int neighborNotBoundaryVertexNum; //邻域中非边界点的个数
    int boundaryVertexIndex = 0;
    bool *boundaryVertexEliminated = new bool[mBoundaryVertexNum]; //标记对应边界点是否应被剔除
    progress.setLabelText("Corroding boundary...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum * 2);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点（包括未被正确计算出曲率的点，因为在上一步根据曲率阈值确定初始边界的过程中，未被正确计算出曲率的点全部被标记为非初始边界点）
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
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
        boundaryVertexEliminated[boundaryVertexIndex] = (neighborNotBoundaryVertexNum > 2);
        boundaryVertexIndex++;
    }
    boundaryVertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点（包括未被正确计算出曲率的点，因为在上一步根据曲率阈值确定初始边界的过程中，未被正确计算出曲率的点全部被标记为非初始边界点）
        {
            continue;
        }
        progress.setValue(mBoundaryVertexNum + boundaryVertexIndex);
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

void ToothSegmentation::dilateBoundary(QProgressDialog &progress)
{
    int neighborBoundaryVertexNum; //邻域中边界点的个数
    int notBoundaryVertexIndex = 0;
    bool *boundaryVertexAdded = new bool[mToothMesh.mVertexNum - mBoundaryVertexNum]; //标记对应非边界点是否应被添加为边界点
    progress.setLabelText("Dilating boundary...");
    progress.setMinimum(0);
    progress.setMaximum((mToothMesh.mVertexNum - mBoundaryVertexNum) * 2);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过初始边界点
        {
            continue;
        }
        progress.setValue(notBoundaryVertexIndex);
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
        boundaryVertexAdded[notBoundaryVertexIndex] = (neighborBoundaryVertexNum > 2);
        notBoundaryVertexIndex++;
    }
    notBoundaryVertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过初始边界点
        {
            continue;
        }
        progress.setValue((mToothMesh.mVertexNum - mBoundaryVertexNum) + notBoundaryVertexIndex);
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

void ToothSegmentation::paintBoundary(QProgressDialog &progress)
{
    int vertexIndex = 0;
    Mesh::Color colorRed(1.0, 0.0, 0.0), colorWhite(1.0, 1.0, 1.0);
    progress.setLabelText("Painting boundary vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            mToothMesh.set_color(*vertexIter, colorRed);
        }
        else
        {
            mToothMesh.set_color(*vertexIter, colorWhite);
        }
        vertexIndex++;
    }
}

void ToothSegmentation::automaticCuttingOfGingiva(QWidget *parentWidget)
{
    QProgressDialog progress(parentWidget);
    progress.setWindowTitle("Automatic cutting Of gingiva...");
    progress.setMinimumSize(400, 120);
    progress.setCancelButtonText("cancel");
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);

    //测试，手动粗略去除牙龈上的边界点
    //removeBoundaryVertexOnGingiva(progress, mToothMesh.BBox.origin.y + mToothMesh.BBox.size.y * 0.5);

    //计算初始边界点质心
    mGingivaCuttingPlanePoint = Mesh::Point(0.0, 0.0, 0.0); //质心点
    int boundaryVertexIndex = 0;
    float tempCurvature, curvatureSum = 0;
    progress.setLabelText("Computing center of boundary vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        tempCurvature = mToothMesh.property(mVPropHandleCurvature, *vertexIter);
        mGingivaCuttingPlanePoint += mToothMesh.point(*vertexIter) * tempCurvature; //将该点曲率作为加权
        curvatureSum += tempCurvature;
        boundaryVertexIndex++;
    }
    mGingivaCuttingPlanePoint /= curvatureSum;

    //测试，输出质心点
    cout << "质心点：\n" << mGingivaCuttingPlanePoint << endl;

    //计算协方差矩阵
    Matrix3f covarMat;
    covarMat << 0, 0, 0,
            0, 0, 0,
            0, 0, 0; //TODO 还可以用covarMat=MatrixXf::Zero(3, 3);或covarMat.setZero();实现初始化为0
    Matrix3f tempMat;
    Mesh::Point tempVertex;
    boundaryVertexIndex = 0;
    progress.setLabelText("Computing covariance matrix...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        tempVertex = mToothMesh.point(*vertexIter);
        tempVertex -= mGingivaCuttingPlanePoint;
        tempMat << tempVertex[0] * tempVertex[0], tempVertex[0] * tempVertex[1], tempVertex[0] * tempVertex[2],
                tempVertex[1] * tempVertex[0], tempVertex[1] * tempVertex[1], tempVertex[1] * tempVertex[2],
                tempVertex[2] * tempVertex[0], tempVertex[2] * tempVertex[1], tempVertex[2] * tempVertex[2];
        covarMat += tempMat;
        boundaryVertexIndex++;
    }
    covarMat /= mBoundaryVertexNum;

    //测试，输出协方差矩阵
    cout << "协方差矩阵：\n" << covarMat << endl;

    //计算分割平面法向量（即协方差矩阵对应最小特征值的特征向量）
    /*EigenSolver<Matrix3f> eigenSolver(covarMat, true);
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
    mGingivaCuttingPlaneNormal = Mesh::Normal(eigenvectors(eigenvalueMinIndex, 0).real(), eigenvectors(eigenvalueMinIndex, 1).real(), eigenvectors(eigenvalueMinIndex, 2).real());*/
    mGingivaCuttingPlaneNormal = Mesh::Normal(0.0, 1.0, 0.0); //TODO 假设的值

    //测试，手动调整（沿法向量方向平移）牙龈分割平面，TODO 需要区分牙齿上颚和下颚
    mGingivaCuttingPlanePoint += mGingivaCuttingPlaneNormal * 3;

    //计算牙龈分割平面（正方形）的4个顶点
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
    float gingivaCutPlaneSize = boundingBoxMaxEdgeLength * 1.5 / 1.414; //正方形中心到4个顶点的距离，TODO 1.5的意义是使画出来的分割平面比模型稍大一点
    float x0, y0, z0; //质心点（牙龈分割平面正方形中心点(记为点O)）
    x0 = mGingivaCuttingPlanePoint[0];
    y0 = mGingivaCuttingPlanePoint[1];
    z0 = mGingivaCuttingPlanePoint[2];
    float x1, y1, z1; //法向量
    x1 = mGingivaCuttingPlaneNormal[0];
    y1 = mGingivaCuttingPlaneNormal[1];
    z1 = mGingivaCuttingPlaneNormal[2];
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
    float scale1 = gingivaCutPlaneSize / sqrt((x2 - x0) * (x2 - x0) + y0 * y0 + z0 * z0); //向量ON乘以此倍数得到长度为gingivaCutPlaneSize的向量（从点O指向正方形其中一个顶点(记为点A)）
    float x3, y3, z3; //正方形其中一个顶点(点A)
    x3 = x0 + (x2 - x0) * scale1;
    y3 = y0 + (y2 - y0) * scale1;
    z3 = z0 + (z2 - z0) * scale1;
    float x4, y4, z4; //正方形中与上述顶点相邻的顶点(记为点B)
    float x5, y5, z5; //与向量OB同向的向量(记为向量OM)（由法向量和OA作叉积而得）
    x5 = y1 * (z3 - z0) - z1 * (y3 - y0);
    y5 = z1 * (x3 - x0) - x1 * (z3 - z0);
    z5 = x1 * (y3 - y0) - y1 * (x3 - x0);
    float scale2 = gingivaCutPlaneSize / sqrt(x5 * x5 + y5 * y5 + z5 * z5); //向量OM乘以此倍数得到向量OB
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
    std::vector<Mesh::VertexHandle> faceVertexhandles;
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
    mExtraMesh.set_normal(vertexHandles[0], mGingivaCuttingPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[1], mGingivaCuttingPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[2], mGingivaCuttingPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[3], mGingivaCuttingPlaneNormal);
    Mesh::FaceHandle faceHandle = *(mExtraMesh.vf_iter(vertexHandles[0]));
    if(!mExtraMesh.has_face_normals())
    {
        mExtraMesh.request_face_normals();
    }
    mExtraMesh.set_normal(faceHandle, mGingivaCuttingPlaneNormal);
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

    //剔除牙龈上的初始边界点
    removeBoundaryVertexOnGingiva(progress);

    //标记非边界区域
    markGingivaRegion(progress);

    //去除噪声区域
    removeNoiseRegion(progress);

    //非边界区域着色
    paintClassifiedNonBoundary(progress);

    //边界点重新着色
    //paintBoundary(progress);

    //关闭进度条
    progress.setValue(mToothMesh.mVertexNum);
}

void ToothSegmentation::boundarySkeletonExtraction(QWidget *parentWidget)
{
    checkCustomMeshPropertiesExistence();

    QProgressDialog progress(parentWidget);
    progress.setWindowTitle("Boundary skeleton extraction...");
    progress.setMinimumSize(400, 120);
    progress.setCancelButtonText("cancel");
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);

    //逐步删除某一类外围点
    int classifiedBoundaryVertexNum[4];
    int boundaryVertexIndex;
    static int iterTimes = 0; //迭代次数
    while(true)
    {
        iterTimes++;

        //边界点分类
        classifyBoundaryVertex(progress, classifiedBoundaryVertexNum);

        //测试
        //break;

        if(classifiedBoundaryVertexNum[0] == 0 && classifiedBoundaryVertexNum[1] == 0 && classifiedBoundaryVertexNum[2] == 0)
        {
            break;
        }
        //剔除某一类外围点
        boundaryVertexIndex = 0;
        progress.setLabelText(QString("Deleting disk vertices...\nNo.%1 times.\n%2 center vertices left.\n%3 disk vertices(to gingiva) left.\n%4 disk vertices(to tooth) left.").arg(iterTimes).arg(classifiedBoundaryVertexNum[0]).arg(classifiedBoundaryVertexNum[1]).arg(classifiedBoundaryVertexNum[2]));
        progress.setMinimum(0);
        progress.setMaximum(mBoundaryVertexNum);
        progress.setValue(0);
        for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
        {
            if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
            {
                continue;
            }
            progress.setValue(boundaryVertexIndex);
            if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) == (iterTimes % 2 == 0 ? DISK_VERTEX_1 : DISK_VERTEX_2))
            {
                mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
                mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = (iterTimes % 2 == 0 ? GINGIVA_REGION : TOOTH_REGION);
                if(iterTimes % 2 == 0) //将新出现的牙龈区域标记为已访问过，而新出现的非牙龈区域则不能，因为不确定是牙齿还是噪声
                {
                    mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = true;
                }
                mBoundaryVertexNum--;
            }
            boundaryVertexIndex++;
        }
    }

    //边界点分类着色
    paintClassifiedBoundary(progress);

    //非边界区域分类着色
    paintClassifiedNonBoundary(progress);

    //关闭进度条
    progress.setValue(mBoundaryVertexNum);
}

void ToothSegmentation::classifyBoundaryVertex(QProgressDialog &progress, int *classifiedBoundaryVertexNum)
{
    for(int i = 0; i < 4; i++)
    {
        classifiedBoundaryVertexNum[i] = 0;
    }

    int neighborVertexTypeChangeTimes; //某边界点邻域点是否属于边界点这个属性改变（从边界点到非边界点或从非边界点到边界点）的次数
    int neighborBoundaryVertexNum; //某边界点邻域中边界点数量
    Mesh::VertexVertexIter tempVvIterBegin; //由于在遍历邻域顶点时需要使用2个迭代器，因此保存初始邻域点
    int boundaryVertexIndex = 0;
    progress.setLabelText("Classifying boundary vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
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

        //如果为孤立点，则任意判断其为DISK_VERTEX_1或DISK_VERTEX_2（总之要被剔除）（注意：不能直接将其设置为非边界点，因为在删除边界点之后要更新其所属的非边界区域）
        if(neighborBoundaryVertexNum == 0)
        {
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_2;
            boundaryVertexIndex++;
            continue;
        }

        switch(neighborVertexTypeChangeTimes)
        {
        case 0: //TODO 如果邻域中只有1个边界点，应该判定为复杂点
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = CENTER_VERTEX;
            classifiedBoundaryVertexNum[0]++;
            break;
        case 2:
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_2;
            break;
        case 4:
        default:
            mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = COMPLEX_VERTEX;
            classifiedBoundaryVertexNum[3]++;
            break;
        }
        boundaryVertexIndex++;
    }

    //将外围点分成2类
    boundaryVertexIndex = 0;
    progress.setLabelText("Classifying boundary vertices(Disk vertices)...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        if(mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) != DISK_VERTEX_2) //跳过非外围点
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
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_1;
                classifiedBoundaryVertexNum[1]++;
                break;
            }
            if(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) == TOOTH_REGION)
            {
                mToothMesh.property(mVPropHandleBoundaryVertexType, *vertexIter) = DISK_VERTEX_2;
                classifiedBoundaryVertexNum[2]++;
                break;
            }
        }
        boundaryVertexIndex++;
    }
}

void ToothSegmentation::paintClassifiedBoundary(QProgressDialog &progress)
{
    int vertexIndex = 0;
    Mesh::Color colorWhite(1.0, 1.0, 1.0), colorGreen(0.0, 1.0, 0.0), colorBlue(0.0, 0.0, 1.0), colorYellow(1.0, 1.0, 0.0), colorRed(1.0, 0.0, 0.0);
    progress.setLabelText("Painting classified boundary vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
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
            case DISK_VERTEX_1:
                mToothMesh.set_color(*vertexIter, colorBlue);
                break;
            case DISK_VERTEX_2:
                mToothMesh.set_color(*vertexIter, colorYellow);
                break;
            case COMPLEX_VERTEX:
                mToothMesh.set_color(*vertexIter, colorRed);
                break;
            }
        }
        vertexIndex++;
    }
}

void ToothSegmentation::removeBoundaryVertexOnGingiva(QProgressDialog &progress)
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
    progress.setLabelText("Removing boundary vertices on gingiva...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        tempBoundaryVertex = mToothMesh.point(*vertexIter);
        //如果该初始边界点位于牙龈分割平面的上方（牙龈方向），则剔除此边界点，TODO 需要分别考虑牙齿上颚和下颚
        if(x1 * (tempBoundaryVertex[0] - x0) + y1 * (tempBoundaryVertex[1] - y0) + z1 * (tempBoundaryVertex[2] - z0) > 0)
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mBoundaryVertexNum--;
        }
        boundaryVertexIndex++;
    }
}

void ToothSegmentation::removeBoundaryVertexOnGingiva(QProgressDialog &progress, float y0)
{
    int boundaryVertexIndex = 0;
    Mesh::Point tempBoundaryVertex;
    progress.setLabelText("Removing boundary vertices on gingiva...");
    progress.setMinimum(0);
    progress.setMaximum(mBoundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        tempBoundaryVertex = mToothMesh.point(*vertexIter);
        //如果该初始边界点位于牙龈分割平面的上方（牙龈方向），则剔除此边界点，TODO 需要分别考虑牙齿上颚和下颚
        if(tempBoundaryVertex[1] > y0)
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mBoundaryVertexNum--;
        }
        boundaryVertexIndex++;
    }
}

void ToothSegmentation::markGingivaRegion(QProgressDialog &progress)
{
    //初始化所有非边界点的NonBoundaryRegionType属性为TOOTH_REGION，RegionGrowingVisited属性为false
    int vertexIndex = 0;
    progress.setLabelText("Init region type of all nonboundary vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) = TOOTH_REGION;
        mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = false;
        vertexIndex++;
    }

    //选择区域生长种子点
    Mesh::Point tempVertex;
    Mesh::VertexIter vertexIter;
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
        tempVertex = mToothMesh.point(*vertexIter);
        if(x1 * (tempVertex[0] - x0) + y1 * (tempVertex[1] - y0) + z1 * (tempVertex[2] - z0) > 0)
        {
            break;
        }
    }

    //区域生长
    regionGrowing(*vertexIter, GINGIVA_REGION);
}

int ToothSegmentation::regionGrowing(Mesh::VertexHandle seedVertexHandle, mNonBoundaryRegionType regionType)
{
    mToothMesh.property(mVPropHandleNonBoundaryRegionType, seedVertexHandle) = regionType;
    mToothMesh.property(mVPropHandleRegionGrowingVisited, seedVertexHandle) = true;

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
                continue;
            }
            mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexVertexIter) = true;
            mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexVertexIter) = regionType;
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

void ToothSegmentation::paintClassifiedNonBoundary(QProgressDialog &progress)
{
    int vertexIndex = 0;
    Mesh::Color colorBlue(0.0, 0.0, 1.0), colorGreen(0.0, 1.0, 0.0), colorWhite(1.0, 1.0, 1.0);
    progress.setLabelText("Painting classified nonboundary regions...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter))
        {
            //mToothMesh.set_color(*vertexIter, colorGreen);
        }
        else
        {
            switch(mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter))
            {
            case GINGIVA_REGION:
                mToothMesh.set_color(*vertexIter, colorBlue);
                break;
            case TOOTH_REGION:
                mToothMesh.set_color(*vertexIter, colorGreen);
                break;
            case TEMP_REGION:
            default:
                mToothMesh.set_color(*vertexIter, colorWhite);
                break;
            }
        }
        vertexIndex++;
    }
}

void ToothSegmentation::removeNoiseRegion(QProgressDialog &progress)
{
    //初始化所有点的RegionGrowingVisited属性（牙龈区域为true，其他为false）
    int vertexIndex = 0;
    progress.setLabelText("Init RegionGrowingVisited of all vertices...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) && mToothMesh.property(mVPropHandleNonBoundaryRegionType, *vertexIter) == GINGIVA_REGION)
        {
            mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = true;
        }
        else
        {
            mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter) = false;
        }
        vertexIndex++;
    }

    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) || mToothMesh.property(mVPropHandleRegionGrowingVisited, *vertexIter))
        {
            continue;
        }
        int regionVertexNum = regionGrowing(*vertexIter, TEMP_REGION);
        if(regionVertexNum < mToothMesh.mVertexNum * 0.001) //TODO 这个阈值是臆想的，但是达到了效果
        {
            regionGrowing(*vertexIter, GINGIVA_REGION);
        }
        else
        {
            regionGrowing(*vertexIter, TOOTH_REGION);
        }
    }
}

/*float ToothSegmentation::cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const
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
