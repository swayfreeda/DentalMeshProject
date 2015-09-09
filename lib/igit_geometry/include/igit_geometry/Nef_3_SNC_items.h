// Copyright (c) 1997-2002  Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Michael Seel        <seel@mpi-sb.mpg.de>
//                 Miguel Granados     <granados@mpi-sb.mpg.de>
//                 Susan Hert          <hert@mpi-sb.mpg.de>
//                 Lutz Kettner        <kettner@mpi-sb.mpg.de>
//                 Peter Hachenberger  <hachenberger@mpi-sb.mpg.de>
#ifndef CGAL_SNC_ITEMS_H
#define CGAL_SNC_ITEMS_H

#include <igit_geometry/Nef_3_Vertex.h>
#include <igit_geometry/Nef_3_Halfedge.h>
#include <igit_geometry/Nef_3_Halffacet.h>
#include <igit_geometry/Nef_3_Volume.h>
#include <igit_geometry/Nef_3_SHalfedge.h>
#include <igit_geometry/Nef_3_SHalfloop.h>
#include <igit_geometry/Nef_3_SFace.h>

#undef CGAL_NEF_DEBUG
#define CGAL_NEF_DEBUG 83
#include <igit_geometry/Nef_2_debug.h>

namespace CGAL {

template <typename K, typename I, typename M> class SNC_sphere_map;
template <typename R> class SM_decorator;

class SNC_items {
 public:
  template <class Refs> class Vertex    :    public Vertex_base<Refs> {};
  template <class Refs> class SVertex   :    public Halfedge_base<Refs> {};
  template <class Refs> class Halffacet :    public Halffacet_base<Refs> {};
  template <class Refs> class Volume    :    public Volume_base<Refs> {};
  template <class Refs> class SHalfedge :    public SHalfedge_base<Refs> {};
  template <class Refs> class SHalfloop :    public SHalfloop_base<Refs> {};
  template <class Refs> class SFace     :    public SFace_base<Refs> {};
}; // SNC_items



} //namespace CGAL
#endif //CGAL_SNC_ITEMS_H
