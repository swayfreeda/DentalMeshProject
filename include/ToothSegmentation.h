#ifndef TOOTHSEGMENTATION_H
#define TOOTHSEGMENTATION_H

#include "Mesh.h"

#include <QProgressDialog>

using namespace SW;
using namespace std;

class ToothSegmentation
{

public:
    enum mBoundaryVertexType
    {
        CENTER_VERTEX = 0, //中心点（边界内部点）
        COMPLEX_VERTEX, //复杂点（单点宽度边界点）
        DISK_VERTEX_GINGIVA, //外围点（边界外围点，与牙龈区域相邻）
        DISK_VERTEX_TOOTH //外围点（边界外围点，与第1颗牙齿区域相邻，后续逐个加1）
    };

    enum mNonBoundaryRegionType
    {
        TEMP_REGION = 0, //临时区域（用来判断噪声区域）
        GINGIVA_REGION, //牙龈区域
        TOOTH_REGION //牙齿区域（此为第1颗牙齿区域，后续逐个加1）
    };

private:
    Mesh mToothMesh; //牙齿模型网格
    Mesh mExtraMesh; //附加信息网格，用来显示牙龈分割平面等附加信息

    //Mesh自定义属性
    OpenMesh::VPropHandleT<double> mVPropHandleCurvature; //顶点处曲率
    static const string mVPropHandleCurvatureName; //保存带自定义属性的Mesh到文件时需要设置的对应属性的名称
    OpenMesh::VPropHandleT<bool> mVPropHandleCurvatureComputed; //该顶点处曲率是否被正确计算
    static const string mVPropHandleCurvatureComputedName;
    OpenMesh::VPropHandleT<bool> mVPropHandleIsToothBoundary; //该顶点是否是牙齿边界点
    static const string mVPropHandleIsToothBoundaryName;
    OpenMesh::VPropHandleT<int> mVPropHandleBoundaryVertexType; //该顶点属于的边界点类别
    static const string mVPropHandleBoundaryVertexTypeName;
    OpenMesh::VPropHandleT<int> mVPropHandleNonBoundaryRegionType; //该顶点属于的非边界区域类别
    static const string mVPropHandleNonBoundaryRegionTypeName;
    OpenMesh::VPropHandleT<bool> mVPropHandleRegionGrowingVisited; //该顶点在区域生长过程中是否已被访问过
    static const string mVPropHandleRegionGrowingVisitedName;

    int mBoundaryVertexNum; //牙齿边界点数量

    Mesh::Point mGingivaCuttingPlanePoint; //牙龈分割平面点
    Mesh::Normal mGingivaCuttingPlaneNormal; //牙龈分割平面法向量

    int mToothNum; //牙齿颗数

public:
    ToothSegmentation(const Mesh &toothMesh);

    ToothSegmentation();

    void setToothMesh(const Mesh &toothMesh);

    Mesh getToothMesh() const;

    Mesh getExtraMesh() const;

    //4.1. Identify potential tooth boundary
    void identifyPotentialToothBoundary(QWidget *parentWidget);

    //4.2. Automatic cutting of gingiva
    void automaticCuttingOfGingiva(QWidget *parentWidget);

    //4.3. Boundary skeleton extraction
    void boundarySkeletonExtraction(QWidget *parentWidget);

private:
    //计算曲率
    void computeCurvature(QWidget *parentWidget, QProgressDialog &progress);

    //将曲率转换成灰度，再转换成伪彩色，将伪彩色信息写入到顶点颜色属性
    void curvature2PseudoColor();

    //计算曲率最大值和最小值
    void computeCurvatureMinAndMax(double &curvatureMin, double &curvatureMax);

    //检查mToothMesh中是否存在curvature（顶点处的曲率）属性和curvature_computed（曲率是否被正确计算），如果不存在则报错
    void checkCustomMeshPropertiesExistence();

    //对边界区域进行1邻域腐蚀操作
    void corrodeBoundary(QProgressDialog &progress);

    //对边界区域进行1邻域膨胀操作
    void dilateBoundary(QProgressDialog &progress);

    //边界点着色
    void paintBoundary(QProgressDialog &progress);

    //边界点分类，classifiedBoundaryVertexNum返回各类边界点数量
    void classifyBoundaryVertex(QProgressDialog &progress, int *classifiedBoundaryVertexNum);

    //分类后的边界点着色
    void paintClassifiedBoundary(QProgressDialog &progress);

    //根据牙龈分割平面剔除牙龈上的初始边界点
    void removeBoundaryVertexOnGingiva(QProgressDialog &progress);

    //手动设置xz平面剔除牙龈上的初始边界点
    void removeBoundaryVertexOnGingiva(QProgressDialog &progress, float y0);

    //标记牙龈区域
    void markNonBoundaryRegion(QProgressDialog &progress);

    //区域生长，返回该区域的顶点数量，如果regionType为TEMP_REGION，则只计算该区域的顶点数量，visited属性不变为true，也就是说可以再次进行区域生长
    int regionGrowing(Mesh::VertexHandle vertexHandle, int regionType);

    //非边界区域分类着色
    void paintClassifiedNonBoundary(QProgressDialog &progress);

    //测试，保存模型（带颜色信息）
    void saveToothMesh(string filename);

    //灰度转伪彩色
    void gray2PseudoColor(float grayValue, Mesh::Color &pseudoColor);

/*private:
    //计算两个向量夹角的cos值
    float cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const;

    //计算两个向量夹角的cot值
    float cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const;*/

};

#endif // TOOTHSEGMENTATION_H
