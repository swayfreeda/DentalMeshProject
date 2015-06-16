#include"include/Mesh.h"


using namespace SW;


Mesh::Mesh(){}
Mesh::Mesh(QString name):MeshName(name){}


void Mesh::getBoundingBox(){}

// 0--vertices 1-- wireframe 2-- flatLine
void Mesh::draw(int flag){

    switch (flag){
    case 0:
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        for(Mesh::HalfedgeIter hit= this->halfedges_begin(); hit!= this->halfedges_end(); hit++){
            Mesh::VertexHandle vh = to_vertex_handle(hit);
            Mesh::Point v = this->point(vh);

            //normal
            Mesh::Normal nor;
            calc_vertex_normal_loop(vh, nor);
            glNormal3f(nor[0], nor[1], nor[2]);
            //position
            glVertex3f(v[0], v[1], v[2]);
        }
        glEnd();
        glPopMatrix();
        break;
    case 1:
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        for(Mesh::EdgeIter eit = edges_begin(); eit!= edges_end(); eit++){
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
        for(Mesh::FaceIter fit = faces_begin(); fit!= faces_end(); fit++){

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_POLYGON);
            Mesh::Normal nor = calc_face_normal(fit);
            glNormal3f(nor[0], nor[1], nor[2]);
            for(Mesh::FaceHalfedgeIter fhit = fh_iter(*fit);
                fhit.is_valid(); ++fhit){
                Mesh::VertexHandle vh = to_vertex_handle(fhit);
                Mesh::Point v = this->point(vh);
                glVertex3f(v[0], v[1], v[2]);
            }
            glEnd();
        }

        break;

    }

}


