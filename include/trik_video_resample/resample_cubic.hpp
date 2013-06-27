#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_CUBIC_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_CUBIC_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <stdlib.h>
#include <cassert>


/*
 * Result is:
 *                             [  0,  2,  0,  0 ]
 * 0,5 * [1, _t, _t^2, _t^3] * [ -1,  0,  1,  0 ]
 *                             [  2, -5,  4, -1 ]
 *                             [ -1,  3, -3,  1 ]
 */
static inline
bool
trikCubicInterpolationWeight(const float	_t,
                             float		_result[restrict 4])
{
  assert(_t && _result);

  const float t0 = 1;
  const float t1 = _t;
  const float t2 = _t*_t;
  const float t3 = _t*_t*_t;

  _result[0] = 0.5 * ( 0*t0 + -1*t1 +  2*t2 + -1*t3 );
  _result[1] = 0.5 * ( 2*t0 +  0*t1 + -5*t2 +  3*t3 );
  _result[2] = 0.5 * ( 0*t0 +  1*t1 +  4*t2 + -3*t3 );
  _result[3] = 0.5 * ( 0*t0 +  0*t1 + -1*t2 +  1*t3 );
  return true;
}


/*
 * Result is N times by:
 *                      [ vn0 ]
 * [ w0, w1, w2, w3 ] * [ vn1 ]
 *   pre-calculated     [ vn2 ]
 *                      [ vn3 ]
 */
template <size_t N>
bool
trikCubicInterpolationValue(const float _w[restrict 4],
                            const float _v[restrict N*4],
                            float       _result[restrict N])
{
  assert(_w && _v && _result);

  const float w0 = _w[0];
  const float w1 = _w[1];
  const float w2 = _w[2];
  const float w3 = _w[3];

  for (size_t n = 0; n < N; ++n)
  {
    _result[n] = _v[n*4+0]*w0
               + _v[n*4+1]*w1
               + _v[n*4+2]*w2
               + _v[n*4+3]*w3;
  }

  return true;
}


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_CUBIC_HPP_
