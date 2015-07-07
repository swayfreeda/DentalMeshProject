#ifndef MESH_H
#define MESH_H

//
//  BoundingBox.h
//  Reconstructor
//
//  Created by sway on 6/13/15.
//  Copyright (c) 2015 None. All rights reserved.
//

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "include/BoundingBox.h"

namespace SW{

class Mesh:public OpenMesh::TriMesh_ArrayKernelT<>
{
  public:
    Mesh(QString name);
    Mesh();

    void getBoundingBox();

    //计算BoundingBox
    void computeBoundingBox();

    // 0--vertices 1-- wireframe 2-- flatLine
    void draw(int flag);

   public:
    BoundingBox BBox;
    QString MeshName;

};
}
#endif // MESH_H
