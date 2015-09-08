#include"include/Mesh.h"

using namespace SW;
//************************************************************//
//2015/09/07
//mhw merge code
//************************************************************//
bool Mesh::writeModel(std::string Write_path){
    bool ret=OpenMesh::IO::write_mesh(*(this),Write_path);
    if(ret==1){
        return 0;// no error;
    }else{
        return 1;// there have an error;
    }
}
bool Mesh::readModel(std::string Mod_Path){

    bool ret=OpenMesh::IO::read_mesh(*(this),Mod_Path);


    this->Hedge_angle.resize(this->n_halfedges());
    //*****************************
    //2015-06-23 TYPE=Notes
    //*****************************
    //我想吐槽一下为什么返回是一个整数;为什么返回1表示没有错误？？？？;
    //*****************************

    if(ret==1){
        return 0;// no error;
    }else{
        return 1;// there have an error;
    }

}


bool Mesh::isSelectP(Mesh::VertexHandle vh,QVector<QVector<int> > Select_P_Array,int* Belong_PS){
    for(int i=0;i<Select_P_Array.size();i++){
        for(int j=0;j<Select_P_Array[i].size();j++){

            if(vh.idx()==Select_P_Array[i][j]){
                *(Belong_PS)=i;
                return true;

            }
        }
    }
    return false;
}

double Mesh::Gethalfedge_length(OpenMesh::HalfedgeHandle hh){

    auto point1=this->point(this->from_vertex_handle(hh));
    auto point2=this->point(this->to_vertex_handle(hh));
    //        std::cout<<"x"<<point1[0]<<"____y"<<point1[1]<<"____z"<<point1[2]<<std::endl;
    //        std::cout<<"x"<<point2[0]<<"____y"<<point2[1]<<"____z"<<point2[2]<<std::endl;
    auto l_temX=(point1[0]-point2[0])*(point1[0]-point2[0]);
    auto l_temY=(point1[1]-point2[1])*(point1[1]-point2[1]);
    auto l_temZ=(point1[2]-point2[2])*(point1[2]-point2[2]);
    return std::sqrt(l_temX+l_temY+l_temZ);

}

OpenMesh::VertexHandle Mesh::Getopposite_point(OpenMesh::HalfedgeHandle hh){
}

double Mesh::GetHtan_angleV(double Close_edgeA_length,double Close_edgeB_length,double Opposite_edge_length){
    double CosV=((Close_edgeA_length*Close_edgeA_length)+(Close_edgeB_length*Close_edgeB_length)-(Opposite_edge_length*Opposite_edge_length))/(2*Close_edgeA_length*Close_edgeB_length);
    return std::sqrt((1-CosV)/(1+CosV));
}

