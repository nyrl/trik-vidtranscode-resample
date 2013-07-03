#ifndef TRIK_LIBIMAGE_STDCPP_HPP_
#define TRIK_LIBIMAGE_STDCPP_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {


class noncopyable
{
  public:
    noncopyable() {}

  private:
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
};


template <bool _expr>
class assert_inst // kind of static_assert for TI compiler
{
  public:
    assert_inst()
    {
      char should_fail_on_false[_expr?1:-1]; // should not compile if _expr is false
      (void)should_fail_on_false; // warn prevention
    }

};


} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_STDCPP_HPP_
