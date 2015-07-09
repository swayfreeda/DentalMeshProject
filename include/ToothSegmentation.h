#ifndef TOOTHSEGMENTATION_H
#define TOOTHSEGMENTATION_H

#include "Mesh.h"

#include <QProgressDialog>

using namespace SW;
using namespace std;

class ToothSegmentation
{

private:
    Mesh mToothMesh; //牙齿模型网格
    //bool mCurvatureComputed; //是否已经计算得到顶点处曲率信息

    //Mesh自定义属性
    OpenMesh::VPropHandleT<double> mVPropHandleCurvature; //顶点处曲率
    static const string mVPropHandleCurvatureName; //保存带自定义属性的Mesh到文件时需要设置的对应属性的名称
    OpenMesh::VPropHandleT<bool> mVPropHandleCurvatureComputed; //该顶点处曲率是否被正确计算
    static const string mVPropHandleCurvatureComputedName;
    OpenMesh::VPropHandleT<bool> mVPropHandleIsToothBoundary; //该顶点是否是牙齿边界点
    static const string mVPropHandleIsToothBoundaryName;

    Mesh::Point mGingivaCuttingPlanePoint; //牙龈分割平面点
    Mesh::Normal mGingivaCuttingPlaneNormal; //牙龈分割平面法向量

public:
    ToothSegmentation(Mesh toothMesh);

    Mesh getToothMesh();

    //4.1. Identifying potential tooth boundary
    void identifyPotentialToothBoundary(QWidget *parentWidget);

    //4.2. Automatic cutting of gingiva
    void automaticCuttingOfGingiva();

private:
    //计算曲率
    void computeCurvature(QWidget *parentWidget, QProgressDialog &progress);

    //将曲率转换成灰度，再转换成伪彩色，将伪彩色信息写入到顶点颜色属性
    void curvature2PseudoColor();

    //计算曲率最大值和最小值
    void computeCurvatureMinAndMax(double &curvatureMin, double &curvatureMax);

    //检查mToothMesh中是否存在curvature（顶点处的曲率）属性和curvature_computed（曲率是否被正确计算），如果不存在则报错
    void checkCustomMeshPropertiesExistence();

/*private:
    //计算两个向量夹角的cos值
    float cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const;

    //计算两个向量夹角的cot值
    float cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const;*/

};

#endif // TOOTHSEGMENTATION_H
