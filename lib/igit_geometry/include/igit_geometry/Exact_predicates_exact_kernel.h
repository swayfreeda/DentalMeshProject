
#ifndef CGAL_EXACT_PREDICATES_EXACT_CONSTRUCTIONS_KERNEL_H
#define CGAL_EXACT_PREDICATES_EXACT_CONSTRUCTIONS_KERNEL_H


namespace CGAL {

#include <igit_geometry/Lazy_exact_nt_.h>
#include <igit_geometry/E_selector_new.h>   // got it
#include<igit_geometry/Filtered_kernel.h>
#include<igit_geometry/Kernel_Type_equality_wrapper.h>
#include<igit_geometry/Lazy_kernel.h>
typedef internal::Exact_field_selector<double>::Type Epeck_ft;

template < typename T >
struct Simple_cartesian_new;
class Epeck
  : public internal::Static_filters<
      Type_equality_wrapper<
             Lazy_kernel_base< Simple_cartesian<Epeck_ft>,
                               Simple_cartesian<Interval_nt_advanced>,
                           Cartesian_converter< Simple_cartesian<Epeck_ft>,
                                    Simple_cartesian<Interval_nt_advanced> >,
                               Epeck>,
             Epeck >, false>
{};




typedef Epeck Exact_predicates_exact_constructions_kernel;

} //namespace CGAL

#endif // CGAL_EXACT_PREDICATES_EXACT_CONSTRUCTIONS_KERNEL_H
