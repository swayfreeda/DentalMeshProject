#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H


//
//  BoundingBox.h
//  Reconstructor
//
//  Created by sway on 6/12/15.
//  Copyright (c) 2015 None. All rights reserved.
//
#include"QGLViewer/qglviewer.h"
#include "Types.h"

class BoundingBox {

public:
    BoundingBox(){}
    ~BoundingBox(){}
    SW::Vector origin;
    SW::Vector size;
};
#endif // BOUNDINGBOX_H
