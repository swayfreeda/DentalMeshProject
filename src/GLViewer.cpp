
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
#define mhw_Debug//mhw调试输出
#define BUFFER_OFFSET(offset)  ((GLvoid*) NULL + offset)
#define NumberOf(array) (sizeof(array)/sizeof(array[0])

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SW::GLViewer::GLViewer(QWidget *parent, const char * name, Qt::WFlags f)
//{
//    setAutoFillBackground(true);// painter auto clear the background
//}

SW::Shader SW::GLViewer::m_shader;


#if QT_VERSION <   QT_VERSION_CHECK(5, 0 , 0)
SW::GLViewer::GLViewer(QWidget *parent0,  const char *parent1, QGLWidget*f):
    QGLViewer(parent0, parent1, f)
{
    //setAutoFillBackground(true);
    displayType = FLATLINE;

    m_length = 0.1;

    mCallSuperMousePressEvent = true;
    mCallSuperMouseMoveEvent = true;
    mCallSuperMouseReleaseEvent = true;
    mCallSuperMouseDoubleClickEvent = true;
    mCallSuperWheelEvent = true;
    mCallSuperKeyPressEvent = true;
    mCallSuperKeyReleaseEvent = true;

    //********************************//
    //2015/09/07
    //mhw merge code
    //********************************//
    select_vertices_mode = false;
    MovePoints_mode=false;
    P_OnMoving=false;
    OnLaplacian=false;
    m_length = 0.1;
    DrawRect=false;
    RunningModel=Default;//初始化的模式为空
    //********************************//
}
#else
SW::GLViewer::GLViewer(QWidget *parent0, const QGLWidget *parent1, Qt::WindowFlags f ):
    QGLViewer(parent0, parent1, f)
{
    //setAutoFillBackground(true);
    displayType = FLATLINE;

    m_length = 0.1;

    mCallSuperMousePressEvent = true;
    mCallSuperMouseMoveEvent = true;
    mCallSuperMouseReleaseEvent = true;
    mCallSuperMouseDoubleClickEvent = true;
    mCallSuperWheelEvent = true;
    mCallSuperKeyPressEvent = true;
    mCallSuperKeyReleaseEvent = true;

    mCurrentProcessMode = NONE;
    //********************************//
    //2015/09/07
    //mhw merge code
    //********************************//
    select_vertices_mode = false;
    MovePoints_mode=false;
    P_OnMoving=false;
    OnLaplacian=false;
    m_length = 0.1;
    DrawRect=false;
    //********************************//
}
#endif

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
    drawAxises(0.1, m_length);
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    setMeshMaterial();
    foreach(SW::Mesh mesh, meshes){
        glPushMatrix();
        //mesh.draw(displayType);//mhw改201509079
        mesh.draw(displayType,Select_P_Array,MoveVectors);
        glPopMatrix();
    }
    glPopAttrib();

    if(DrawRect && select_vertices_mode)
        drawSelectingWindow();
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
    renderText(axisLength, 0.0, 0.0, "X", QFont("helvetica", 12, QFont::Bold));

    //qglColor(Qt::green);
    glColor3f(0.0, 1.0, 0.0);
    renderText(0.0, axisLength, 0.0, "Y", QFont("helvetica", 12, QFont::Bold));

    //qglColor(Qt::blue);
    glColor3f(0.0, 0.0, 1.0);
    renderText(0.0, 0.0, axisLength, "Z", QFont("helvetica", 12, QFont::Bold));

}


