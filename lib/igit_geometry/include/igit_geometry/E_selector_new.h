#ifndef CGAL_INTERNAL_EXACT_TYPE_SELECTOR_H
#define CGAL_INTERNAL_EXACT_TYPE_SELECTOR_H
//exact_type_selector
#include <igit_geometry/Lazy_exact_nt_.h>
#include <igit_geometry/Gmpq_.h>

namespace CGAL { namespace internal {
template < typename >
struct Exact_field_selector
#ifdef CGAL_USE_GMP
{ typedef Gmpq Type; };
#elif defined(CGAL_USE_LEDA)
{ typedef leda_rational Type; };
#else
{ typedef Quotient<MP_Float> Type; };
#endif

//template <>
//struct Exact_ring_selector<double>
//#ifdef CGAL_HAS_MPZF
//{ typedef Mpzf Type; };
//#elif defined(CGAL_HAS_THREADS) || !defined(CGAL_USE_GMP)
////{ typedef MP_Float Type; };
////#else
//{ typedef Gmpzf Type; };
//#endif



} } // namespace CGAL::internal

#endif // CGAL_INTERNAL_EXACT_TYPE_SELECTOR_H
