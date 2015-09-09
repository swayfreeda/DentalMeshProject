

#ifndef CGAL_OBJECT_H
#define CGAL_OBJECT_H

#include <igit_geometry/basic_.h>



#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

namespace CGAL {

class Object
{
    boost::shared_ptr<boost::any> obj;

    // returns an any pointer from a variant
    struct Any_from_variant : public boost::static_visitor<boost::any*> {
      template<typename T>
      boost::any* operator()(const T& t) const {
        return new boost::any(t);
      }
    };

    template<class T>
    friend const T* object_cast(const Object * o);

    template<class T>
    friend T object_cast(const Object & o);


  public:

    struct private_tag{};

    Object() : obj() { }

#ifndef CGAL_CFG_NO_CPP0X_RVALUE_REFERENCE
    template <class T>
    Object(T && t, private_tag) : obj(new boost::any(std::forward<T>(t))) { }
#else
    template <class T>
    Object(const T&t, private_tag) : obj(new boost::any(t)) { }
#endif

    // implicit constructor from optionals containing variants
    template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
    Object(const boost::optional< boost::variant<BOOST_VARIANT_ENUM_PARAMS(T) > >& t)
      : obj( t ? boost::apply_visitor(Any_from_variant(), *t) : NULL) { }


    template <class T>
    bool assign(T &t) const
    {
      if(obj) {
        #ifdef CGAL_USE_ANY_BAD_CAST
        try {
          t = boost::any_cast<T>(*obj);
          return true;
        } catch(...) {
          return false;
        }
        #else
        const T* res =  boost::any_cast<T>(&(*obj));
        if (res){
          t=*res;
          return true;
        }
        return false;
        #endif
      } else {
        return false;
      }
    }

    bool
    empty() const
    {
      return !obj;
    }

    // is_empty() is kept for backward compatibility.
    // empty() was introduced for consistency with e.g. std::vector::empty().
    bool
    is_empty() const
    {
    return empty();
    }



};


#ifndef CGAL_CFG_NO_CPP0X_RVALUE_REFERENCE
template <class T>
inline
Object
make_object(T && t)
{
    return Object(std::forward<T>(t), Object::private_tag());
}
#else
template <class T>
inline
Object
make_object(const T& t)
{
    return Object(t, Object::private_tag());
}
#endif

template <class T>
inline
bool
assign(T& t, const Object& o)
{
    return o.assign(t);
}

} //namespace CGAL

#endif // CGAL_OBJECT_H