//////////////////////////////////////////////////
//mhw copy
//////////////////////////////////////////////////
void SW::GLViewer::mousePressEvent(QMouseEvent *e)
{

    qglviewer::Vec p3D(e->pos().x(),e->pos().y(),0);
    qglviewer::Vec p2D = camera()->unprojectedCoordinatesOf(p3D,NULL);
    qDebug()<<"3D:X_"<<p2D.x<<"Y_"<<p2D.y<<"Z_"<<p2D.z<<endl;
    /*TODO*/
    // initialized selecting window
    if(MovePoints_mode && mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){
        int S_Num=0;//按下的是那个点集;
        if(IsSelectPoint(0,e->pos().x(),e->pos().y(),&S_Num)){
            P_OnMoving=true;
            Cur_choose_P=S_Num;
            qglviewer::Vec p2D(e->pos().x(),e->pos().y(),0);
            qglviewer::Vec p3D = camera()->unprojectedCoordinatesOf(p2D,NULL);
            qDebug()<<"3D:X_"<<p3D.x<<"Y_"<<p3D.y<<"Z_"<<p3D.z<<endl;
            PressMouse_pos.x=p3D.x;
            PressMouse_pos.y=p3D.y;
            PressMouse_pos.z=p3D.z;
            updateGL();
            // qDebug() << "S_Num:"<<S_Num<< endl;
        }
    }
    if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ControlModifier)&&select_vertices_mode&& mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){

        DrawRect = true;
        selecting_window = QRect(e->pos(), e->pos());
        //qDebug() << "mousePressEvent"<<e->pos().x() << ", " << e->pos().y() << endl;

        updateGL();
    }
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onMousePressed(e);

        if(mCallSuperMousePressEvent){
            QGLViewer::mousePressEvent(e);
        }
    }
    else{
        //qDebug() << "NomousePressEvent"<<e->pos().x() << ", " << e->pos().y() << endl;
        QGLViewer::mousePressEvent(e);

    }

}

void SW::GLViewer::mouseMoveEvent(QMouseEvent *e){
    /*TODO*/
    // selecting vertices
    // set color
    // current_selected_vertices  (e->modifiers() == Qt::ControlModifier)

    if(MovePoints_mode&&P_OnMoving&& mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){

        qglviewer::Vec p3D(e->pos().x(),e->pos().y(),0);
        qglviewer::Vec p2D = camera()->unprojectedCoordinatesOf(p3D,NULL);
        qDebug()<<"3D:X_"<<p2D.x<<"Y_"<<p2D.y<<"Z_"<<p2D.z<<endl;
        CurMouse_pos.x=p2D.x;
        CurMouse_pos.y=p2D.y;
        CurMouse_pos.z=p2D.z;

        //实时变形
        //        MoveVectors.X_arr[Cur_choose_P]=(CurMouse_pos.x-PressMouse_pos.x);
        //        MoveVectors.Y_arr[Cur_choose_P]=(CurMouse_pos.y-PressMouse_pos.y);
        //        MoveVectors.Z_arr[Cur_choose_P]=(CurMouse_pos.z-PressMouse_pos.z);
        MoveVectors.X_arr[Cur_choose_P]= MoveVectors.X_arr[Cur_choose_P]+(CurMouse_pos.x-PressMouse_pos.x);
        MoveVectors.Y_arr[Cur_choose_P]=MoveVectors.Y_arr[Cur_choose_P]+(CurMouse_pos.y-PressMouse_pos.y);
        MoveVectors.Z_arr[Cur_choose_P]=MoveVectors.Z_arr[Cur_choose_P]+(CurMouse_pos.z-PressMouse_pos.z);
        PressMouse_pos.x=CurMouse_pos.x;
        PressMouse_pos.y=CurMouse_pos.y;
        PressMouse_pos.z=CurMouse_pos.z;
        // L实时变形/***************************************************/

        if((fabs(MoveVectors.Z_arr[Cur_choose_P])+fabs(MoveVectors.X_arr[Cur_choose_P])+fabs(MoveVectors.Y_arr[Cur_choose_P]))<2){
            //MHW::LTransform TryL(this->meshes[0],"./",10000,0,this->meshes[0].n_vertices());
            Do_L.P->Run(this->meshes[0],Select_P_Array,MoveVectors);

            meshes[0].writeModel("tets.obj");
            updateGL();
            //     viewAll();
            return;
        }
    }
    if ((e->modifiers() == Qt::ControlModifier)&&select_vertices_mode&&mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){
        selecting_window.setBottomRight(e->pos());
        updateGL();
    }
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onMouseMoved(e);
        if(mCallSuperMouseMoveEvent) {
            QGLViewer::mouseMoveEvent(e);
        }
    }
    else{
        //  qDebug() << e->pos().x() << ", " << e->pos().y() << endl;
        QGLViewer::mouseMoveEvent(e);
        return;
    }
}

