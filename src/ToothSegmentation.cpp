#include "ToothSegmentation.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <igl/read_triangle_mesh.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/principal_curvature.h>

using namespace SW;
using namespace std;
using namespace Eigen;

ToothSegmentation::ToothSegmentation(Mesh toothMesh)
{
    mToothMesh = toothMesh;
}

Mesh ToothSegmentation::getToothMesh()
{
    return mToothMesh;
}

void ToothSegmentation::identifyingPotentialToothBoundary()
{
    //OpenMesh学习测试
    /*mToothMesh.request_face_normals();
    mToothMesh.request_vertex_normals();
    mToothMesh.update_normals();
    Mesh::Normal tempNormal;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        tempNormal = mToothMesh.normal(*vertexIter);
        tempNormal *= 0.1;
        cout << "Vertex #" << *vertexIter << ": " << mToothMesh.point(*vertexIter);
        mToothMesh.set_point(*vertexIter, mToothMesh.point(*vertexIter) + tempNormal);
        cout << " moved to " << mToothMesh.point( *vertexIter );
        cout << ", Normal(*0.1): " << tempNormal << endl;
    }
    mToothMesh.release_vertex_normals();
    mToothMesh.release_face_normals();*/

    //在Mesh中添加一项参数：curvature（顶点处的曲率）
    OpenMesh::VPropHandleT<double> curvature;
    mToothMesh.add_property(curvature, "curvature");

    //计算网格顶点数和面片数
    int vertexNum =0;
    int faceNum = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        vertexNum++;
    }
    for(Mesh::FaceIter faceIter = mToothMesh.faces_begin(); faceIter != mToothMesh.faces_end(); faceIter++)
    {
        faceNum++;
    }

    //使用libIGL库从模型文件中读取网格顶点列表和面片列表
    /*MatrixXd V_igl; //网格顶点列表
    MatrixXi F_igl; //网格三角片面列表
    igl::read_triangle_mesh(mToothMesh.MeshName.toStdString(), V_igl, F_igl);*/

    //从Mesh类中提取用于libIGL库计算的Eigen格式的网格顶点列表和片面列表
    MatrixXd V(vertexNum, 3); //网格顶点列表
    MatrixXi F(faceNum, 3); //网格三角片面列表
    Mesh::Point tempVertex;
    int vertexIndex = 0, vertexCoordinateIndex;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        tempVertex = mToothMesh.point(*vertexIter);
        for(vertexCoordinateIndex = 0; vertexCoordinateIndex < 3; vertexCoordinateIndex++)
        {
            V(vertexIndex, vertexCoordinateIndex) = tempVertex[vertexCoordinateIndex];
        }
        vertexIndex++;
    }
    int faceIndex = 0, faceVertexIndex;
    for(Mesh::FaceIter faceIter = mToothMesh.faces_begin(); faceIter != mToothMesh.faces_end(); faceIter++)
    {
        faceVertexIndex = 0;
        for(Mesh::FaceVertexIter faceVertexIter = mToothMesh.fv_iter(*faceIter); faceVertexIter.is_valid(); faceVertexIter++)
        {
            F(faceIndex, faceVertexIndex) = faceVertexIter->idx();
            faceVertexIndex++;
        }
        faceIndex++;
    }

    //计算平均曲率方法1
    VectorXd H1(vertexNum); //平均曲率
    MatrixXd HN; // Alternative discrete mean curvature
    SparseMatrix<double> L, M, Minv;
    igl::cotmatrix(V, F, L);
    igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_VORONOI, M);
    igl::invert_diag(M, Minv);
    HN = -Minv * (L * V); // Laplace-Beltrami of position
    H1 = HN.rowwise().norm(); // Extract magnitude as mean curvature

    //计算平均曲率方法2
    VectorXd H2(vertexNum); //平均曲率
    MatrixXd PD1, PD2;
    VectorXd PV1, PV2;
    igl::principal_curvature(V, F, PD1, PD2, PV1, PV2); // Compute curvature directions via quadric fitting
    H2 = 0.5 * (PV1 + PV2); // mean curvature

    //测试
    /*double temp1, temp2;
    int i = 0;
    while(1) //当i超出范围时程序异常退出，由此得到H1或H2的长度，经测试，H1和H2的长度为网格点的个数，但是i相同时H1和H2相差较大，不知为何
    {
        temp1 = H1(i);
        temp2 = H2(i);
        i++;
    }*/

    //曲率归一化（并不需要归一化，因为曲率的正负代表弯曲方向，不能改变，曲率的大小代表弯曲程度，也不要改变）（经测试，对于Beetle.obj模型，H1最大值69.293，最小值0.014，H2最大值6.332，最小值-6.545，实际模型平均曲率有正(凸)有负(凹)，因此使用H2作为正确的平均曲率）
    /*double curvatureMax1 = H1(0), curvatureMin1 = H1(0);
    for(vertexIndex = 0; vertexIndex < vertexNum; vertexIndex++)
    {
        if(H1(vertexIndex) > curvatureMax1)
        {
            curvatureMax1 = H1(vertexIndex);
        }
        else if(H1(vertexIndex) < curvatureMin1)
        {
            curvatureMin1 = H1(vertexIndex);
        }
    }
    for(vertexIndex = 0; vertexIndex < vertexNum; vertexIndex++)
    {
        H1(vertexIndex) = (H1(vertexIndex) - curvatureMin1) / (curvatureMax1 - curvatureMin1) * 2 - 1;
    }
    double curvatureMax2 = H2(0), curvatureMin2 = H2(0);
    for(vertexIndex = 0; vertexIndex < vertexNum; vertexIndex++)
    {
        if(H2(vertexIndex) > curvatureMax2)
        {
            curvatureMax2 = H2(vertexIndex);
        }
        else if(H2(vertexIndex) < curvatureMin2)
        {
            curvatureMin2 = H2(vertexIndex);
        }
    }
    for(vertexIndex = 0; vertexIndex < vertexNum; vertexIndex++)
    {
        H2(vertexIndex) = (H2(vertexIndex) - curvatureMin2) / (curvatureMax2 - curvatureMin2) * 2 - 1;
    }*/

    //将计算得到的曲率信息写入到Mesh
    vertexIndex = 0;
    for(Mesh::VertexIter vertexIter = mToothMesh.vertices_begin(); vertexIter != mToothMesh.vertices_end(); vertexIter++)
    {
        mToothMesh.property(curvature, *vertexIter) = H2(vertexIndex);
        //测试
        /*double tempCurvature1 = H1(vertexIndex);
        double tempCurvature2 = mToothMesh.property(curvature, *vertexIter);*/
        vertexIndex++;
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
