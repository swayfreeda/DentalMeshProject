#include<stdlib.h>
#include<iostream>
#include"include/Mesh.h"
#include"Eigen/Sparse"
#include<vector>
#include <math.h>
#include "include/LaplaceTransform.h"
#include<time.h>
#include<QDebug>
#include<float.h>

using namespace MHW;

LTransform::LTransform(SW::Mesh Mesh,std::string Mesh_Path_Save,int Ww, double Ff,int Npoint):spMat(Npoint,Npoint),spMat_V(Npoint,3),spMat_L(Npoint,Npoint),spMat_F(Npoint,Npoint),FC_spMat_A(Npoint,Npoint),FC_spMat_B_N3(Npoint,3){

    objMesh=Mesh;
    Mesh_Save=Mesh_Path_Save;
    Mpoint=objMesh.n_vertices();//矩阵点的个数;
    //    LMT_point=objMesh.Get_limitP_fM(Select_P_Array,MoveVectors);
    W=Ww;
    F=Ff;
    LMT_point_size=0;
    std::vector<double> DMat;//对角矩阵;
    spMat_C=NULL;

    //    Eigen::SparseMatrix<double> *spMat_C_tem= new Eigen::SparseMatrix<double> (Npoint,Npoint);
    //    spMat_C=*()
    Create_spMat_V();
    Create_spMat();
    Create_spMat_L();
    Create_spMat_F();

    //connect(this, SIGNAL(selected_changed()), this, SLOT(updateLaplacian()));

    // emit selected_changed();
}



void LTransform::Run(SW::Mesh &Mesh,QVector<QVector<int> > Select_P_Array, SW::MVector MoveVectors){

    Eigen::SparseMatrix<double> spMat_U(Mpoint,3);


    LMT_point=objMesh.Get_limitP_fM(Select_P_Array,MoveVectors);
    if(LMT_point_size!=LMT_point.size()){
        LMT_point_size=LMT_point.size();
        Create_spMat_C();
        Create_spMat_U(spMat_U);
        Create_NewMatrix_V(Mesh,*(spMat_C),spMat_U,spMat_F);
    }else{
        Create_spMat_U(spMat_U);
        Create_NewMatrix_V(Mesh,*(spMat_C),spMat_U,spMat_F);
    }

    // Create_NewMatrix_V(SW::Mesh& Mesh, Eigen::SparseMatrix &spMat_C, Eigen::SparseMatrix &spMat_U){

}



//void initL(){
//    objMesh=Mesh;
//    Mesh_Save=Mesh_Path_Save;
//    Mpoint=objMesh.n_vertices();//矩阵点的个数;
//    LMT_point=objMesh.Get_limitP_fM(Select_P_Array,MoveVectors);
//    W=Ww;
//    F=Ff;
//    std::vector<double> DMat;//对角矩阵;
//    Create_spMat_V();
//    Create_spMat();
//    Create_spMat_L();
//}

void LTransform::Create_spMat_V(){
    //*****************************
    //2015-06-23 TYPE=Notes
    //*****************************
    //遍历输出所有坐标位置;构建笛卡尔坐标矩阵;
    //*****************************
    qDebug() <<"START:spMat_V.insert"<< endl;
    for(auto it=objMesh.vertices_begin();it!=objMesh.vertices_end();++it){//MHW::Mesh::VertexIter
        auto point=objMesh.point(it.handle());//OpenMesh::Vec3f
        spMat_V.insert(it.handle().idx(),0) =point[0];
        spMat_V.insert(it.handle().idx(),1) =point[1];
        spMat_V.insert(it.handle().idx(),2) =point[2];

        // std::cout<<"x"<<point[0]<<"____y"<<point[1]<<"____z"<<point[2]<<std::endl;
    }
    qDebug() <<"END:spMat_V.insert"<< endl;
}

