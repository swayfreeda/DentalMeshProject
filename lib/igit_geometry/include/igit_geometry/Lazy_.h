#ifndef LAZY_KERNEL_H
#define LAZY_KERNEL_H


#include <igit_geometry/Handle_.h>
#include <igit_geometry/Default.h>
#include <igit_geometry/Min_Max.h>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/has_xxx.hpp>

#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum.hpp>

namespace CGAL {

template <typename AT, typename ET, typename EFT, typename E2A> class Lazy;

template <typename ET_>
class Lazy_exact_nt;

template <typename AT, typename ET, typename EFT, typename E2A>
inline
const AT&
approx(const Lazy<AT,ET, EFT, E2A>& l)
{
  return l.approx();
}



template <typename AT, typename ET, typename EFT, typename E2A>
inline
const ET&
exact(const Lazy<AT,ET,EFT,E2A>& l)
{
  return l.exact();
}


template <typename AT, typename ET, typename EFT, typename E2A>
inline
unsigned
depth(const Lazy<AT,ET,EFT,E2A>& l)
{
  return l.depth();
}


#define CGAL_LAZY_FORWARD(T) \
  inline const T & approx(const T& d) { return d; } \
  inline const T & exact (const T& d) { return d; } \
  inline unsigned  depth (const T&  ) { return 0; }


CGAL_LAZY_FORWARD(long double)
CGAL_LAZY_FORWARD(double)
CGAL_LAZY_FORWARD(float)
CGAL_LAZY_FORWARD(int)
CGAL_LAZY_FORWARD(unsigned int)
CGAL_LAZY_FORWARD(long)
CGAL_LAZY_FORWARD(unsigned long)

CGAL_LAZY_FORWARD(Return_base_tag)
CGAL_LAZY_FORWARD(Null_vector)
CGAL_LAZY_FORWARD(Origin)
CGAL_LAZY_FORWARD(Orientation)
CGAL_LAZY_FORWARD(Bbox_2)
CGAL_LAZY_FORWARD(Bbox_3)

#undef CGAL_LAZY_FORWARD


struct Depth_base {
#ifdef CGAL_PROFILE

#else
 unsigned depth() const { return 0; }
  void set_depth(unsigned) {}
#endif
};


// Abstract base class for lazy numbers and lazy objects
template <typename AT_, typename ET, typename E2A>
class Lazy_rep : public Rep, public Depth_base
{
  //Lazy_rep (const Lazy_rep&); // cannot be copied.

public:

  typedef AT_ AT;

  mutable AT at;
  mutable ET *et;

  Lazy_rep ()
    : at(), et(NULL){}

  Lazy_rep (const AT& a)
      : at(a), et(NULL){}

  Lazy_rep (const AT& a, const ET& e)
      : at(a), et(new ET(e)) {}

  const AT& approx() const
  {
      return at;
  }

  AT& approx()
  {
      return at;
  }

  const ET & exact() const
  {
    if (et==NULL)
      update_exact();
    return *et;
  }



  bool is_lazy() const { return et == NULL; }
  virtual void update_exact() const = 0;
  virtual ~Lazy_rep() { delete et; }
};


//____________________________________________________________
// The rep for the leaf node

template <typename AT, typename ET, typename E2A>
class Lazy_rep_0 : public Lazy_rep<AT, ET, E2A>
{

  typedef Lazy_rep<AT, ET, E2A> Base;
public:

  void
  update_exact() const
  {
    this->et = new ET();
  }

  Lazy_rep_0()
    : Lazy_rep<AT,ET, E2A>() {}

  Lazy_rep_0(const AT& a, const ET& e)
    : Lazy_rep<AT,ET,E2A>(a, e) {}

  Lazy_rep_0(const AT& a, void*)
    : Lazy_rep<AT,ET,E2A>(a) {}

  Lazy_rep_0(const ET& e)
    : Lazy_rep<AT,ET,E2A>(E2A()(e), e) {}

