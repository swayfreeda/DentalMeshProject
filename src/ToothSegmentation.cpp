#include "ToothSegmentation.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <igl/read_triangle_mesh.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/principal_curvature.h>

#include <QProgressDialog>
#include <QTime>
#include <QMessageBox>
#include <QFile>

using namespace SW;
using namespace std;
using namespace Eigen;

const string ToothSegmentation::mVPropHandleCurvatureName = "vprop_curvature";
const string ToothSegmentation::mVPropHandleCurvatureComputedName = "vprop_curvature_computed";
const string ToothSegmentation::mVPropHandleIsToothBoundaryName = "vprop_is_tooth_boundary";

ToothSegmentation::ToothSegmentation(Mesh toothMesh)
{
    //首先判断是否存在保存了曲率信息的mesh文件（后缀名为.om），如果存在则读取之，如果不存在或读取出错，则直接将输入参数toothMesh复制到成员变量mToothMesh
    /*string meshWithCurvatureFileName = toothMesh.MeshName.toStdString() + ".with_curvature.om";
    try
    {
        if(!OpenMesh::IO::read_mesh(mToothMesh, meshWithCurvatureFileName))
        {
            cout << "Failed to read Mesh(with curvature) from file: " + meshWithCurvatureFileName << endl;
            mToothMesh = toothMesh;
            mCurvatureComputed = false;
        }
        else
        {
            //测试，是否存在曲率信息（TODO 经测试，并没有，还未找到原因）
//            if(!mToothMesh.get_property_handle(mVPropHandleCurvature, mVPropHandleCurvatureName))
//            {
//                cerr << "没有曲率信息" << endl;
//            }

            mCurvatureComputed = true;
            mToothMesh.MeshName = toothMesh.MeshName;
            mToothMesh.computeEntityNumbers();
            mToothMesh.computeBoundingBox();
        }
    }
    catch(std::exception &x)
    {
        cout << "Failed to read Mesh(with curvature) from file: " + meshWithCurvatureFileName << endl;
        mToothMesh = toothMesh;
        mCurvatureComputed = false;
    }*/

    //初始化成员变量
    mToothMesh = toothMesh;

    //在Mesh添加自定义属性
    if(!mToothMesh.get_property_handle(mVPropHandleCurvature, mVPropHandleCurvatureName))
    {
        mToothMesh.add_property(mVPropHandleCurvature, mVPropHandleCurvatureName);
        /*mToothMesh.property(mVPropHandleCurvature).set_persistent(true); //为了在将Mesh保存到文件时可以同时保存curvature属性，需要调用set_persistent(true)，并且在上一行添加curvature属性时传入一个字符串参数作为参数名
        if(!mToothMesh.property(mVPropHandleCurvature).persistent())
        {
            cerr << "mToothMesh.property(mVPropHandleCurvature).set_persistent(true) failed." << endl;
        }*/
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

Mesh ToothSegmentation::getToothMesh()
{
    return mToothMesh;
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
    progress.setLabelText("Fining boundary by connectivity filtering...");
    progress.setMinimum(0);
    progress.setMaximum(mToothMesh.mVertexNum);
    progress.setValue(0);
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        progress.setValue(vertexIndex);
        if(!mToothMesh.property(mVPropHandleIsToothBoundary, *vertexIter)) //跳过非初始边界点
        {
            continue;
        }
        if(!mToothMesh.property(mVPropHandleCurvatureComputed, *vertexIter)) //跳过未被正确计算出曲率的顶点
        {
            continue;
        }
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
        vertexIndex++;
    }
    progress.setValue(mToothMesh.mVertexNum);

    QMessageBox::information(parentWidget, "Info", QString("Boundary vertices: %1\nAll vertices: %2").arg(boundaryVertexNum).arg(mToothMesh.mVertexNum));

    //测试，将曲率值转换成伪彩色
    //curvature2PseudoColor();

    QMessageBox::information(parentWidget, "Info", "Identify potential tooth boundary done!");
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

    //将Mesh（包括curvature属性）保存到文件
    /*string meshWithCurvatureFileName = mToothMesh.MeshName.toStdString() + ".with_curvature.om";
    if(!OpenMesh::IO::write_mesh(mToothMesh, meshWithCurvatureFileName))
    {
        QMessageBox::information(parentWidget, "Error", "Failed to save Mesh(with curvature) to file: " + QString(meshWithCurvatureFileName.c_str()));
    }*/

    //测试，读取保存的包含curvature属性的文件，检查是否有curvature属性（TODO 经测试，并没有，还未找到原因）
    /*Mesh testMesh;
    if(!OpenMesh::IO::read_mesh(testMesh, meshWithCurvatureFileName))
    {
        cout << "Failed to read Mesh(with curvature) from file: " + meshWithCurvatureFileName << endl;
    }
    else
    {
        double testCurvature = testMesh.property(mVPropHandleCurvature, *(testMesh.vertices_begin()));
    }*/
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
