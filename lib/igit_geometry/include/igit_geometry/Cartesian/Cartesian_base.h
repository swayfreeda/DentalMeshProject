// Copyright (c) 2000-2004  
// Utrecht University (The Netherlands),
// ETH Zurich (Switzerland),
// INRIA Sophia-Antipolis (France),
// Max-Planck-Institute Saarbruecken (Germany),
// and Tel-Aviv University (Israel).  All rights reserved. 
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
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
// Author(s)     : Sylvain Pion

#ifndef CGAL_CARTESIAN_BASE_H
#define CGAL_CARTESIAN_BASE_H

#include <igit_geometry/basic_.h>
#include <igit_geometry/basic_classes.h>
//#include <CGAL/Kernel/global_functions.h>

#include <igit_geometry/Cartesian/Point_2.h>
#include <igit_geometry/Cartesian/Vector_2.h>
#include <igit_geometry/Cartesian/Direction_2.h>
#include <igit_geometry/Cartesian/Line_2.h>
#include <igit_geometry/Cartesian/Ray_2.h>
#include <igit_geometry/Cartesian/Segment_2.h>
#include <igit_geometry/Cartesian/Triangle_2.h>
#include <igit_geometry/Cartesian/Circle_2.h>
#include <igit_geometry/Cartesian/Iso_rectangle_2.h>
#include <igit_geometry/Cartesian/Aff_transformation_2.h>
#include <igit_geometry/Cartesian/Data_accessor_2.h>
//#include <CGAL/ConicCPA2.h>

//#include <igit_geometry/Cartesian/predicates_on_points_2.h>
//#include <igit_geometry/Cartesian/predicates_on_directions_2.h>
//#include <igit_geometry/Cartesian/basic_constructions_2.h>

#include <igit_geometry/Cartesian/Point_3.h>
#include <igit_geometry/Cartesian/Vector_3.h>
#include <igit_geometry/Cartesian/Direction_3.h>
#include <igit_geometry/Cartesian/Line_3.h>
#include <igit_geometry/Cartesian/Plane_3.h>
#include <igit_geometry/Cartesian/Ray_3.h>
#include <igit_geometry/Cartesian/Segment_3.h>
#include <igit_geometry/Cartesian/Triangle_3.h>
#include <igit_geometry/Cartesian/Tetrahedron_3.h>
#include <igit_geometry/Cartesian/Iso_cuboid_3.h>
#include <igit_geometry/Cartesian/Sphere_3.h>
#include <igit_geometry/Cartesian/Circle_3.h>
#include <igit_geometry/Cartesian/Aff_transformation_3.h>

#include <igit_geometry/Cartesian/predicates_on_points_3.h>
#include <igit_geometry/Cartesian/predicates_on_planes_3.h>
#include <igit_geometry/Cartesian/basic_constructions_3.h>

//#include <CGAL/representation_tags.h>
#include <igit_geometry/Cartesian/function_objects.h>
#include <igit_geometry/Uncertain.h>

namespace CGAL {

template < typename K_, typename FT_>
struct Cartesian_base
{
    typedef K_                                          Kernel;
    typedef FT_                                         FT;
    typedef Cartesian_base<K_,FT_>                      Self;
    typedef Cartesian_tag                               Rep_tag;
    typedef Cartesian_tag                               Kernel_tag;

    enum { Has_filtered_predicates = false };
    typedef Boolean_tag<Has_filtered_predicates> Has_filtered_predicates_tag;

    typedef CGAL::Object                                Object_2;
    typedef CGAL::Object                                Object_3;

    // Boolean   had originally been Bool. It was renamed to avoid a conflict
    // between a macro defined in Xlib.h poorly chosen to have the same name,
    // that is 'Bool'.
    typedef typename Same_uncertainty_nt<bool, FT>::type
                                                        Boolean;
    typedef typename Same_uncertainty_nt<CGAL::Sign, FT>::type
                                                        Sign;
    typedef typename Same_uncertainty_nt<CGAL::Comparison_result, FT>::type
                                                        Comparison_result;
    typedef typename Same_uncertainty_nt<CGAL::Orientation, FT>::type
                                                        Orientation;
    typedef typename Same_uncertainty_nt<CGAL::Oriented_side, FT>::type
                                                        Oriented_side;
    typedef typename Same_uncertainty_nt<CGAL::Bounded_side, FT>::type
                                                        Bounded_side;
    typedef typename Same_uncertainty_nt<CGAL::Angle, FT>::type
                                                        Angle;

    template <typename T>
    struct Ambient_dimension {
      typedef typename T::Ambient_dimension type;
    };

    template <typename T>
    struct Feature_dimension {
      typedef typename T::Feature_dimension type;
    };

    typedef PointC2<Kernel>                             Point_2;
    typedef VectorC2<Kernel>                            Vector_2;
    typedef DirectionC2<Kernel>                         Direction_2;
    typedef SegmentC2<Kernel>                           Segment_2;
    typedef LineC2<Kernel>                              Line_2;
    typedef RayC2<Kernel>                               Ray_2;
    typedef TriangleC2<Kernel>                          Triangle_2;
    typedef CircleC2<Kernel>                            Circle_2;
    typedef Iso_rectangleC2<Kernel>                     Iso_rectangle_2;
    typedef Aff_transformationC2<Kernel>                Aff_transformation_2;

    typedef PointC3<Kernel>                             Point_3;
    typedef VectorC3<Kernel>                            Vector_3;
    typedef DirectionC3<Kernel>                         Direction_3;
    typedef LineC3<Kernel>                              Line_3;
    typedef PlaneC3<Kernel>                             Plane_3;
    typedef RayC3<Kernel>                               Ray_3;
    typedef SegmentC3<Kernel>                           Segment_3;
    typedef TriangleC3<Kernel>                          Triangle_3;
    typedef TetrahedronC3<Kernel>                       Tetrahedron_3;
    typedef Iso_cuboidC3<Kernel>                        Iso_cuboid_3;
    typedef SphereC3<Kernel>                            Sphere_3;
    typedef CircleC3<Kernel>                            Circle_3;
    typedef Aff_transformationC3<Kernel>                Aff_transformation_3;

    typedef typename cpp11::array<FT_, 2>::const_iterator Cartesian_const_iterator_2;
    typedef typename cpp11::array<FT_, 3>::const_iterator Cartesian_const_iterator_3;

    // Undocumented stuff.
    typedef Data_accessorC2<Kernel>                     Data_accessor_2;
    //typedef ConicCPA2<Point_2,Data_accessor_2>          Conic_2;
};

} //namespace CGAL

#endif // CGAL_CARTESIAN_BASE_H