void SW::GLViewer::mouseReleaseEvent(QMouseEvent *e){

    if(MovePoints_mode&&P_OnMoving&&mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){
        P_OnMoving=false;
        ReleaseMouse_pos.x=e->pos().x();
        ReleaseMouse_pos.y=e->pos().y();
        ////放开鼠标开始发生形变
        //        MHW::LTransform TryL(this->meshes[0],"./",Select_P_Array,MoveVectors,10000,0,this->meshes[0].n_vertices());
        //        TryL.Run(this->meshes[0]);
        //        MoveVectors.X_arr[Cur_choose_P]=(ReleaseMouse_pos.x-PressMouse_pos.x);
        //        MoveVectors.Y_arr[Cur_choose_P]=(ReleaseMouse_pos.y-PressMouse_pos.y);
    }
    if(select_vertices_mode ==true&&DrawRect&&mCurrentProcessMode == LAPLACIAN_TRANSFORM_MODE){
        DrawRect=false;
        handleSelectPoint(0);
        MoveVectors.X_arr.append(0);
        MoveVectors.Y_arr.append(0);//同时添加偏移向量集合;
        MoveVectors.Z_arr.append(0);
        updateGL();
    }
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onMouseReleased(e);
        if(mCallSuperMouseReleaseEvent){
            QGLViewer::mouseReleaseEvent(e);
        }
    }
    else{
        QGLViewer::mouseReleaseEvent(e);
    }
}

void SW::GLViewer::drawSelectingWindow()
{
    startScreenCoordinatesSystem();
    glEnable(GL_BLEND);

    //the select window
    glLineWidth(2.0);
    glColor4f(0.4f, 0.9f, 0.1f, 0.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(selecting_window.left(), selecting_window.top());
    glVertex2i(selecting_window.right(),selecting_window.top());
    glVertex2i(selecting_window.right(), selecting_window.bottom());
    glVertex2i(selecting_window.left(), selecting_window.bottom());
    glEnd();

    glDisable(GL_BLEND);
    stopScreenCoordinatesSystem();
}


void SW::GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onMouseDoubleClicked(e);

        if(mCallSuperMouseDoubleClickEvent) {
            QGLViewer::mouseDoubleClickEvent(e);
        }
    }
    else{
        QGLViewer::mouseDoubleClickEvent(e);
    }
}

void SW::GLViewer::wheelEvent(QWheelEvent *e)
{
    onWheeled(e);

    int numDegrees = e->delta() / 8;
    //int numSteps = numDegrees / 15;
    //    if(numSteps >0)
    //    m_length = 0.1/(float)numSteps;
    //    else
    //    m_length = -0.1*(float)numSteps;



    //    //imageScale();
    e->accept();
    update();

    if(mCallSuperWheelEvent) {
        QGLViewer::wheelEvent(e);
    }
}

void SW::GLViewer::keyPressEvent(QKeyEvent *e)
{
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onKeyPressed(e);

        if(mCallSuperKeyPressEvent) {
            QGLViewer::keyPressEvent(e);
        }
    }
    else{
        QGLViewer::keyPressEvent(e);
    }
}

void SW::GLViewer::keyReleaseEvent(QKeyEvent *e)
{
    if(mCurrentProcessMode == SEGMENTATION_MODE){
        onKeyReleased(e);

        if(mCallSuperKeyReleaseEvent)
        {
            QGLViewer::keyReleaseEvent(e);
        }
    }
    else{
        QGLViewer::keyReleaseEvent(e);
    }
}

