

#ifndef CGAL_RESULT_OF_H
#define CGAL_RESULT_OF_H

#include <boost/utility/result_of.hpp>


namespace CGAL{

namespace cpp11{

template<typename F>
struct result_of
{
  #if BOOST_VERSION < 104400
  typedef typename boost::result_of<F>::type type;
  #else
  typedef typename boost::tr1_result_of<F>::type type;
  #endif
};

}

}

#endif //CGAL_RESULT_OF_H
