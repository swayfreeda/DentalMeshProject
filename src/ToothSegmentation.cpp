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

    //计算顶点处曲率
    computeCurvature(parentWidget, progress);

    //如果Mesh中没有顶点颜色这个属性，则添加之（为了标记边界点）
    if(!mToothMesh.has_vertex_colors())
    {
        mToothMesh.request_vertex_colors();
    }

    //计算曲率阈值
    double curvatureMin, curvatureMax;
    computeCurvatureMinAndMax(curvatureMin, curvatureMax);
    double curvatureThreshold = curvatureMin * 0.01; //经肉眼观察，对于模型36293X_Zhenkan_070404.obj，0.01这个值最合适。

    //根据曲率阈值判断初始边界点
    int boundaryVertexNum = 0;
    int vertexIndex = 0;
    Mesh::Color colorRed(1.0, 0.0, 0.0), colorWhite(1.0, 1.0, 1.0);
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
            mToothMesh.set_color(*vertexIter, colorWhite);
            continue;
        }
        if(mToothMesh.property(mVPropHandleCurvature, *vertexIter) < curvatureThreshold) //如果该顶点处的曲率小于某个阈值，则确定为初始边界点
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = true;
            mToothMesh.set_color(*vertexIter, colorRed);
            boundaryVertexNum++;
        }
        else
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mToothMesh.set_color(*vertexIter, colorWhite);
        }
        vertexIndex++;
    }

    QMessageBox::information(parentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(boundaryVertexNum).arg(mToothMesh.mVertexNum));

    //连通性滤波
    vertexIndex = 0;
    int neighborBoundaryVertexNum; //邻域中边界点的个数
    int boundaryVertexIndex = 0;
    progress.setLabelText("Fining boundary by connectivity filtering...");
    progress.setMinimum(0);
    progress.setMaximum(boundaryVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点（包括未被正确计算出曲率的点，因为在上一步根据曲率阈值确定初始边界的过程中，未被正确计算出曲率的点全部被标记为非初始边界点）
        {
            continue;
        }
        progress.setValue(boundaryVertexIndex);
        //计算邻域中边界点的个数
        neighborBoundaryVertexNum = 0;
        for(Mesh::VertexVertexIter vertexVertexIter = mToothMesh.vv_iter(*vertexIter); vertexVertexIter.is_valid(); vertexVertexIter++)
        {
            if(mToothMesh.property(mVPropHandleIsToothBoundary, *vertexVertexIter))
            {
                neighborBoundaryVertexNum++;
            }
        }
        //邻域中边界点的个数小于4的剔除，否则保留
        if(neighborBoundaryVertexNum < 4)
        {
            mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter) = false;
            mToothMesh.set_color(*vertexIter, colorWhite);
            boundaryVertexNum--;
        }
        boundaryVertexIndex++;
    }
    progress.setValue(mToothMesh.mVertexNum);

    QMessageBox::information(parentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(boundaryVertexNum).arg(mToothMesh.mVertexNum));

    //记录牙齿边界点个数，方便以后使用
    mBoundaryVertexNum = boundaryVertexNum;

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
            cout << "Fail to open file \"" << curvatureFileName << "\" ." << endl;
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

    curvatureMin = 1000000.0; //初始化最小值为某个足够大的值（因为第一个顶点不确定是否被正确计算出曲率）
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
            || !mToothMesh.get_property_handle(mVPropHandleIsToothBoundary, mVPropHandleIsToothBoundaryName))
    {
        throw runtime_error("There should be these properties in mToothMesh:\n"
                            + mVPropHandleCurvatureName + "\n"
                            + mVPropHandleCurvatureComputedName + "\n"
                            + mVPropHandleIsToothBoundaryName);
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

    //计算初始边界点质心
    Mesh::Point boundaryVerticesCenter(0.0, 0.0, 0.0); //质心点
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
        boundaryVerticesCenter += mToothMesh.point(*vertexIter) * tempCurvature; //将该点曲率作为加权
        curvatureSum += tempCurvature;
        boundaryVertexIndex++;
    }
    //progress.setValue(mBoundaryVertexNum);
    boundaryVerticesCenter /= curvatureSum;

    //计算协方差矩阵
    Matrix3f covarMat;
    covarMat << 0, 0, 0,
            0, 0, 0,
            0, 0, 0;
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
        tempVertex -= boundaryVerticesCenter;
        tempMat << tempVertex[0] * tempVertex[0], tempVertex[0] * tempVertex[1], tempVertex[0] * tempVertex[2],
                tempVertex[1] * tempVertex[0], tempVertex[1] * tempVertex[1], tempVertex[1] * tempVertex[2],
                tempVertex[2] * tempVertex[0], tempVertex[2] * tempVertex[1], tempVertex[2] * tempVertex[2];
        covarMat += tempMat;
        boundaryVertexIndex++;
    }
    covarMat /= mBoundaryVertexNum;

    //测试，输出协方差矩阵
    cout << covarMat << endl;

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
    Mesh::Normal gingivaCutPlaneNormal(eigenvectors(eigenvalueMinIndex, 0).real(), eigenvectors(eigenvalueMinIndex, 1).real(), eigenvectors(eigenvalueMinIndex, 2).real());*/
    Mesh::Normal gingivaCutPlaneNormal(0.0, 1.0, 0.0); //假设的值

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
    float gingivaCutPlaneSize = boundingBoxMaxEdgeLength * 1.5 / 1.414; //正方形中心到4个顶点的距离
    float x0, y0, z0; //质心点（牙龈分割平面正方形中心点(记为点O)）
    x0 = boundaryVerticesCenter[0];
    y0 = boundaryVerticesCenter[1];
    z0 = boundaryVerticesCenter[2];
    float x1, y1, z1; //法向量
    x1 = gingivaCutPlaneNormal[0];
    y1 = gingivaCutPlaneNormal[1];
    z1 = gingivaCutPlaneNormal[2];
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
    cout << "牙龈分割平面正方形顶点：" << endl
         << "A: " << x3 << ", " << y3 << ", " << z3 << ", " << endl
         << "B: " << x4 << ", " << y4 << ", " << z4 << ", " << endl
         << "C: " << x6 << ", " << y6 << ", " << z6 << ", " << endl
         << "D: " << x7 << ", " << y7 << ", " << z7 << ", " << endl;

    //将分割平面添加到mExtraMesh中，以便显示
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
    mExtraMesh.set_normal(vertexHandles[0], gingivaCutPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[1], gingivaCutPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[2], gingivaCutPlaneNormal);
    mExtraMesh.set_normal(vertexHandles[3], gingivaCutPlaneNormal);
    Mesh::FaceHandle faceHandle = *(mExtraMesh.vf_iter(vertexHandles[0]));
    if(!mExtraMesh.has_face_normals())
    {
        mExtraMesh.request_face_normals();
    }
    mExtraMesh.set_normal(faceHandle, gingivaCutPlaneNormal);
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
