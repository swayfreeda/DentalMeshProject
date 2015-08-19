
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/freeglut.h>

#ifdef QT_BUFFER
#include<QGLFunctions>
#endif

#include"include/Mesh.h"
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
#include<QDebug>

#include<time.h>
#include<sstream>
#include<cassert>

#include <math.h>

#include "ToothSegmentation.h"

GLuint arrayId;
GLuint numElement;

#define BUFFER_OFFSET(offset)  ((GLvoid*) NULL + offset)
#define NumberOf(array) (sizeof(array)/sizeof(array[0])

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SW::GLViewer::GLViewer(QWidget *parent, const char * name, Qt::WFlags f)
//{
//    setAutoFillBackground(true);// painter auto clear the background
//}

SW::Shader SW::GLViewer::m_shader;

SW::GLViewer::GLViewer(QWidget *parent0,  const char *parent1, QGLWidget*f):
    QGLViewer(parent0, parent1, f)
{
    //setAutoFillBackground(true);
    displayType = FLATLINE;

    m_length = 0.1;
}

SW::GLViewer::~GLViewer()
{

}

//NON CLASS METHOD
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

void SW::GLViewer::viewAll()
{
    //计算所有Mesh的整体BoundingBox
    Mesh::Point tempVertex = meshes[0].point(*(meshes[0].vertices_begin()));
    float maxX = tempVertex[0], minX = tempVertex[0];
    float maxY = tempVertex[1], minY = tempVertex[1];
    float maxZ = tempVertex[2], minZ = tempVertex[2];
    int meshNum = meshes.size();
    for(int meshIndex = 0; meshIndex < meshNum; meshIndex++)
    {
        for(Mesh::VertexIter vertexIter = meshes[meshIndex].vertices_begin(); vertexIter != meshes[meshIndex].vertices_end(); vertexIter++)
        {
            tempVertex = meshes[meshIndex].point(*vertexIter);
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
    }

    setSceneBoundingBox(qglviewer::Vec(minX, minY, minZ), qglviewer::Vec(maxX, maxY, maxZ));
    showEntireScene();
    updateGL();
}

void SW::GLViewer::init()
{

    setGL();
    initGLSL();

#if BUFFER_
    //glClearColor(0.0, 0.0, 0.0, 0.0);

    //glDisable(GL_DITHER);

    glShadeModel(GL_FLAT);

    //glEnable(GL_DEPTH_TEST);

    enum{Vertices, Color, Elements, NumVBOs};
    GLuint buffers[NumVBOs];

    // glew init is very important or errors occur
    glewInit();
    // generate vertex arrays, and each array is corresponding to a object to be render
    glGenVertexArrays(1, &arrayId);

#ifdef QT_BUFFER
    QGLFunctions qtgl;
    qtgl.initializeGLFunctions(0);
#endif

    // 3D world coordinate of points
#ifdef QT_BUFFER
    QVector<QVector3D> Verts;
    Verts.append(QVector3D(-1.0, -1.0, -1.0));
    Verts.append(QVector3D(-1.0, -1.0, 1.0));
    Verts.append(QVector3D(-1.0, 1.0, -1.0));
    Verts.append(QVector3D(-1.0, 1.0, 1.0));
    Verts.append(QVector3D(1.0, -1.0, -1.0));
    Verts.append(QVector3D(1.0, -1.0, 1.0));
    Verts.append(QVector3D(1.0, 1.0, -1.0));
    Verts.append(QVector3D(1.0, 1.0, 1.0));
#else


    GLfloat Verts[][3] = {
        {-1.0, -1.0, -1.0},
        {-1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0},
        {-1.0, 1.0, 1.0},
        {1.0,  -1.0, -1.0},
        {1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0},
        {1.0, 1.0, 1.0},
    };
#endif

    // colors of points
#ifdef QT_BUFFER
    QVector<QVector3D> Colors;
    Colors.append(QVector3D(0, 0.0, 0.0));
    Colors.append(QVector3D(0, 0.0, 1.0));
    Colors.append(QVector3D(0, 1.0, 0.0));
    Colors.append(QVector3D(0, 1.0, 1.0));
    Colors.append(QVector3D(1.0, 0.0, 0.0));
    Colors.append(QVector3D(1.0, 0.0, 1.0));
    Colors.append(QVector3D(1.0, 1.0, 0.0));
    Colors.append(QVector3D(1.0, 1.0, 1.0));
#else
    GLfloat Colors[][3] = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0},
        {0.0, 1.0, 1.0},
        {1.0, 0.0, 0.0},
        {1.0, 0.0, 1.0},
        {1.0, 1.0, 0.0},
        {1.0, 1.0, 1.0},
    };
#endif

    // indices of points
#ifdef QT_BUFFER
    QVector<uint> Indices;
    Indices.append(0);Indices.append(1);Indices.append(3);Indices.append(2);
    Indices.append(4);Indices.append(6);Indices.append(7);Indices.append(5);
    Indices.append(2);Indices.append(3);Indices.append(7);Indices.append(6);
    Indices.append(0);Indices.append(4);Indices.append(5);Indices.append(1);
    Indices.append(0);Indices.append(2);Indices.append(6);Indices.append(4);
    Indices.append(1);Indices.append(5);Indices.append(7);Indices.append(3);
#else
    GLubyte Indices[]={
        0, 1, 3, 2,
        4, 6, 7, 5,
        2, 3, 7, 6,
        0, 4, 5, 1,
        0, 2, 6, 4,
        1, 5, 7, 3,
    };
#endif

    // active a vertex array
    glBindVertexArray(arrayId);

    // generate buffer objects, and each attribute(vertices, color, and normal..) is corresponding to one buffer

#ifdef QT_BUFFER
    qtgl.glGenBuffers(NumVBOs, buffers);
