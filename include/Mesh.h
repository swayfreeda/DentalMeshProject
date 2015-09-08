#ifndef MESH_H
#define MESH_H

//
//  BoundingBox.h
//  Reconstructor
//
//  Created by sway on 6/13/15.
//  Copyright (c) 2015 None. All rights reserved.
//

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "include/BoundingBox.h"

namespace SW{

//************************************************************//
//2015/09/07
//mhw merge code
//************************************************************//


class MPoint{
public:
    double x;
    double y;
    double z;
    //************************************************************//
    //编译过不去
    //************************************************************//
    //    MPoint(double x,double y):x(x),y(y),z(0){}
    //    MPoint(double x,double y,double z):x(x),y(y),z(z){}
    //    MPoint(const MPoint& p){
    //        x=p.x;
    //        y=p.y;
    //        z=p.z;
    //    }

};
struct MVector{
public:
    QVector<double> X_arr;
    QVector<double>Y_arr;
    QVector<double>Z_arr;

};

struct Halfedge_angle{
    double edge_length;
    double f_angle_halftan;
    double t_angle_halftan;
};
struct Angle_point{
    OpenMesh::VertexHandle Point;
    double halftan;
};

struct MyPoint{
    int index;
    double X;
    double Y;
    double Z;
};
//************************************************************//

struct MyTraits : public OpenMesh::DefaultTraits
{
    typedef OpenMesh::Vec3f Color; //使用浮点颜色值
};

class Mesh:public OpenMesh::TriMesh_ArrayKernelT<MyTraits>
{

    //  friend class  MainWindow;
public:
    Mesh(QString name);
    Mesh();

    void getBoundingBox();

    //计算BoundingBox
    void computeBoundingBox();

    //计算顶点数、面片数、边数
    void computeEntityNumbers();

    // 0--vertices 1-- wireframe 2-- flatLine
    // void draw(int flag); //mhw改201509079


private:
    //画OpenGL原点
    void drawOrigin();

    //画BoundingBox
    void drawBoundingBox();

public:
    int mVertexNum, mFaceNum, mEdgeNum;
    BoundingBox BBox;
    QString MeshName;

    //************************************************************//
    //2015/09/07
    //mhw merge code
    //************************************************************//
public:
    bool writeModel(std::string Write_path);
    bool readModel(std::string Mod_Path);
    void draw(int flag,QVector< QVector<int> > Select_P_Array, MVector MoveVectors);

    bool isSelectP(VertexHandle vh, QVector<QVector<int> > Select_P_Array, int *Belong_PS);

    double Gethalfedge_length(OpenMesh::HalfedgeHandle hh);
    OpenMesh::VertexHandle Getopposite_point(OpenMesh::HalfedgeHandle hh);
    double GetHtan_angleV(double Close_edgeA_length,double Close_edgeB_length,double Opposite_edge_length);
    void HandleFace_EA(OpenMesh::FaceHandle);
    std::vector<MyPoint>Get_limitP_fM(QVector<QVector<int> > Select_P_Array,MVector MoveVectors);
    bool is_limitP(std::vector<MyPoint> LMT_point,int indexN);
    std::vector<Halfedge_angle> Hedge_angle;

    //************************************************************//

};


}
#endif // MESH_H
