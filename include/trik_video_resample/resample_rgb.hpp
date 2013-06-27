#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>
#include <cmath>


enum Trik_RGBType
{
  Trik_RGB565,
  Trik_RGB888
};


template <Trik_RGBType _Kind, typename UByteType>
class TrikRGBAccessor
{
  public:

    static
    const UByteType* read(const UByteType* restrict _from,
                          float& _r,
                          float& _g,
                          float& _b);

    static
    UByteType* write(UByteType* restrict _to,
                     const float& _r,
                     const float& _g,
                     const float& _b);
};




template <typename UByteType>
class TrikRGBAccessor<Trik_RGB565, UByteType>
{
  public:
    static
    const UByteType* read(const UByteType* restrict _from,
                          float& _r,
                          float& _g,
                          float& _b)
    {
      assert(_from);

      const UByteType f1 = *(_from++);
      const UByteType f2 = *(_from++);

      const unsigned fr = ((f1>>3) & 0x1f);
      const unsigned fg = ((f1&0x07) << 3) | ((f2>>5) & 0x07);
      const unsigned fb = f2 & 0x1f;

      // normalize to 0..1
      _r = static_cast<float>(fr)/(1u<<5);
      _g = static_cast<float>(fg)/(1u<<6);
      _b = static_cast<float>(fb)/(1u<<5);

      return _from;
    }

    static
    UByteType* write(UByteType* restrict _to,
                     const float& _r,
                     const float& _g,
                     const float& _b)
    {
      assert(_to);

      const unsigned ur = roundf(_r*(1u<<5));
      const unsigned ug = roundf(_g*(1u<<6));
      const unsigned ub = roundf(_b*(1u<<5));

      const UByteType t1 = ((ur<<3) & 0xf8) | ((ug>>3) & 0x07);
      const UByteType t2 = ((ug<<5) & 0xe0) | ((ub   ) & 0x1f);

      *(_to++) = t1;
      *(_to++) = t2;

      return _to;
    }
};




template <typename UByteType>
class TrikRGBAccessor<Trik_RGB888, UByteType>
{
  public:
    static
    const UByteType* read(const UByteType* restrict _from,
                          float& _r,
                          float& _g,
                          float& _b)
    {
      assert(_from);

      const unsigned fr = *(_from++);
      const unsigned fg = *(_from++);
      const unsigned fb = *(_from++);

      // normalize to 0..1
      _r = static_cast<float>(fr)/(1u<<8);
      _g = static_cast<float>(fg)/(1u<<8);
      _b = static_cast<float>(fb)/(1u<<8);

      return _from;
    }

    static
   UByteType* write(UByteType* restrict _to,
                    const float& _r,
                    const float& _g,
                    const float& _b)
    {
      assert(_to);

      const unsigned ur = roundf(_r*(1u<<8));
      const unsigned ug = roundf(_g*(1u<<8));
      const unsigned ub = roundf(_b*(1u<<8));

      *(_to++) = ur;
      *(_to++) = ug;
      *(_to++) = ub;

      return _to;
    }
};


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_RGB_HPP_