void Mesh::HandleFace_EA(OpenMesh::FaceHandle face){

    std::vector<OpenMesh::VertexHandle> vv;
    std::vector<OpenMesh::HalfedgeHandle> hh;

    //*****************************
    //2015-06-25 TYPE=Note
    //*****************************
    //取面的点与边;
    //*****************************
    for(auto it=this->fh_begin(face);it!=this->fh_end(face);++it){
        hh.push_back(it.handle());
    }
    for(auto it=this->fv_begin(face);it!=this->fv_end(face);++it){
        vv.push_back(it.handle());
    }
    //*****************************
    //2015-06-25 TYPE=dd
    //*****************************
    //求各个边的长度;
    //*****************************
    double h0_length=this->Gethalfedge_length(hh[0]);
    double h1_length=this->Gethalfedge_length(hh[1]);
    double h2_length=this->Gethalfedge_length(hh[2]);
    if((h0_length*h1_length*h2_length)==0){
        std::cout<<"Face_edge_length_err"<<face.idx()<<std::endl;
    }


    //*****************************
    //2015-06-25 TYPE=dd
    //*****************************
    //处理边角关系，建立边角关系向量;
    //*****************************

    for(auto it=vv.begin();it!=vv.end();++it){

        if(*it!=this->from_vertex_handle(hh[0])&&*it!=this->to_vertex_handle(hh[0])){

            double Vhalftan=this->GetHtan_angleV(h1_length,h2_length,h0_length);

            if(*it!=this->from_vertex_handle(hh[1])){
                //std::cout<<"不是H1的from角"<<std::endl;
                this->Hedge_angle[hh[1].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[1].idx()].t_angle_halftan=Vhalftan;


                this->Hedge_angle[hh[2].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[2].idx()].f_angle_halftan=Vhalftan;



            }else{
                // std::cout<<"是H1的from角"<<std::endl;
                this->Hedge_angle[hh[1].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[1].idx()].f_angle_halftan=Vhalftan;

                this->Hedge_angle[hh[2].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[2].idx()].t_angle_halftan=Vhalftan;
            }

        }

    }

    for(auto it=vv.begin();it!=vv.end();++it){

        if(*it!=this->from_vertex_handle(hh[1])&&*it!=this->to_vertex_handle(hh[1])){

            double Vhalftan=this->GetHtan_angleV(h0_length,h2_length,h1_length);

            if(*it!=this->from_vertex_handle(hh[0])){
                // std::cout<<"不是H0的from角"<<std::endl;
                this->Hedge_angle[hh[0].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[0].idx()].t_angle_halftan=Vhalftan;


                this->Hedge_angle[hh[2].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[2].idx()].f_angle_halftan=Vhalftan;



            }else{
                // std::cout<<"是H0的from角"<<std::endl;
                this->Hedge_angle[hh[0].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[0].idx()].f_angle_halftan=Vhalftan;

                this->Hedge_angle[hh[2].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[2].idx()].t_angle_halftan=Vhalftan;
            }

        }

    }

    for(auto it=vv.begin();it!=vv.end();++it){

        if(*it!=this->from_vertex_handle(hh[2])&&*it!=this->to_vertex_handle(hh[2])){

            double Vhalftan=this->GetHtan_angleV(h0_length,h1_length,h2_length);

            if(*it!=this->from_vertex_handle(hh[0])){
                // std::cout<<"不是H0的from角"<<std::endl;
                this->Hedge_angle[hh[0].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[0].idx()].t_angle_halftan=Vhalftan;


                this->Hedge_angle[hh[1].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[1].idx()].f_angle_halftan=Vhalftan;



            }else{
                // std::cout<<"是H0的from角"<<std::endl;
                this->Hedge_angle[hh[0].idx()].edge_length=h1_length;
                this->Hedge_angle[hh[0].idx()].f_angle_halftan=Vhalftan;

                this->Hedge_angle[hh[1].idx()].edge_length=h2_length;
                this->Hedge_angle[hh[1].idx()].t_angle_halftan=Vhalftan;
            }

        }

    }


}



std::vector<MyPoint>  Mesh::Get_limitP_fM(QVector<QVector<int> > Select_P_Array, MVector MoveVectors){
    std::vector<MyPoint> ret;
    bool CongFu=false;
    for(int i=0;i<Select_P_Array.size();i++){

        for(int j=0;j<Select_P_Array[i].size();j++){

            for(int k=0;k<ret.size();k++){
                if(Select_P_Array[i][j]==ret[k].index){
                    CongFu=true;
                }
            }
            if(!CongFu){
                MyPoint tempV_P;
                OpenMesh::VertexHandle tempV(Select_P_Array[i][j]);
                auto iter=this->point(tempV);
                tempV_P.index=tempV.idx();
                // double ctl_l=0.05;
                double ctl_l=1;
                tempV_P.X=iter[0]+(ctl_l*MoveVectors.X_arr[i]);
                tempV_P.Y=iter[1]+(ctl_l*MoveVectors.Y_arr[i]);
                tempV_P.Z=iter[2]+(ctl_l*MoveVectors.Z_arr[i]);
                //tempV_P.Z=iter[2];
                ret.push_back(tempV_P);
            }else{
                CongFu=false;
            }

        }

    }
    return ret;
}