  void
  print_dag(std::ostream& os, int level) const
  {
    this->print_at_et(os, level);
  }
};

// Macro helpers to build the kernel objects
#define CGAL_TYPEMAP_AC(z, n, t) typedef typename Type_mapper< t##n, LK, AK >::type A##n;
#define CGAL_TYPEMAP_EC(z, n, t) typedef typename Type_mapper< t##n, LK, EK >::type E##n;
#define CGAL_LEXACT(z,n,t) CGAL::exact( l##n )
#define CGAL_LARGS(z, n, t) L##n const& l##n

#define CGAL_TMAP(z, n, d) typename Type_mapper< L##n, d##K, LK >::type
#define CGAL_PRUNE_TREE(z, n, d) l##n = L##n ();
#define CGAL_LINIT(z, n, d) l##n(l##n)
#define CGAL_LN(z, n, d) d(l##n)
#define CGAL_MLIST(z, n, d) mutable L##n l##n;

//____________________________________________________________

template <typename AT, typename ET, typename AC, typename EC, typename E2A, typename L1>
class Lazy_rep_1
  : public Lazy_rep<AT, ET, E2A>
  , private EC
{
 // typedef Lazy_rep<AT, ET, E2A> Base;

  mutable L1 l1_;

  const EC& ec() const { return *this; }

public:

  void
  update_exact() const
  {
    this->et = new ET(ec()(CGAL::exact(l1_)));
    this->at = E2A()(*(this->et));
    // Prune lazy tree
    l1_ = L1();
  }

  Lazy_rep_1(const AC& ac, const EC& ec, const L1& l1)
    : Lazy_rep<AT,ET, E2A>(ac(CGAL::approx(l1))), EC(ec), l1_(l1)
  {
    this->set_depth(CGAL::depth(l1_) + 1);
  }

#ifdef CGAL_LAZY_KERNEL_DEBUG_SHOW_TYPEID
#  define CGAL_LAZY_PRINT_TYPEID CGAL::msg(os, level, typeid(AC).name());
#else  // not CGAL_LAZY_KERNEL_DEBUG_SHOW_TYPEID
#  define CGAL_LAZY_PRINT_TYPEID
#endif // not CGAL_LAZY_KERNEL_DEBUG_SHOW_TYPEID

};

#ifdef CGAL_LAZY_KERNEL_DEBUG
#  define CGAL_PRINT_DAG_LN(z, n, d) \
  CGAL::print_dag(l##n, os, level+1);
#  define CGAL_LAZY_REP_PRINT_DAG(n)                            \

#else // not CGAL_LAZY_KERNEL_DEBUG
#  define CGAL_LAZY_REP_PRINT_DAG(n)
#endif // not CGAL_LAZY_KERNEL_DEBUG

#define CGAL_LAZY_REP(z, n, d)                                               \
  template< typename AT, typename ET, typename AC, typename EC, typename E2A, BOOST_PP_ENUM_PARAMS(n, typename L)> \
class Lazy_rep_##n :public Lazy_rep< AT, \
                                     ET, \
                                     E2A >,                             \
                    private EC                                          \
{                                                                       \
  BOOST_PP_REPEAT(n, CGAL_MLIST, _)                                          \
  const EC& ec() const { return *this; } \
public: \
  void update_exact() const { \
    this->et = new ET(ec()( BOOST_PP_ENUM(n, CGAL_LEXACT, _) ) ); \
    this->at = E2A()(*(this->et));                           \
    BOOST_PP_REPEAT(n, CGAL_PRUNE_TREE, _) \
  } \
  Lazy_rep_##n(const AC& ac, const EC&, BOOST_PP_ENUM(n, CGAL_LARGS, _)) \
    : Lazy_rep<AT, ET, E2A>(ac( BOOST_PP_ENUM(n, CGAL_LN, CGAL::approx) )), BOOST_PP_ENUM(n, CGAL_LINIT, _) \
  { this->set_depth(max_n( BOOST_PP_ENUM(n, CGAL_LN, CGAL::depth) ) + 1); }  \
                                                                        \
  CGAL_LAZY_REP_PRINT_DAG(n)                                          \
};

BOOST_PP_REPEAT_FROM_TO(2, 9, CGAL_LAZY_REP, _)

#undef CGAL_TMAP
#undef CGAL_PRUNE_TREE
#undef CGAL_LINIT
#undef CGAL_LAZY_REP
#undef CGAL_LN
#undef CGAL_MLIST
#undef CGAL_PRINT_DAG_LN
#undef CGAL_LAZY_REP_PRINT_DAG
#undef CGAL_LAZY_PRINT_TYPEID

template < typename K1, typename K2 >
struct Approx_converter
{
  typedef K1         Source_kernel;
  typedef K2         Target_kernel;
  //typedef Converter  Number_type_converter;

  template < typename T >
  const typename T::AT&
  operator()(const T&t) const
  { return t.approx(); }

  const Null_vector&
  operator()(const Null_vector& n) const
  { return n; }

  const Bbox_2&
  operator()(const Bbox_2& b) const
  { return b; }

  const Bbox_3&
  operator()(const Bbox_3& b) const
  { return b; }
};

template < typename K1, typename K2 >
struct Exact_converter
{
  typedef K1         Source_kernel;
  typedef K2         Target_kernel;
  //typedef Converter  Number_type_converter;

  template < typename T >
  const typename T::ET&
  operator()(const T&t) const
  { return t.exact(); }

  const Null_vector&
  operator()(const Null_vector& n) const
  { return n; }

  const Bbox_2&
  operator()(const Bbox_2& b) const
  { return b; }

  const Bbox_3&
  operator()(const Bbox_3& b) const
  { return b; }
};

//____________________________________________________________







//____________________________________________________________
// The handle class
template <typename AT_, typename ET_, typename EFT, typename E2A>
class Lazy : public Handle
{
public :

  typedef Lazy<AT_, ET_, EFT, E2A>  Self;
  typedef Lazy_rep<AT_, ET_, E2A>   Self_rep;

  typedef AT_ AT; // undocumented
  typedef ET_ ET; // undocumented

  typedef AT  Approximate_type;
  typedef ET  Exact_type;


  Lazy()
    : Handle(zero()) {}



  Lazy(Self_rep *r)
  {
    PTR = r;
  }

  Lazy(const ET& e)
  {
    PTR = new Lazy_rep_0<AT,ET,E2A>(e);
  }

  const AT& approx() const
  { return ptr()->approx(); }

  const ET& exact() const
  { return ptr()->exact(); }

  AT& approx()
  { return ptr()->approx(); }


  unsigned depth() const
  {
    return ptr()->depth();
  }

  void print_dag(std::ostream& os, int level) const
  {
    ptr()->print_dag(os, level);
  }

private:

  // We have a static variable for optimizing the default constructor,
  // which is in particular heavily used for pruning DAGs.
  static const Self & zero()
  {
#ifdef CGAL_HAS_THREADS
    static boost::thread_specific_ptr<Self> z;
    if (z.get() == NULL) {
        z.reset(new Self(new Lazy_rep_0<AT, ET, E2A>()));
    }
    return * z.get();
#else
    static const Self z = new Lazy_rep_0<AT, ET, E2A>();
    return z;
#endif
  }

  Self_rep * ptr() const { return (Self_rep*) PTR; }
};

// The magic functor for Construct_bbox_[2,3], as there is no Lazy<Bbox>

template <typename LK, typename AC, typename EC>
struct Lazy_construction_bbox
{
  static const bool Protection = true;
  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename AC::result_type result_type;

  AC ac;
  EC ec;

};


template <typename LK, typename AC, typename EC>
struct Lazy_construction_nt {

  static const bool Protection = true;

  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename LK::E2A E2A;

  AC ac;
  EC ec;

  template<typename>
  struct result { };

#define CGAL_RESULT_NT(z, n, d)                                              \
  template< typename F, BOOST_PP_ENUM_PARAMS(n, class T) >              \
  struct result<F( BOOST_PP_ENUM_PARAMS(n, T) )> {                      \
    BOOST_PP_REPEAT(n, CGAL_TYPEMAP_EC, T)                                   \
    typedef Lazy_exact_nt<                                              \
      typename boost::remove_cv< typename boost::remove_reference <     \
      typename cpp11::result_of<EC( BOOST_PP_ENUM_PARAMS(n, E) )>::type >::type >::type > type; \
  };

  BOOST_PP_REPEAT_FROM_TO(1, 6, CGAL_RESULT_NT, _)

#define CGAL_NT_OPERATOR(z, n, d)                                            \
  template<BOOST_PP_ENUM_PARAMS(n, class L)>                            \
  typename cpp11::result_of<Lazy_construction_nt(BOOST_PP_ENUM_PARAMS(n, L))>::type \
  operator()( BOOST_PP_ENUM(n, CGAL_LARGS, _) ) const {                      \
    BOOST_PP_REPEAT(n, CGAL_TYPEMAP_EC, L)                                     \
    BOOST_PP_REPEAT(n, CGAL_TYPEMAP_AC, L)                                     \
    typedef typename boost::remove_cv< typename boost::remove_reference < \
                                        typename cpp11::result_of< EC(BOOST_PP_ENUM_PARAMS(n, E)) >::type >::type >::type ET; \
    typedef typename boost::remove_cv< typename boost::remove_reference < \
                                        typename cpp11::result_of< AC(BOOST_PP_ENUM_PARAMS(n, A)) >::type >::type >::type AT; \
    CGAL_BRANCH_PROFILER(std::string(" failures/calls to   : ") + std::string(CGAL_PRETTY_FUNCTION), tmp); \
    Protect_FPU_rounding<Protection> P;                                 \
   try {                                                               \
      return new Lazy_rep_##n<AT, ET, AC, EC, To_interval<ET>, BOOST_PP_ENUM_PARAMS(n, L) >(ac, ec, BOOST_PP_ENUM_PARAMS(n, l)); \
    } catch (Uncertain_conversion_exception) {                          \
      CGAL_BRANCH_PROFILER_BRANCH(tmp);                                 \
      Protect_FPU_rounding<!Protection> P2(CGAL_FE_TONEAREST);          \
      return new Lazy_rep_0<AT,ET,To_interval<ET> >(ec( BOOST_PP_ENUM(n, CGAL_LEXACT, _) )); \
    }                                                                  \
  }                                                                     \

  BOOST_PP_REPEAT_FROM_TO(1, 6, CGAL_NT_OPERATOR, _)

#undef INTERVAL_OPERATOR
#undef CGAL_RESULT_NT
};






// The following functor returns an Object with a Lazy<Something> inside
// As the nested kernels return Objects of AK::Something and EK::Something
// we have to unwrap them from the Object, and wrap them in a Lazy<Something>
//
// TODO: write operators for other than two arguments. For the current kernel we only need two for Intersect_2

template <typename LK, typename AC, typename EC>
struct Lazy_construction_object
{
  static const bool Protection = true;

  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename EK::FT EFT;
  typedef typename LK::E2A E2A;
  typedef typename AC::result_type AT;
  typedef typename EC::result_type ET;
  typedef Object result_type;

  //typedef Lazy<Object, Object, EFT, E2A> Lazy_object;

  AC ac;
  EC ec;

public:



};



//____________________________________________________________
// The magic functor that has Lazy<Something> as result type.
// Two versions are distinguished: one that needs to fiddle
// with result_of and another that can forward the result types.

namespace internal {
BOOST_MPL_HAS_XXX_TRAIT_DEF(result_type)

// lift boost::get into a functor with a result_type member name and
// extend it to operate on optionals

// TODO there is a mismatch between the result_type typedef and the
// actual return type of operator()
template<typename T>
struct Variant_cast {
  typedef T result_type;

  template<BOOST_VARIANT_ENUM_PARAMS(typename U)>
  const T&
  operator()(const boost::optional< boost::variant< BOOST_VARIANT_ENUM_PARAMS(U) > >& o) const {
    // can throw but should never because we always build it inside
    // a static visitor with the right type
    return boost::get<T>(*o);
  }

  template<BOOST_VARIANT_ENUM_PARAMS(typename U)>
  T&
  operator()(boost::optional< boost::variant< BOOST_VARIANT_ENUM_PARAMS(U) > >& o) const {
    // can throw but should never because we always build it inside
    // a static visitor with the right type, if it throws bad_get
    return boost::get<T>(*o);
  }
};


template<typename Result, typename AK, typename LK, typename EK, typename Origin>
struct Fill_lazy_variant_visitor_2 : boost::static_visitor<> {
  Fill_lazy_variant_visitor_2(Result& r, Origin& o) : r(&r), o(&o) {}
  Result* r;
  Origin* o;

  template<typename T>
  void operator()(const T&) {
    // the equivalent type we are currently matching in the lazy kernel
    typedef T AKT;
    typedef typename Type_mapper<AKT, AK, EK>::type EKT;
    typedef typename Type_mapper<AKT, AK, LK>::type LKT;

    typedef Lazy_rep_1<AKT, EKT, Variant_cast<AKT>, Variant_cast<EKT>, typename LK::E2A, Origin> Lcr;
    Lcr * lcr = new Lcr(Variant_cast<AKT>(), Variant_cast<EKT>(), *o);

    *r = LKT(lcr);
  }

};

template<typename Result, typename AK, typename LK, typename EK>
struct Fill_lazy_variant_visitor_0 : boost::static_visitor<> {
  Fill_lazy_variant_visitor_0(Result& r) : r(&r) {}
  Result* r;

  template<typename T>
  void operator()(const T& t) {
    // the equivalent type we are currently matching in the lazy kernel
    typedef T EKT;
    typedef typename Type_mapper<EKT, EK, AK>::type AKT;
    typedef typename Type_mapper<EKT, EK, LK>::type LKT;

    *r = LKT(new Lazy_rep_0<AKT, EKT, typename LK::E2A>(t));
  }

};

} // internal

template <typename LK, typename AC, typename EC>
struct Lazy_construction_variant {
  static const bool Protection = true;

  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename EK::FT EFT;
  typedef typename LK::E2A E2A;


  template<typename>
  struct result {
    // this does not default, if you want to make a lazy lazy-kernel,
    // you are on your own
  };

  #define CGAL_RESULT(z, n, d) \
    template< typename F, BOOST_PP_ENUM_PARAMS(n, class T) >            \
    struct result<F( BOOST_PP_ENUM_PARAMS(n, T) )> {                    \
      BOOST_PP_REPEAT(n, CGAL_TYPEMAP_AC, T)                            \
      typedef typename Type_mapper<                                     \
        typename cpp11::result_of<AC( BOOST_PP_ENUM_PARAMS(n, A) )>::type, AK, LK>::type type; \
    };

  BOOST_PP_REPEAT_FROM_TO(1, 9, CGAL_RESULT, _)

  template <typename L1, typename L2>
  typename result<Lazy_construction_variant(L1, L2)>::type
  operator()(const L1& l1, const L2& l2) const {
    typedef typename cpp11::result_of<Lazy_construction_variant(L1, L2)>::type result_type;

    typedef typename cpp11::result_of<AC(typename Type_mapper<L1, LK, AK>::type,
                                         typename Type_mapper<L2, LK, AK>::type)>::type AT;
    typedef typename cpp11::result_of<EC(typename Type_mapper<L1, LK, EK>::type,
                                         typename Type_mapper<L2, LK, EK>::type)>::type ET;

    CGAL_BRANCH_PROFILER(std::string(" failures/calls to   : ") + std::string(CGAL_PRETTY_FUNCTION), tmp);
    Protect_FPU_rounding<Protection> P;

    try {
      Lazy<AT, ET, EFT, E2A> lazy(new Lazy_rep_2<AT, ET, AC, EC, E2A, L1, L2>(AC(), EC(), l1, l2));

      // the approximate result requires the trait with types from the AK
      AT approx_v = lazy.approx();
      // the result we build
      result_type res;

      if(!approx_v) {
        // empty
        return res;
      }

      // the static visitor fills the result_type with the correct unwrapped type
      internal::Fill_lazy_variant_visitor_2< result_type, AK, LK, EK, Lazy<AT, ET, EFT, E2A> > visitor(res, lazy);
      boost::apply_visitor(visitor, *approx_v);

      return res;
    } catch (Uncertain_conversion_exception) {
      CGAL_BRANCH_PROFILER_BRANCH(tmp);
      Protect_FPU_rounding<!Protection> P2(CGAL_FE_TONEAREST);

      ET exact_v = EC()(CGAL::exact(l1), CGAL::exact(l2));
      result_type res;

      if(!exact_v) {
        return res;
      }

      internal::Fill_lazy_variant_visitor_0<result_type, AK, LK, EK> visitor(res);
      boost::apply_visitor(visitor, *exact_v);
      return res;
    }
  }

  template <typename L1, typename L2, typename L3>
  typename result<Lazy_construction_variant(L1, L2, L3)>::type
  operator()(const L1& l1, const L2& l2, const L3& l3) const {
    typedef typename result<Lazy_construction_variant(L1, L2, L3)>::type result_type;

    typedef typename cpp11::result_of<AC(typename Type_mapper<L1, LK, AK>::type,
                                         typename Type_mapper<L2, LK, AK>::type,
                                         typename Type_mapper<L3, LK, AK>::type)>::type AT;
    typedef typename cpp11::result_of<EC(typename Type_mapper<L1, LK, EK>::type,
                                         typename Type_mapper<L2, LK, EK>::type,
                                         typename Type_mapper<L3, LK, EK>::type)>::type ET;

    CGAL_BRANCH_PROFILER(std::string(" failures/calls to   : ") + std::string(CGAL_PRETTY_FUNCTION), tmp);
    Protect_FPU_rounding<Protection> P;
    try {
      Lazy<AT, ET, EFT, E2A> lazy(new Lazy_rep_3<AT, ET, AC, EC, E2A, L1, L2, L3>(AC(), EC(), l1, l2, l3));

      // the approximate result requires the trait with types from the AK
      AT approx_v = lazy.approx();
      // the result we build
      result_type res;

      if(!approx_v) {
        // empty
        return res;
      }

      // the static visitor fills the result_type with the correct unwrapped type
      internal::Fill_lazy_variant_visitor_2< result_type, AK, LK, EK, Lazy<AT, ET, EFT, E2A> > visitor(res, lazy);
      boost::apply_visitor(visitor, *approx_v);

      return res;
    } catch (Uncertain_conversion_exception) {
      CGAL_BRANCH_PROFILER_BRANCH(tmp);
      Protect_FPU_rounding<!Protection> P2(CGAL_FE_TONEAREST);

      ET exact_v = EC()(CGAL::exact(l1), CGAL::exact(l2), CGAL::exact(l3));
      result_type res;

      if(!exact_v) {
        return res;
      }

      internal::Fill_lazy_variant_visitor_0< result_type, AK, LK, EK> visitor(res);
      boost::apply_visitor(visitor, *exact_v);
      return res;
    }
  }
};

template<typename LK, typename AC, typename EC, typename E2A = Default,
         bool has_result_type = internal::has_result_type<AC>::value && internal::has_result_type<EC>::value >
struct Lazy_construction;


// we have a result type, low effort
template<typename LK, typename AC, typename EC, typename E2A_>
struct Lazy_construction<LK, AC, EC, E2A_, true> {
  static const bool Protection = true;

  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename boost::remove_cv<
    typename boost::remove_reference < typename AC::result_type >::type >::type AT;
  typedef typename boost::remove_cv<
    typename boost::remove_reference < typename EC::result_type >::type >::type  ET;

  typedef typename EK::FT EFT;
  typedef typename Default::Get<E2A_, typename LK::E2A>::type E2A;

  typedef typename Type_mapper<AT, AK, LK>::type result_type;

  AC ac;
  EC ec;

#define CGAL_CONSTRUCTION_OPERATOR(z, n, d  )                           \
  template<BOOST_PP_ENUM_PARAMS(n, class L)>                            \
  result_type                                                           \
  operator()( BOOST_PP_ENUM(n, CGAL_LARGS, _) ) const {                 \
    typedef Lazy< AT, ET, EFT, E2A> Handle;                             \
    CGAL_BRANCH_PROFILER(std::string(" failures/calls to   : ") + std::string(CGAL_PRETTY_FUNCTION), tmp); \
    Protect_FPU_rounding<Protection> P;                                 \
    try {                                                               \
      return result_type( Handle(new Lazy_rep_##n<AT, ET, AC, EC, E2A, BOOST_PP_ENUM_PARAMS(n, L)>(ac, ec, BOOST_PP_ENUM_PARAMS(n, l)))); \
    } catch (Uncertain_conversion_exception) {                          \
      CGAL_BRANCH_PROFILER_BRANCH(tmp);                                 \
      Protect_FPU_rounding<!Protection> P2(CGAL_FE_TONEAREST);          \
      return result_type( Handle(new Lazy_rep_0<AT,ET,E2A>(ec( BOOST_PP_ENUM(n, CGAL_LEXACT, _) ))) ); \
    }                                                                   \
  }

  // arity 1-8
  BOOST_PP_REPEAT_FROM_TO(1, 9, CGAL_CONSTRUCTION_OPERATOR, _)

  // nullary
  result_type
  operator()() const
  {
    typedef Lazy<AT, ET, EFT, E2A> Handle;
    return result_type( Handle(new Lazy_rep_0<AT,ET,E2A>()) );
  }

#undef CGAL_CONSTRUCTION_OPERATOR

};


template <typename LK, typename AC, typename EC, typename E2A_>
struct Lazy_construction<LK, AC, EC, E2A_, false>
{
  static const bool Protection = true;

  typedef typename LK::Approximate_kernel AK;
  typedef typename LK::Exact_kernel EK;
  typedef typename EK::FT EFT;
  typedef typename Default::Get<E2A_, typename LK::E2A>::type E2A;

  template<typename>
  struct result {
    // this does not default, if you want to make a lazy lazy-kernel,
    // you are on your own
  };

  AC ac;
  EC ec;



 BOOST_PP_REPEAT_FROM_TO(1, 9, CGAL_RESULT, _)

};

} //namespace CGAL

#undef CGAL_TYPEMAP_AC
#undef CGAL_TYPEMAP_EC
#undef CGAL_LEXACT
#undef CGAL_LARGS



#endif // LAZY_KERNEL_H
