#ifndef CURVATURECOMPUTER_H
#define CURVATURECOMPUTER_H

#include <iostream>
#include <vector>

#include <QProgressDialog>
#include <QVector>

#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <Eigen/SparseCholesky>

#include "Mesh.h"

using namespace SW;
using namespace std;

#define KRING

/*
  由IGL库中principal_ccurvature.cpp中的同名类修改而来。
  其中searchType固定为K_RING_SEARCH，normalType固定为AVERAGE。
  原类使用std::vector存储mesh数据，现改用OpenMesh半边数据结构，并加入OpenMP并行计算。
*/
class CurvatureComputer : public QObject
{
    Q_OBJECT

private:
    class Quadric
    {
    private:
        float data[5];

    public:
        inline Quadric ()
        {
            a() = b() = c() = d() = e() = 1.0;
        }

        inline Quadric(float av, float bv, float cv, float dv, float ev)
        {
            a() = av;
            b() = bv;
            c() = cv;
            d() = dv;
            e() = ev;
        }

        inline float& a()
        {
            return data[0];
        }
        inline float& b()
        {
            return data[1];
        }
        inline float& c()
        {
            return data[2];
        }
        inline float& d()
        {
            return data[3];
        }
        inline float& e()
        {
            return data[4];
        }

        inline float evaluate(float u, float v)
        {
            return a()*u*u + b()*u*v + c()*v*v + d()*u + e()*v;
        }

        inline float du(float u, float v)
        {
            return 2.0*a()*u + b()*v + d();
        }

        inline float dv(float u, float v)
        {
            return 2.0*c()*v + b()*u + e();
        }

        inline float duv(float u, float v)
        {
            return b();
        }

        inline float duu(float u, float v)
        {
            return 2.0*a();
        }

        inline float dvv(float u, float v)
        {
            return 2.0*c();
        }

        inline static Quadric fit(const QVector<Mesh::Point> &vv)
        {
            int vvSize = vv.size();
            assert(vvSize >= 5);

            Eigen::MatrixXf A(vvSize, 5);
            Eigen::MatrixXf b(vvSize, 1);
            Eigen::MatrixXf sol(5, 1);

            Mesh::Point tempPoint;
            int c = 0;
            for(QVector<Mesh::Point>::const_iterator vpi = vv.begin(); vpi != vv.end(); vpi++, c++)
            {
                tempPoint = *vpi;
                float u = tempPoint[0];
                float v = tempPoint[1];
                float n = tempPoint[2];

                A(c,0) = u*u;
                A(c,1) = u*v;
                A(c,2) = v*v;
                A(c,3) = u;
                A(c,4) = v;

                b(c) = n;
            }

            sol=A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

            return Quadric(sol(0), sol(1), sol(2), sol(3), sol(4));
        }
    };

    class comparer
    {
    public:
        inline bool operator() (const pair<Mesh::VertexHandle, float> &lhs, const pair<Mesh::VertexHandle, float> &rhs) const
        {
            return lhs.second > rhs.second;
        }
    };

    Mesh mMesh; //要计算曲率的Mesh

    int mKRing; //使用某顶点的mKRing邻域计算曲率
    float mSphere; //使用某顶点为圆心，mSphere为半径的球内顶点计算曲率
    bool mLocalMode; //使用该顶点处法向量(true)还是kRing邻域内法向量的平均值(false)
    bool mProjectionPlaneCheck; // Check collected vertices on tangent plane

    QVector<float> mCurvature;
    QVector<bool> mCurvatureComputed; //whether current vertex's curvature has been correctly computed

    class ThreadArguments
    {
    public:
        CurvatureComputer *curvatureComputer;
        QVector<Mesh::VertexHandle> *vertices;
        int startVertexIndex;
        int endVertexIndex;
        int *completedVertexNum;
    };

public:
    CurvatureComputer(Mesh &mesh);

    void computeCurvature(QProgressDialog *progress);

    void getResult(QVector<float> &curvature, QVector<bool> &computed);

private:
    static void* threadFun(void *arg);

    void computeCurvature(QVector<Mesh::VertexHandle> &vertices, int startVertexIndex, int endVertexIndex, int *completedVertexNum);

    inline void getKRing(const Mesh::VertexHandle &centerVertexHandle, const int k, QVector<Mesh::VertexHandle> &vv);

    inline void getSphere(const Mesh::VertexHandle &centerVertexHandle, const float r, const int min, QVector<Mesh::VertexHandle> &vv);

    inline float getAverageEdge(QProgressDialog *progress);

    inline void applyProjOnPlane(const Mesh::Normal &ppn, const QVector<Mesh::VertexHandle> &vin, QVector<Mesh::VertexHandle> &vout);

    inline void getAverageNormal(const Mesh::VertexHandle &centerVertexHandle, const QVector<Mesh::VertexHandle> &vv, Mesh::Normal &normal);

    inline void computeReferenceFrame(const Mesh::VertexHandle &centerVertexHandle, const Mesh::Normal &normal, QVector<Mesh::Point> &ref);

    inline Mesh::Point project(const Mesh::Point &v, const Mesh::Point &vp, const Mesh::Normal &ppn);

    inline void fitQuadric(const Mesh::Point &v, const QVector<Mesh::Point> &ref, const QVector<Mesh::VertexHandle> &vv, Quadric *q);

    inline float finalEigenStuff(Quadric &q);

signals:
    void progressValueChanged(int value);

};

#endif // CURVATURECOMPUTER_H
