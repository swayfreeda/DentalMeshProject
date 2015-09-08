//
//mhw备注:
//
//
#ifndef  LAPLACETRANSFORM_H
#define LAPLACETRANSFORM_H

#include<stdlib.h>
#include<iostream>
#include"include/Mesh.h"
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include<vector>
#include <math.h>
#include<time.h>

namespace MHW
{

class LTransform{
    //    Q_OBJECT
public:    
    LTransform(SW::Mesh Mesh,std::string Mesh_Path_Save,int Ww,double Ff,int Npoint);

    int W;//系数;
    double F;//材料型变阻尼系数;
    int Mpoint;//矩阵点的个数;
    SW::Mesh objMesh;
    std::string Mesh_Save;
    std::vector<SW::MyPoint> LMT_point;
    int LMT_point_size;
    Eigen::SparseMatrix<double> spMat;//临接矩阵//构造拉普拉斯矩阵;
    Eigen::SparseMatrix<double> spMat_L;//拉普拉斯坐标系;
    Eigen::SparseMatrix<double> *spMat_C;//选择矩阵;
    Eigen::SparseMatrix<double> spMat_V;//原坐标系;
    //  Eigen::SparseMatrix<double> spMat_U;//控制点坐标矩阵;
    Eigen::SparseMatrix<double> spMat_F;//整体位置保持控制矩阵;
    std::vector<double> DMat;//对角矩阵;

    Eigen::MatrixXd  FC_spMat_A;//解方程AX=b的A项
    Eigen::MatrixXd   FC_spMat_B_N3;////解方程AX=b的项
    Eigen::LLT<Eigen::MatrixXd> lltOfA;//解方程用到的

    void Run(SW::Mesh &Mesh, QVector<QVector<int> > Select_P_Array, SW::MVector MoveVectors);
    void Create_spMat_L();//拉普拉斯坐标系;
    void Create_NewMatrix_V(SW::Mesh &Mesh,Eigen::SparseMatrix<double> &spMat_C,Eigen::SparseMatrix<double> &spMat_U,Eigen::SparseMatrix<double> &spMat_F);//输出新坐标系;
private:

    void Create_DMat();//整体位置保持控制矩阵;

    //*****************************
    //2015-07-03 TYPE=Notes
    //*****************************
    //构造函数调用
    //*****************************
    void Create_spMat_V();//原坐标系;
    void Create_spMat_U(Eigen::SparseMatrix<double> &spMat_U);//控制点坐标矩阵;
    void Create_spMat_C();//选择矩阵;
    void Create_spMat_F();//整体位置保持控制矩阵;
    void Create_spMat();//构建拉普拉斯系数矩阵;

};


class Do_LTransform{

public:
    LTransform* P;
    QVector<SW::Mesh> meshes;
    void startL(QVector<SW::Mesh> &M);

};
}
#endif // LAPLACETRANSFORM_H
