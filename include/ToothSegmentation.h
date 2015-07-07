#ifndef TOOTHSEGMENTATION_H
#define TOOTHSEGMENTATION_H

#include "Mesh.h"

using namespace SW;

class ToothSegmentation
{

private:
    Mesh mToothMesh; //牙齿模型网格
    Mesh::Point mGingivaCuttingPlanePoint; //牙龈分割平面点
    Mesh::Normal mGingivaCuttingPlaneNormal; //牙龈分割平面法向量

public:
    ToothSegmentation(Mesh toothMesh);

    Mesh getToothMesh();

    //4.1. Identifying potential tooth boundary
    void identifyingPotentialToothBoundary();

    //4.2. Automatic cutting of gingiva
    void automaticCuttingOfGingiva();

/*private:
    //计算两个向量夹角的cos值
    float cos(const Mesh::Point &vector1, const Mesh::Point &vector2) const;

    //计算两个向量夹角的cot值
    float cot(const Mesh::Point &vector1, const Mesh::Point &vector2) const;*/

};

#endif // TOOTHSEGMENTATION_H