void LTransform::Create_spMat(){
    //*****************************
    //2015-06-26 TYPE=Notes
    //*****************************
    //建立临接矩阵
    //*****************************
    int temp_js=0;
    for(auto it=objMesh.halfedges_begin();it!=objMesh.halfedges_end();++it){
        auto fromVX =objMesh.from_vertex_handle(it.handle());
        auto toVX =objMesh.to_vertex_handle(it.handle());
        int fromN=fromVX.idx();
        int toN=toVX.idx();

        //目前仅计算大到小的边的W
        if(fromN>toN){
            auto h_next=objMesh.next_halfedge_handle(it.handle());
            auto h_before=objMesh.next_halfedge_handle(h_next);

            double E_length=objMesh.Gethalfedge_length(it.handle());//BD,DB长度;

            double  length_close2=objMesh.Gethalfedge_length(h_before);//CB长度;
            double  length_opposite=objMesh.Gethalfedge_length(h_next);//DC长度;

            double Htan1_B=objMesh.GetHtan_angleV(E_length,length_close2,length_opposite);//角CBD一半的正切值;
            double Htan1_S=objMesh.GetHtan_angleV(E_length,length_opposite,length_close2);//角BDC一半的正切值;
            double Htan2_B;//角DBA一半的正切值;
            double Htan2_S;//角ADB一半的正切值;
            if(objMesh.is_boundary(it.handle())){

                Htan2_B=Htan1_B;
            }else{
                auto OE= objMesh.opposite_halfedge_handle(it.handle());
                auto OE_h_next=objMesh.next_halfedge_handle(OE);
                auto OE_h_before=objMesh.next_halfedge_handle(OE_h_next);

                double  OE_length_close2=objMesh.Gethalfedge_length(OE_h_next);//BA长度;
                double  OE_length_opposite=objMesh.Gethalfedge_length(OE_h_before);//AD长度;
                Htan2_B=objMesh.GetHtan_angleV(E_length,OE_length_close2,OE_length_opposite);//角DBA一半的正切值;
                Htan2_S=objMesh.GetHtan_angleV(E_length,OE_length_opposite,OE_length_close2);//角DBA一半的正切值;
            }

            /*
                A --->----D
                  \      //\
                   \    //  \
                    \  //    \
                   B \//---<--\C

                   //num_B>numD的前提：

            */
            //spMat为对称矩阵
            //qDebug() <<"START:spMat.insert"<< endl;
            double  End_Value=(((Htan1_B+Htan2_B)/E_length)+((Htan1_S+Htan2_S)/E_length))/2;


            //**************************************************************************************
            //  测试拉普拉斯矩阵的值
            //处理异常值-nan,-nan导致拉普拉斯变化无法正常进行
            if(isnan(End_Value)){
                End_Value=0;
            }
            std::cout<<End_Value<<std::endl;
            //牙齿数据有4个-nan
            temp_js++;
            //**************************************************************************************


            spMat.insert(fromN,toN)=End_Value;//W_BD的凸权值取B_S的平均值;
            spMat.insert(toN,fromN)=End_Value;//W_BD的凸权值取B_S的平均值;
            // qDebug() <<"END:spMat.insert"<< endl;

        }
    }

    std::cout<<"################################------"<<temp_js<<"----"<<std::endl;
    //*****************************
    //2015-07-03 TYPE=Notes
    //*****************************
    //必须在这里调用Create_DMat;
    //*****************************
    Create_DMat();
    //*****************************
    //2015-06-26 TYPE=Notes
    //*****************************
    //构造拉普拉斯矩阵
    //*****************************
    qDebug() <<"START:spMat2.insert"<< endl;
    for(int i=0;i<DMat.size();i++){
        spMat.insert(i,i) =-DMat[i];//取负值;
    }
    qDebug() <<"END:spMat2.insert"<< endl;
}




void LTransform::Create_DMat(){
    //*****************************
    //2015-06-26 TYPE=Notes
    //*****************************
    //spMat非主对角线元素必须已赋值
    //建立对角矩阵;
    //*****************************
    DMat.resize(Mpoint);

    for (int k=0; k<spMat.outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(spMat,k); it; ++it)
        {
            if(it.value()>1000){
                //错误原因是半边的长度为0;
                std::cout<<"error_index"<<it.index()<<"value:"<<it.value()<<std::endl;

            }
            DMat[it.row()]=DMat[it.row()]+it.value();
        }
}

void LTransform::Create_spMat_L(){
    //*****************************
    //2015-07-03 TYPE=Alert;
    //*****************************
    //函数调用顺序不可变;
    //*****************************

    spMat_L=spMat*spMat_V;//构造拉普拉斯坐标系;//??????????????????????????
}

void LTransform::Create_spMat_C(){

    //*****************************
    //2015-06-29 TYPE=Notes
    //*****************************
    //LMT_point必须已赋值
    //求解笛卡尔坐标;1.构建选择矩阵spMat_C;控制点的索引;
    //*****************************
    qDebug() <<"START:spMat_C.insert"<< endl;
    delete spMat_C;
    spMat_C= new Eigen::SparseMatrix<double> (Mpoint,Mpoint);

    for(int i=0;i<LMT_point.size();i++){
        spMat_C->insert(LMT_point[i].index,LMT_point[i].index)=1;

    }


    qDebug() <<"END:spMat_C.insert"<< endl;
    //*****************************
    FC_spMat_A=(spMat.transpose()*spMat)+((W*(*spMat_C))+spMat_F);//计算Ax=b中的A
    lltOfA.compute(FC_spMat_A);//计算X=A`b的A`
    //*****************************
}

void LTransform::Create_spMat_U(Eigen::SparseMatrix<double> &spMat_U){
    //*****************************
    //2015-06-29 TYPE=Notes
    //*****************************
    //求解笛卡尔坐标;2.构建控制点矩阵spMat_U;
    //*****************************
    //spMat_U=spMat_V;

    //*****************************
    qDebug() <<"START:spMat_U.insert"<< endl;
    for(int i=0;i<LMT_point.size();i++){

        spMat_U.insert(LMT_point[i].index,0)=LMT_point[i].X;
        spMat_U.insert(LMT_point[i].index,1)=LMT_point[i].Y;
        spMat_U.insert(LMT_point[i].index,2)=LMT_point[i].Z;

    }


    for (int k=0; k<spMat_V.outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(spMat_V,k); it; ++it){

            if(!objMesh.is_limitP(LMT_point,it.row())){
                spMat_U.insert(it.row(),it.col())=it.value();
            }

        }
    qDebug() <<"END:spMat_U.insert"<< endl;

}//控制点坐标矩阵;