#else
    glGenBuffers(NumVBOs, buffers);
#endif

    //---------------------------------------buffer for vertices----------------------------------//
    // active a buffer object
#ifdef QT_BUFFER
    qtgl.glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]);
#else
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]);
#endif
    // alloc a space for buffer
#ifdef QT_BUFFER
    qtgl.glBufferData(GL_ARRAY_BUFFER, Verts.size()*sizeof(GLfloat), Verts.data(), GL_STATIC_DRAW);
#else
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
#endif
    // put the data into the corresponding buffer
    glVertexPointer(3, GL_FLOAT, 0,BUFFER_OFFSET(0));
    glEnableClientState(GL_VERTEX_ARRAY);



    //---------------------------------------buffer for colors----------------------------------//
    // buffer for colors
#ifdef QT_BUFFER
    qtgl.glBindBuffer(GL_ARRAY_BUFFER, buffers[Color]);
#else
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Color]);
#endif
#ifdef QT_BUFFER
    qtgl.glBufferData(GL_ARRAY_BUFFER, Colors.size()*sizeof(GLfloat), Colors.data(), GL_STATIC_DRAW);
#else
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
#endif
    glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_COLOR_ARRAY);


    //---------------------------------------buffer for elements----------------------------------//
    // buffer for elements
#ifdef QT_BUFFER
    numElement = Indices.size();
#else
    numElement = sizeof(Indices)/sizeof(Indices[0]);
#endif
#ifdef QT_BUFFER
    qtgl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements]);
#else
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements]);
#endif
#ifdef QT_BUFFER
    qtgl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(uint), Indices.data(), GL_STATIC_DRAW);
#else
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
#endif

#endif
}

void SW::GLViewer::draw()
{
    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //m_shader.enable();
    //drawAxises(0.1, m_length);
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    setMeshMaterial();
    foreach(SW::Mesh mesh, meshes){
        glPushMatrix();
          mesh.draw(displayType);
        glPopMatrix();
    }
    glPopAttrib();

    //m_shader.disable();
#if BUFFER_
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);

    drawAxises(0.1, 0.1);

    // active the vertex array
    glBindVertexArray(arrayId);


    // drawelements
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#ifdef QT_BUFFER
    glDrawElements(GL_QUADS, numElement,  GL_UNSIGNED_INT, BUFFER_OFFSET(0));
#else
    glDrawElements(GL_QUADS, numElement,  GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
#endif


#endif
    glFlush();
}

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
    renderText(axisLength, 0.0, 0.0, "X", QFont("helvetica", 12, QFont::Bold, TRUE));

    //qglColor(Qt::green);
    glColor3f(0.0, 1.0, 0.0);
    renderText(0.0, axisLength, 0.0, "Y", QFont("helvetica", 12, QFont::Bold, TRUE));

    //qglColor(Qt::blue);
    glColor3f(0.0, 0.0, 1.0);
    renderText(0.0, 0.0, axisLength, "Z", QFont("helvetica", 12, QFont::Bold, TRUE));

}

void SW::GLViewer::mousePressEvent(QMouseEvent *e)
{
    onMousePressed(e);

    QGLViewer::mousePressEvent(e);
}

void SW::GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
    if(meshes.empty())
    {
        return;
    }
    QGLViewer::mouseReleaseEvent(e);
}

void SW::GLViewer::mouseMoveEvent(QMouseEvent *e)
{
    if(meshes.empty())
    {
        return;
    }
    QGLViewer::mouseMoveEvent(e);
}

void SW::GLViewer::wheelEvent(QWheelEvent *e)
{
    if(meshes.empty())
    {
        return;
    }
    int numDegrees = e->delta() / 8;
    //int numSteps = numDegrees / 15;
    //    if(numSteps >0)
    //    m_length = 0.1/(float)numSteps;
    //    else
    //    m_length = -0.1*(float)numSteps;



    //    //imageScale();
    e->accept();
    update();
    QGLViewer::wheelEvent(e);
}

void SW::GLViewer::keyPressEvent(QKeyEvent *e)
{
    if(meshes.empty())
    {
        return;
    }
    QGLViewer::keyPressEvent(e);
}

void SW::GLViewer::setGL(void){
    glClearColor( .5, .5, .5, 1. );
    setLighting();
}

void SW::GLViewer::setLighting(void){
    GLfloat position[4] = { 20., 30., 40., 0. };
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glEnable( GL_LIGHT0 );
    glEnable( GL_NORMALIZE );

}

void SW::GLViewer::initGLSL(){
    m_shader.loadVertex( "shaders/vertex.glsl" );
    m_shader.loadFragment( "shaders/fragment.glsl" );
}

void SW::GLViewer::setMeshMaterial(){

    GLfloat  diffuse[4] = { .8, .5, .3, 1. };
    GLfloat specular[4] = { .3, .3, .3, 1. };
    GLfloat  ambient[4] = { .2, .2, .5, 1. };

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse  );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient  );
    glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, 16.      );
}

void SW::GLViewer::toggleDisplayVertices()
{
    displayType = VERTICES;
    updateGL();
}

void SW::GLViewer::toggleDisplayWireFrame()
{
    displayType = WIREFRAME;
    updateGL();
}

void SW::GLViewer::toggleDisplayFlatLine()
{
    displayType = FLATLINE;
    updateGL();
}

void SW::GLViewer::addMesh(const Mesh &mesh)
{
    meshes.append(mesh);
}

int SW::GLViewer::getMeshNum()
{
    return meshes.size();
}

Mesh & SW::GLViewer::getMesh(int index)
{
    return meshes[index];
}

void SW::GLViewer::removeAllMeshes()
{
    meshes.clear();
}
