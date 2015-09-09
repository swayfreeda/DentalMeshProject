
#ifndef CGAL_HANDLE_H
#define CGAL_HANDLE_H

#include <igit_geometry/Handle_for_.h>


namespace CGAL {

class Rep
{
    friend class Handle;
  protected:
             Rep() { count = 1; }
    virtual ~Rep() {}

    int      count;
};

class Handle
{
  public:

    typedef std::ptrdiff_t Id_type ;

    Handle()
    : PTR(static_cast<Rep*>(0)) {}

    Handle(const Handle& x)
    {

      PTR = x.PTR;
      PTR->count++;
    }

    ~Handle()
    {
    if ( PTR && (--PTR->count == 0))
        delete PTR;
    }

    Handle&
    operator=(const Handle& x)
    {
      x.PTR->count++;
      if ( PTR && (--PTR->count == 0))
      delete PTR;
      PTR = x.PTR;
      return *this;
    }

    int
    refs()  const { return PTR->count; }

    Id_type id() const { return PTR - static_cast<Rep*>(0); }

    bool identical(const Handle& h) const { return PTR == h.PTR; }

  protected:
   Rep* PTR;
};



} //namespace CGAL

#endif // CGAL_HANDLE_H
