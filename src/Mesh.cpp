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

// 0--vertices 1-- wireframe 2-- flatLine
void Mesh::draw(int flag)
{
    //画OpenGL原点
    glPushMatrix();
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
    glPointSize(1);
    glPopMatrix();

    //画BoundingBox
    glPushMatrix();
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
    glPointSize(1);
    glPopMatrix();

    //int featureVertexNum = 0; //牙齿分割初始边界点个数

    switch (flag)
    {
    case 0:
        glPushMatrix();
        //glPointSize(1);
        glBegin(GL_POINTS);
        for(Mesh::HalfedgeIter hit = this->halfedges_begin(); hit != this->halfedges_end(); hit++)
        {
            Mesh::VertexHandle vertexHandle = to_vertex_handle(hit);
            Mesh::Point vertex = this->point(vertexHandle);

            //设置颜色
            OpenMesh::VPropHandleT<double> curvature;
            if(this->get_property_handle(curvature, "curvature"))
            {
                double curvatureValue = this->property(curvature, vertexHandle);
                if(curvatureValue < -1)
                {
                    //featureVertexNum++;
                    //std::cout << "Detected " << featureVertexNum << " feature vertex: " << vertex << std::endl;
                    glColor3f(1.0f, 0.0f, 0.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
            }

            //设置法向量
            Mesh::Normal normal;
            calc_vertex_normal_loop(vertexHandle, normal);
            glNormal3f(normal[0], normal[1], normal[2]);

            //设置坐标
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
        glEnd();
        glPopMatrix();
        break;
    case 1:
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
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
        break;
    case 2:
        glColor3f(1.0f, 1.0f, 1.0f);
        //request_vertex_normals();
        for(Mesh::FaceIter fit = faces_begin(); fit != faces_end(); fit++)
        {

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
