#ifndef BOOLEANOPERATION_H
#define BOOLEANOPERATION_H

#include "Mesh.h"
enum BooleanOperation{
    UNION,
    INTERSECTION,
    DIFFERENCE,
};

#include <igit_geometry/Nef_polyhedron_3.h>
#include<igit_geometry/Exact_predicates_exact_kernel.h>
#include <igit_geometry/Polyhedron_3.h>
#include <igit_geometry/Polyhedron_incremental_builder_3.h>
#include<igit_geometry/Union_find.h>
#include <iostream>
#include <fstream>
#include"include/Mesh.h"
#include<OpenMesh/Core/Mesh/AttribKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double> > Polyhedron_d;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Kernel::Vector_3 Vector_3;
typedef Kernel::Aff_transformation_3 Aff_transformation_3;
typedef Polyhedron::HalfedgeDS         HalfedgeDS;
typedef typename Polyhedron::Vertex_const_iterator                  VCI;
typedef typename Polyhedron::Facet_const_iterator                   FCI;
typedef typename Polyhedron::Halfedge_around_facet_const_circulator HFCC;
using namespace SW;
struct CGALtoOpenMesh{
    int Openindex;
    Mesh::Point point_axis;
};
template <class HDS>
class Build_triangle : public CGAL::Modifier_base<HDS> {
public:
    Mesh MyMesh;
    Build_triangle() {}
    void operator()( HDS& hds) {
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( MyMesh.n_vertices(), MyMesh.n_faces(), 0);//面起始
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;
        std::vector<int> Vector_Openmesh_to_CGAL;
        int GP_js=0;//设置计数器
        Vector_Openmesh_to_CGAL.resize(MyMesh.n_vertices(),4);//设置容器大小
        for(int i=0;i<MyMesh.n_vertices();i++){
            Vector_Openmesh_to_CGAL[i]=-1;//初始化为-1
        }
        for(auto it=MyMesh.faces_begin();it!=MyMesh.faces_end();++it){    //遍历openmesh所有面
            int M_js=0;
            int P_index[3];
            for(auto it2=MyMesh.fv_begin(it.handle());it2!=MyMesh.fv_end(it.handle());++it2){//openmesh的face_vectex
                auto point=MyMesh.point(it2.handle());
                // std::cout<<"--------x"<<point[0]<<"--------y"<<point[1]<<"---------z"<<point[2]<<std::endl;
                P_index[M_js]=it2.handle().idx();//保存点的索引号
                if(Vector_Openmesh_to_CGAL[P_index[M_js]]==-1){
                    B.add_vertex( Point( point[0], point[1], point[2]));//保存点
                }
                M_js++;
            }
            B.begin_facet();//加面
            for(int i=0;i<3;i++){
                if(Vector_Openmesh_to_CGAL[P_index[i]]==-1){      //判断点的索引号是否存在（初值为-1）
                    Vector_Openmesh_to_CGAL[P_index[i]]=GP_js;     //不存在，赋新索引号
                    GP_js++;
                    B.add_vertex_to_facet(Vector_Openmesh_to_CGAL[P_index[i]]); //把点的索引号给面
                }else{
                    B.add_vertex_to_facet(Vector_Openmesh_to_CGAL[P_index[i]]);
                }
            }
            B.end_facet();
        }
        B.end_surface();
    }
};
class TransformMode{
public:
    Polyhedron P1;
    Polyhedron P2;
    Polyhedron P12;
    Build_triangle<HalfedgeDS> triangle1,triangle2;
    //get data
    TransformMode(SW::Mesh M1,SW::Mesh M2){
        triangle1.MyMesh=M1;
        triangle2.MyMesh=M2;
        P1.delegate(triangle1);
        P2.delegate(triangle2);
    }
    Mesh Bool_Action(char Bool){  //判断操作
        if (Bool == 'i')
        {
            IntersectionOut();
        }
        else if(Bool=='d'){
            DifferenceOut();
        }
        else if(Bool=='u'){
            UnionOut();
        }
        else
            std::cout<<"action is wrong !"<<std::endl;
        // CGAL  to  openmesh
        std::vector<CGALtoOpenMesh> Vector_CGALtoOpenmesh;  //同上，建立容器
        Mesh CGAL_to_NewMesh;
        for( FCI fi = P12.facets_begin(); fi != P12.facets_end(); ++fi){  //遍历输出后的，CGAL所有面
            std::vector<OpenMesh::VertexHandle> FacePoint;
            int f_js=0;//读取面的3各点
            for( HFCC hc = fi->facet_begin();f_js<3;++hc,f_js++){
                double x_point=CGAL::to_double(hc->vertex()->point().x());
                double y_point=CGAL::to_double(hc->vertex()->point().y());
                double z_point=CGAL::to_double(hc->vertex()->point().z());
                //判断是否是新点，CheckEixstP遍历所有点，对比是否存在。新点返回-1，旧点返回索引号
                int Check_P =CheckEixstP(Vector_CGALtoOpenmesh,x_point,y_point,z_point);
                if(Check_P!=-1){//旧点索引号进栈
                    OpenMesh::VertexHandle old_P(Check_P);
                    FacePoint.push_back(old_P);
                }else{//新点
                    CGALtoOpenMesh P_temp;
                    P_temp.point_axis[0]=x_point;
                    P_temp.point_axis[1]=y_point;
                    P_temp.point_axis[2]=z_point;
                    OpenMesh::VertexHandle new_P=CGAL_to_NewMesh.add_vertex(P_temp.point_axis);
                    P_temp.Openindex=new_P.idx();//存索引号
                    Vector_CGALtoOpenmesh.push_back(P_temp);
                    FacePoint.push_back(new_P);
                }
            }
            CGAL_to_NewMesh.add_face(FacePoint[0],FacePoint[1],FacePoint[2]);//给面添加索引号
        }
        return CGAL_to_NewMesh;
    }
private:
    void IntersectionOut(){
        Nef_polyhedron N1(P1);
        Nef_polyhedron N2(P2);
        Nef_polyhedron N12;
        Aff_transformation_3 aff(CGAL::TRANSLATION, Vector_3(0.1,0.11,0,1));
        N2.transform(aff);
        N12 = N1.intersection(N2);
        N12.closure().convert_to_polyhedron(P12);
    }
    void UnionOut(){
        Nef_polyhedron N1(P1);
        Nef_polyhedron N2(P2);
        Nef_polyhedron N12;
        Aff_transformation_3 aff(CGAL::TRANSLATION, Vector_3(0.1,0.11,0,1));
        N2.transform(aff);
        N12 = N1.join(N2);
        N12.closure().convert_to_polyhedron(P12);
    }
    void DifferenceOut(){
        Nef_polyhedron N1(P1);
        Nef_polyhedron N2(P2);
        Nef_polyhedron N12;
        Aff_transformation_3 aff(CGAL::TRANSLATION, Vector_3(0.1,0.11,0,1));
        N2.transform(aff);
        N12 = N1.difference(N2);
        N12.closure().convert_to_polyhedron(P12);
    }
    int CheckEixstP( std::vector<CGALtoOpenMesh> & Vector_CGALtoOpenmesh,double xx,double yy,double zz){
        for(int i=0;i<Vector_CGALtoOpenmesh.size();i++){
            if(Vector_CGALtoOpenmesh[i].point_axis[0]==xx){
                if(Vector_CGALtoOpenmesh[i].point_axis[1]==yy){
                    if(Vector_CGALtoOpenmesh[i].point_axis[2]==zz){
                        return Vector_CGALtoOpenmesh[i].Openindex;
                    }
                }
            }
        }
        return -1;
    }
};



#endif // BOOLEANOPERATION_H