bool Mesh::is_limitP(std::vector<MyPoint> LMT_point,int indexN){

    for(int i=0;i<LMT_point.size();i++){
        if(LMT_point[i].index==indexN){
            return true;
        }

    }
    return false;
}

//************************************************************//

Mesh::Mesh(){}

Mesh::Mesh(QString name):MeshName(name){}

void Mesh::getBoundingBox(){}

void Mesh::computeBoundingBox()
{
    Point tempVertex = this->point(*(this->vertices_begin()));
    float maxX = tempVertex[0], minX = tempVertex[0];
    float maxY = tempVertex[1], minY = tempVertex[1];
    float maxZ = tempVertex[2], minZ = tempVertex[2];
    for(VertexIter vertexIter = this->vertices_begin(); vertexIter != this->vertices_end(); vertexIter++)
    {
        tempVertex = this->point(*vertexIter);
        if(tempVertex[0] > maxX)
        {
            maxX = tempVertex[0];
        }
        else if(tempVertex[0] < minX)
        {
            minX = tempVertex[0];
        }
        if(tempVertex[1] > maxY)
        {
            maxY = tempVertex[1];
        }
        else if(tempVertex[1] < minY)
        {
            minY = tempVertex[1];
        }
        if(tempVertex[2] > maxZ)
        {
            maxZ = tempVertex[2];
        }
        else if(tempVertex[2] < minZ)
        {
            minZ = tempVertex[2];
        }
    }
    BBox.origin.x = minX;
    BBox.origin.y = minY;
    BBox.origin.z = minZ;
    BBox.size.x = maxX - minX;
    BBox.size.y = maxY - minY;
    BBox.size.z = maxZ - minZ;
}

void Mesh::computeEntityNumbers()
{
    mVertexNum =0;
    mFaceNum = 0;
    mEdgeNum = 0;
    for(VertexIter vit = this->vertices_begin(); vit != this->vertices_end(); vit++)
    {
        mVertexNum++;
    }
    for(FaceIter fit = this->faces_begin(); fit != this->faces_end(); fit++)
    {
        mFaceNum++;
    }
    for(EdgeIter eit = this->edges_begin(); eit != this->edges_end(); eit++)
    {
        mEdgeNum++;
    }
}