void LTransform::Create_spMat_F(){
    qDebug() <<"START:spMat_F.insert"<< endl;
    std::cout<<F<<std::endl;
    for(int i=0;i<Mpoint;i++){
        spMat_F.insert(i,i)=F;
    }
    qDebug() <<"END:spMat_F.insert"<< endl;
}//整体位置保持控制矩阵;


void LTransform::Create_NewMatrix_V(SW::Mesh& Mesh,Eigen::SparseMatrix<double> &spMat_C, Eigen::SparseMatrix<double> &spMat_U,Eigen::SparseMatrix<double> &spMat_F){

    //*****************************
    //    Eigen::SparseMatrix<double> FC_spMat_A;//待求矩阵X的前一项
    //    Eigen::SparseMatrix<double> FC_spMat_B_N3;//待求矩阵X的后一项
    Eigen::VectorXd b_x(Mpoint),b_y(Mpoint),b_z(Mpoint), x_x(Mpoint),x_y(Mpoint),x_z(Mpoint);//为了便于计算,N*3向量化为N*1;
    //*****************************
    //2015-06-29 TYPE=Notes
    //*****************************
    //求解笛卡尔坐标;2.分解矩阵表达;
    //*****************************
    //FC_spMat_A*X=FC_spMat_B_N3;//原式
    //FC_spMat_A*x_z=b_x;//x坐标
    //FC_spMat_A*x_y=b_y;//y坐标
    //FC_spMat_A*x_z=b_z;//z坐标
    //*****************************
    //FC_spMat_A=(spMat.transpose()*spMat)+(W*spMat_C);
    //FC_spMat_B_N3=(spMat.transpose()*spMat_L)+(W*spMat_C*spMat_U);
    // FC_spMat_A=(spMat.transpose()*spMat)+((W*spMat_C)+spMat_F);
    FC_spMat_B_N3=(spMat.transpose()*spMat_L)+(((W*spMat_C)+spMat_F)*spMat_U);


    //*************************************************************
    //Cholesky分解
    //*************************************************************
    //2015-07-13 Type=Add;
    //*************************************************************
    Eigen::MatrixXd VV(Mpoint,3);



    VV=lltOfA.solve(FC_spMat_B_N3);
    //Solve_Cholesky(FC_spMat_A,FC_spMat_B_N3);



    //        for(int i=0;i<Mpoint;i++){
    //            OpenMesh::VertexHandle NewPoint(i);
    //            auto Tem_Point=objMesh.point(NewPoint);
    //            Tem_Point[0]=VV(i,0);
    //            Tem_Point[1]=VV(i,1);
    //            Tem_Point[2]=VV(i,2);
    //            objMesh.set_point(NewPoint,Tem_Point);
    //            //std::cout<<VV(i,j);

    //        }




    for(int i=0;i<Mpoint;i++){
        OpenMesh::VertexHandle NewPoint(i);
        auto Tem_Point=Mesh.point(NewPoint);
        Tem_Point[0]=VV(i,0);
        Tem_Point[1]=VV(i,1);
        Tem_Point[2]=VV(i,2);
        Mesh.set_point(NewPoint,Tem_Point);
        //std::cout<<VV(i,j);

    }
    //*************************************************************
    //    for (int k=0; k<FC_spMat_B_N3.outerSize(); ++k)
    //        for (Eigen::SparseMatrix<double>::InnerIterator it(FC_spMat_B_N3,k); it; ++it)
    //        {
    //            if(it.col()==0){
    //                b_x[it.row()]=it.value();
    //            }
    //            if(it.col()==1){
    //                b_y[it.row()]=it.value();
    //            }
    //            if(it.col()==2){
    //                b_z[it.row()]=it.value();
    //            }
    //            if(it.col()>2){
    //                std::cout<<"error 3 to 1:"<<"Row:"<<it.row()<<"Col:"<<it.col()<<std::endl;
    //            }

    //        }

    //    CGSolver<double>::Solve(FC_spMat_A,b_x,x_x);
    //    CGSolver<double>::Solve(FC_spMat_A,b_y,x_y);
    //    CGSolver<double>::Solve(FC_spMat_A,b_z,x_z);


    //    for(int i=0;i<Mpoint;i++){
    //        OpenMesh::VertexHandle NewPoint(i);
    //        auto Tem_Point=objMesh.point(NewPoint);
    //        Tem_Point[0]=x_x[i];
    //        Tem_Point[1]=x_y[i];
    //        Tem_Point[2]=x_z[i];
    //        objMesh.set_point(NewPoint,Tem_Point);
    //    }

    // Mesh=objMesh;
    // objMesh.writeModel(Mesh_Save);

}

void Do_LTransform::startL(QVector<SW::Mesh>&M){
    meshes=M;
    P  =new MHW::LTransform(meshes[0],"./",10000,0,meshes[0].n_vertices());
}
