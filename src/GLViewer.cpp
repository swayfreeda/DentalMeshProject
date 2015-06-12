

#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/freeglut.h>

#include"Mesh.h"

#ifdef QT_BUFFER
#include<QGLFunctions>
#endif

#include"include/GLViewer.h"
#include <string>

#include<stdlib.h>
#include<fstream>
#include<sstream>


#include<QFileDialog>
#include<qcolordialog.h>
#include<QMessageBox>
#include<QWidget>
#include<qfiledialog.h>
#include<QProgressDialog>
#include<QObject> // tr was not declare
#include<QApplication> // qaApp
#include<QTextStream>


#include<time.h>
#include<sstream>
#include<cassert>

SW::Shader SW::GLViewer::shader;

SW::GLViewer::GLViewer(QWidget *parent0,  const char *parent1,  QGLWidget*f):
    QGLViewer(parent0, parent1, f)
{
    // setAutoFillBackground(true);
    displayType = FLAT;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
SW::GLViewer::~GLViewer()
{

}

////////////////////////////////////NON CLASS METHOD///////////////////////////////////////////
QString SW::GLViewer::helpString()
{
    QString text("<h2> MeshLive 1.0 [2006.10.18.1]<p></h2>");
    text += "An easy and extensible mesh interaction C++ program for real-time applications..<p> ";
    text += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Designed by hywu, jpan, xlvector. Since 2006.<p> ";
    text += "Based on:<p> ";
    text += "QT (http://www.trolltech.com/)<p> ";
    text += "libQGLViewer (http://artis.imag.fr/~Gilles.Debunne/QGLViewer/)<p> ";
    text += "CGAL (http://www.cgal.org/, http://www-sop.inria.fr/geometrica/team/Pierre.Alliez/)<p> ";
    text += "OpenMesh (http://www.openmesh.org/)<p> ";
    text += "Boost (http://www.boost.org/)<p> ";
    text += "OpenCV (http://sourceforge.net/projects/opencvlibrary/)<p> ";
    text += "Python (http://www.python.org/)<p> ";
    text += "etc.<p> ";

    return text;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SW::GLViewer::viewAll()
{
    showEntireScene();

    updateGL();
}

void SW::GLViewer:: setGL( void )
{
    glClearColor( .5, .5, .5, 1. );
    setLighting();
}

void SW::GLViewer:: setLighting( void )
{
    GLfloat position[4] = { 20., 30., 40., 0. };
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glEnable( GL_LIGHT0 );
    glEnable( GL_NORMALIZE );
}

void SW::GLViewer:: setMeshMaterial( void )
{
    GLfloat  diffuse[4] = { .8, .5, .3, 1. };
    GLfloat specular[4] = { .3, .3, .3, 1. };
    GLfloat  ambient[4] = { .2, .2, .5, 1. };

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse  );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient  );
    glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, 16.      );
}

void SW::GLViewer:: initGLSL( void )
   {
       shader.loadVertex( "shaders/vertex.glsl" );
       shader.loadFragment( "shaders/fragment.glsl" );
   }
////////////////////////////////////////////////////////////////////////////////////////////////////
void SW::GLViewer::init()
{
    setGL();
    initGLSL();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SW::GLViewer::draw()
{
   glPushAttrib( GL_ALL_ATTRIB_BITS );
   setMeshMaterial();
   for(int i=0; i< meshes.size(); i++){
       meshes[i].draw(displayType);
   }
   glPopAttrib();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SW::GLViewer::drawAxises(double width, double length)
{
    glEnable(GL_LINE_SMOOTH);

    double axisLength = length;

    glLineWidth(width);

    glBegin(GL_LINES);
    {
        // qglColor(Qt::red);

        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(axisLength, 0.0, 0.0);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        //qglColor(Qt::green);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, axisLength, 0.0);
    }
    glEnd();


    glBegin(GL_LINES);
    {
        //qglColor(Qt::blue);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, axisLength);
    }
    glEnd();

    glLineWidth(1.0);;
    glDisable(GL_LINE_SMOOTH);

    //qglColor(Qt::red);
    glColor3f(1.0, 0.0, 0.0);
    //renderText(axisLength, 0.0, 0.0, "X", QFont("helvetica", 12, QFont::Bold, TRUE));

    //qglColor(Qt::green);
    glColor3f(0.0, 1.0, 0.0);
    // renderText(0.0, axisLength, 0.0, "Y", QFont("helvetica", 12, QFont::Bold, TRUE));

    //qglColor(Qt::blue);
    glColor3f(0.0, 0.0, 1.0);
    // renderText(0.0, 0.0, axisLength, "Z", QFont("helvetica", 12, QFont::Bold, TRUE));

}

void SW::GLViewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
}
void SW::GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
    QGLViewer::mouseReleaseEvent(e);
}
void SW::GLViewer::mouseMoveEvent(QMouseEvent *e)
{
    QGLViewer::mouseMoveEvent(e);
}
void SW::GLViewer::wheelEvent(QWheelEvent *e)
{
    QGLViewer::wheelEvent(e);
}
void SW::GLViewer::keyPressEvent(QKeyEvent *e)
{
    QGLViewer::keyPressEvent(e);
}
