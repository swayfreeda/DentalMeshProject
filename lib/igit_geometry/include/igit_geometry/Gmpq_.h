

#ifndef CGAL_GMPQ_H
#define CGAL_GMPQ_H


//#include <CGAL/Gmp_coercion_traits.h>

#include <igit_geometry/Gmpq_type_.h>

#include <igit_geometry/Coercion_traits.h>

namespace CGAL {

// AST for Gmpq-class
template <> class Algebraic_structure_traits< Gmpq >
  : public Algebraic_structure_traits_base< Gmpq, Field_tag >  {};

// RET for Gmpq-class

template <> class Real_embeddable_traits< Gmpq >
  : public INTERN_RET::Real_embeddable_traits_base< Gmpq , CGAL::Tag_true > {
  public:

    class To_double
      : public std::unary_function< Type, double > {
      public:
        double operator()( const Type& x ) const {
          return x.to_double();
        }
    };

    class To_interval
      : public std::unary_function< Type, std::pair< double, double > > {
      public:
        std::pair<double, double> operator()( const Type& x ) const {
          mpfr_t y;
          mpfr_init2 (y, 53); /* Assume IEEE-754 */
          mpfr_set_q (y, x.mpq(), GMP_RNDD);
          double i = mpfr_get_d (y, GMP_RNDD); /* EXACT but can overflow */
          mpfr_set_q (y, x.mpq(), GMP_RNDU);
          double s = mpfr_get_d (y, GMP_RNDU); /* EXACT but can overflow */
          mpfr_clear (y);
          return std::pair<double, double>(i, s);
        }
    };
};



} //namespace CGAL


#endif // CGAL_GMPQ_H