void Mesh::draw(int flag,   QVector< QVector<int> > Select_P_Array,MVector MoveVectors){
    drawOrigin();
    drawBoundingBox();

    Mesh::Point tempVertex;
    Mesh::Color tempColor;
    Mesh::Normal tempNormal;

    switch (flag)
    {
    case 0:
        glPushMatrix();
        glPointSize(5);
        glBegin(GL_POINTS);
        for(Mesh::HalfedgeIter hit = this->halfedges_begin(); hit != this->halfedges_end(); hit++)
        {
            Mesh::VertexHandle vertexHandle = to_vertex_handle(hit);
            tempVertex = this->point(vertexHandle);

            //设置颜色
            if(!this->has_vertex_colors()) //如果Mesh中没有顶点颜色这个属性，则将所有顶点设置成白色，否则按该点颜色属性设置实际颜色
            {
                glColor3f(1.0, 1.0, 1.0);
            }
            else
            {
                tempColor = this->color(vertexHandle);
                glColor3f(tempColor[0], tempColor[1], tempColor[2]);
            }

            //设置法向量
            calc_vertex_normal_loop(vertexHandle, tempNormal);
            glNormal3f(tempNormal[0], tempNormal[1], tempNormal[2]);

            //设置坐标
            glVertex3f(tempVertex[0], tempVertex[1], tempVertex[2]);
        }
        glEnd();
        glPopMatrix();
        break;
    case 1:
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        for(Mesh::EdgeIter eit = edges_begin(); eit != edges_end(); eit++)
        {
            Mesh::HalfedgeHandle hh1 = halfedge_handle(eit, 0);
            Mesh::HalfedgeHandle hh2 = halfedge_handle(eit, 1);
            Mesh::VertexHandle vh1 = to_vertex_handle(hh1);
            Mesh::Point v1 = this->point(vh1);

            Mesh::VertexHandle vh2 = to_vertex_handle(hh2);
            Mesh::Point v2 = this->point(vh2);
            glVertex3f(v1[0], v1[1], v1[2]);
            glVertex3f(v2[0], v2[1], v2[2]);
        }
        glEnd();
        glPopMatrix();
        draw(0,Select_P_Array,MoveVectors);
        break;
    case 2:
        //glColor3f(1.0f, 1.0f, 1.0f);
        //request_vertex_normals();
        for(Mesh::FaceIter fit = faces_begin(); fit != faces_end(); fit++){
            //如果存在顶点颜色，则计算该面片所有顶点颜色的平均值，显示在该面片上
            if(!this->has_vertex_colors()){
                glColor3f(1.0, 1.0, 1.0);
            }
            else {
                tempColor[0] = 0.0; tempColor[1] = 0.0; tempColor[2] = 0.0;
                for(Mesh::FaceVertexIter faceVertexIter = this->fv_iter(*fit);
                    faceVertexIter.is_valid(); faceVertexIter++) {
                    tempColor += this->color(faceVertexIter);
                }
                tempColor /= 3.0;
                glColor3f(tempColor[0], tempColor[1], tempColor[2]);
            }


            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_POLYGON);
            Mesh::Normal nor = calc_face_normal(fit);
            glNormal3f(nor[0], nor[1], nor[2]);
            for(Mesh::FaceHalfedgeIter fhit = fh_iter(*fit); fhit.is_valid(); ++fhit)
            {
                Mesh::VertexHandle vh = to_vertex_handle(fhit);
                Mesh::Point v = this->point(vh);

                //*************************************************//
                //2015/09/07
                //mhw merge code
                //*************************************************//
                int Belong_PS=90;
                if(isSelectP(vh,Select_P_Array,&Belong_PS)){
                    glColor3f(0.0f, 0.0f, 1.0f);
                    glVertex3f(v[0]+(0.001*MoveVectors.X_arr[Belong_PS]), v[1]+ (0.001*MoveVectors.Y_arr[Belong_PS]), v[2]);
                }else{
                    //glColor3f(1.0f, 1.0f, 1.0f);
                    glVertex3f(v[0], v[1], v[2]);
                }
            }
            glEnd();
        }
        break;
    }

}

//// 0--vertices 1-- wireframe 2-- flatLine
//void Mesh::draw(int flag,   QVector< QVector<int> > Select_P_Array,MVector MoveVectors){


//}



void Mesh::drawOrigin()
{
    glPushMatrix();
    GLint pointSize;
    glGetIntegerv(GL_POINT_SIZE, &pointSize);
    glPointSize(2);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES); //x坐标
    {
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.1, 0.0, 0.0);
    }
    glEnd();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES); //y坐标
    {
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.1, 0.0);
    }
    glEnd();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES); //z坐标
    {
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.1);
    }
    glEnd();
    glPointSize(pointSize);
    glPopMatrix();
}

void Mesh::drawBoundingBox()
{
    glPushMatrix();
    GLint pointSize;
    glGetIntegerv(GL_POINT_SIZE, &pointSize);
    glPointSize(2);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES); //x坐标
    {
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z);
    }
    glEnd();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES); //y坐标
    {
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z);
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
    }
    glEnd();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES); //z坐标
    {
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z);
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    //其他框线
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
        glVertex3f(BBox.origin.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z);
        glVertex3f(BBox.origin.x + BBox.size.x, BBox.origin.y + BBox.size.y, BBox.origin.z + BBox.size.z);
    }
    glEnd();
    glPointSize(pointSize);
    glPopMatrix();
}
