#ifndef SW_GLVIEWER_H
#define SW_GLVIEWER_H

#include"QGLViewer/qglviewer.h"
#include"Shader.h"
#include"Types.h"

#include<list>
#include<vector>
#include<QObject>
#include<QMouseEvent>
#include<QMenu>
#include<QAction>
# include <QMenu>
#include <qcursor.h>
#include <qmap.h>
#include <math.h>
#include<QColor>
#include<QListWidgetItem>
#include <QImage>
#include<QKeyEvent>

#include<QVector2D>
#include<QVector3D>
#include"opencv2/opencv.hpp"
#include<QThread>


namespace SW{


class  GLViewer : public QGLViewer
{
    Q_OBJECT

public:

    enum DispalyType{VERTICES, WIREFRAME, FLAT};
    // GLViewer(QWidget *parent0=0, const QGLWidget *parent1=0, Qt::WFlags f = 0);
    GLViewer(QWidget *parent0=0, const char *parent1=0, QGLWidget *f = 0);
    ~GLViewer();

    virtual void init();
    virtual void draw();
    virtual QString helpString();
    virtual void transform(){}

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);

    virtual void keyPressEvent(QKeyEvent *e);

    void viewAll();

    static void setGL( void );
    static void setLighting( void );
    static void setMeshMaterial( void );
    static void initGLSL( void );

public:
    //draw world coordinates
    void drawAxises(double width, double length);

public slots:
    virtual void drawText(){}
    void toggleDisplayVertices(){
     displayType = VERTICES;
     updateGL();
    }
    void toggleDisplayWireFrame(){
        displayType = WIREFRAME;
        updateGL();
    }

    void toggleDisplayFlat(){
        displayType = FLAT;
        updateGL();
    }



signals:
public:
     std::vector<SW::Mesh> meshes;
private:
      static Shader shader;
      DispalyType displayType;

};

}
#endif // SW_GLVIEWER_H
