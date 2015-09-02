#ifndef SW_GLVIEWER_H
#define SW_GLVIEWER_H

//
//  BoundingBox.h
//  Reconstructor
//
//  Created by sway on 6/13/15.
//  Copyright (c) 2015 None. All rights reserved.
//


#include "include/Mesh.h"
#include "QGLViewer/qglviewer.h"
#include "include/Shader.h"

#include <QObject>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QMenu>
#include <qcursor.h>
#include <qmap.h>
#include <QColor>
#include <QListWidgetItem>
#include <QImage>
#include <QKeyEvent>
#include <QVector2D>
#include <QVector3D>
#include <QThread>
#include <list>
#include <vector>
#include <math.h>

namespace SW{

class  GLViewer : public QGLViewer
{
    Q_OBJECT

public:
     enum DispalyType{VERTICES, WIREFRAME, FLATLINE};
      GLViewer(QWidget *parent0=0, const QGLWidget *parent1=0, Qt::WindowFlags f = 0);
     // GLViewer(QWidget *parent0=0, QGLWidget *f = 0, Qt::WindowFlags flag);
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

    void addMesh(const Mesh &mesh);

    int getMeshNum();

    Mesh getMesh(int index);

    void removeAllMeshes();

    //为了解决每次重新构建项目时都需要注释ui_mainwindow.h中的两行的问题，添加下面两个空方法
    void setFrameShape(QFrame::Shape){}
    void setFrameShadow(QFrame::Shadow){}

protected:
    //draw world coordinates
    void drawAxises(double width, double length);
    void setGL(void);
    void setLighting(void);
    void initGLSL();
    void setMeshMaterial();

public slots:
    virtual void drawText(){}

    void toggleDisplayVertices();

    void toggleDisplayWireFrame();

    void toggleDisplayFlatLine();


signals:



private:
    static SW::Shader m_shader;
    float m_length;
    QVector<Mesh> meshes;

    bool displayVertices;
    bool displayWireFrame;
    bool displayFlatLine;

    DispalyType displayType;

};

}
#endif // SW_GLVIEWER_H
