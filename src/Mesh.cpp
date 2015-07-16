#include"include/Mesh.h"

using namespace SW;

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

// 0--vertices 1-- wireframe 2-- flatLine
void Mesh::draw(int flag)
{
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
        draw(0);
        break;
    case 2:
        //glColor3f(1.0f, 1.0f, 1.0f);
        //request_vertex_normals();
        for(Mesh::FaceIter fit = faces_begin(); fit != faces_end(); fit++)
        {
            //如果存在顶点颜色，则计算该面片所有顶点颜色的平均值，显示在该面片上
            if(!this->has_vertex_colors())
            {
                glColor3f(1.0, 1.0, 1.0);
            }
            else
            {
                tempColor[0] = 0; tempColor[1] = 0; tempColor[2] = 0;
                for(Mesh::FaceVertexIter faceVertexIter = this->fv_iter(*fit); faceVertexIter.is_valid(); faceVertexIter++)
                {
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
                glVertex3f(v[0], v[1], v[2]);
            }
            glEnd();
        }
        break;
    }

}

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