void SW::GLViewer::setCallSuperMousePressEvent(bool callSuperEvent)
{
    mCallSuperMousePressEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperMouseMoveEvent(bool callSuperEvent)
{
    mCallSuperMouseMoveEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperMouseReleaseEvent(bool callSuperEvent)
{
    mCallSuperMouseReleaseEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperMouseDoubleClickEvent(bool callSuperEvent)
{
    mCallSuperMouseDoubleClickEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperWheelEvent(bool callSuperEvent)
{
    mCallSuperWheelEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperKeyPressEvent(bool callSuperEvent)
{
    mCallSuperKeyPressEvent = callSuperEvent;
}

void SW::GLViewer::setCallSuperKeyReleaseEvent(bool callSuperEvent)
{
    mCallSuperKeyReleaseEvent = callSuperEvent;
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

//******************************************************************************//
//2015/09/07
//mhw merge code
//******************************************************************************//
void SW::GLViewer::toggleSelectingVertexMode(bool checked){

    select_vertices_mode = checked;
    if(select_vertices_mode==true){
        setMouseTracking(true);
    }else{
        setMouseTracking(false);
    }
    updateGL();
}
void SW::GLViewer::toggleMovePoints(bool checked){

    MovePoints_mode=checked;
    //qDebug() << "toggleMovePoints" << endl;
    updateGL();
}


void SW::GLViewer::toggleModelReset(){

    meshes[0]=Do_L.P->objMesh;
    for(int i=0;i<MoveVectors.X_arr.size();i++){
        MoveVectors.X_arr[i]=0;
        MoveVectors.Y_arr[i]=0;
        MoveVectors.Z_arr[i]=0;
    }
    Select_P_Array.empty();
    Select_P_Array.clear();
    updateGL();
}

void SW::GLViewer::addMesh(const Mesh &mesh){

    meshes.append(mesh);
}

int SW::GLViewer::getMeshNum(){
    return meshes.size();
}

Mesh & SW::GLViewer::getMesh(int index){
    return meshes[index];
}

void SW::GLViewer::removeAllMeshes(){
    meshes.clear();
}


//*****************************************************************************//
//2015/09/07/
//mhw merge code
//*****************************************************************************//

void SW::GLViewer::handleSelectPoint(int meshesNum){
    QVector<int> tempP;

    for(auto it= meshes[meshesNum].vertices_begin();it!= meshes[meshesNum].vertices_end();++it){//MHW::Mesh::VertexIter
        auto point=meshes[meshesNum].point(it.handle());//OpenMesh::Vec3f
        qglviewer::Vec p3D(point[0],point[1],point[2]);
        qglviewer::Vec p2D = camera()->projectedCoordinatesOf(p3D, NULL);

        if (selecting_window.contains(QPoint((int)p2D.x, (int)p2D.y)))
        {
            tempP.append(it.handle().idx());

        }

    }
    Select_P_Array.append(tempP);


}
QVector<QRect> SW::GLViewer::Get2D_SP_Rect(int meshesNum){
    QVector<QRect> Ret_R;
    for(int i=0;i<Select_P_Array.size();i++){


        double MaxX,MaxY,MinX,MinY;

        OpenMesh::VertexHandle temV1(Select_P_Array[i][0]);
        auto point=meshes[meshesNum].point(temV1);//OpenMesh::Vec3f
        qglviewer::Vec p3D(point[0],point[1],point[2]);
        qglviewer::Vec p2D = camera()->projectedCoordinatesOf(p3D, NULL);
        MaxX=p2D.x;
        MaxY=p2D.y;
        MinX=p2D.x;
        MinY=p2D.y;
        for(int j=1;j<Select_P_Array[i].size();j++){

            OpenMesh::VertexHandle temV(Select_P_Array[i][j]);
            auto point=meshes[meshesNum].point(temV);//OpenMesh::Vec3f
            qglviewer::Vec p3D(point[0],point[1],point[2]);
            qglviewer::Vec p2D = camera()->projectedCoordinatesOf(p3D, NULL);
            // camera()->projectedCoordinatesOf()
            // camera()->cameraCoordinatesOf()
            if(p2D.x>MaxX){
                MaxX= p2D.x;
            }
            if(p2D.y>MaxY){
                MaxY=p2D.y;
            }
            if(p2D.x<MinX){
                MinX= p2D.x;
            }
            if(p2D.y<MinY){
                MinY=p2D.y;
            }
        }

        QRect tempR(QPoint((int)MaxX, (int)MaxY),QPoint((int)MinX, (int)MinY));
        Ret_R.append(tempR);
    }
    return Ret_R;

}
bool SW::GLViewer::IsSelectPoint(int meshesNum,int x,int y,int *S_array_Num){
    QVector<QRect> R_array=Get2D_SP_Rect(meshesNum);
    for(int i=0;i<R_array.size();i++){
        if (R_array[i].contains(QPoint(x, y)))
        {
            *(S_array_Num)=i;
            return true;
        }
    }

    return false;
}

