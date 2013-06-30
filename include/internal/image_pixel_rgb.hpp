#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <stdio.h>
#include <cmath>

#include "include/internal/stdcpp.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


template <size_t _RBits, size_t _GBits, size_t _BBits>
class ImagePixelRGBAccessor : protected BaseImagePixelAccessor
{
  public:
    bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const
    {
      _nr = range(0.0f, m_r / rMax(), 1.0f);
      _ng = range(0.0f, m_g / gMax(), 1.0f);
      _nb = range(0.0f, m_b / bMax(), 1.0f);
      return true;
    }

    bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb)
    {
      m_r = _nr * rMax();
      m_g = _ng * gMax();
      m_b = _nb * bMax();
      return true;
    }


  protected:
    ImagePixelRGBAccessor()
     :m_r(0.0),
      m_g(0.0),
      m_b(0.0)
    {
    }

    ImagePixelRGBAccessor(const ImagePixelRGBAccessor& _src)
     :m_r(_src.m_r),
      m_g(_src.m_g),
      m_b(_src.m_b)
    {
    }


    ImagePixelRGBAccessor& operator*=(const float& _f)
    {
      m_r *= _f;
      m_g *= _f;
      m_b *= _f;
      return *this;
    }

    ImagePixelRGBAccessor& operator+=(const ImagePixelRGBAccessor& _add)
    {
      m_r += _add.m_r;
      m_g += _add.m_g;
      m_b += _add.m_b;
      return *this;
    }

    void loadR(unsigned _r)
    {
      m_r = _r;
    }

    void loadG(unsigned _g)
    {
      m_g = _g;
    }

    void loadB(unsigned _b)
    {
      m_b = _b;
    }

    unsigned storeR() const
    {
      return roundf(range(0.0f, m_r, rMax()));
    }

    unsigned storeG() const
    {
      return roundf(range(0.0f, m_g, gMax()));
    }

    unsigned storeB() const
    {
      return roundf(range(0.0f, m_b, bMax()));
    }

  private:
    float m_r;
    float m_g;
    float m_b;

    static float rMax() { return 1u<<_RBits; }
    static float gMax() { return 1u<<_GBits; }
    static float bMax() { return 1u<<_BBits; }
    static float range(float _min, float _val, float _max) { return std::min(_max, std::max(_min, _val)); }

    template <typename Pixel>
    friend Pixel operator*(const Pixel&, const float&);
    template <typename Pixel>
    friend Pixel& operator+=(Pixel&, const Pixel&);
};


/* it is in internal, but might work due to argument dependent lookup */
template <typename Pixel>
Pixel operator*(const Pixel& _op1, const float& _op2)
{
  Pixel res(_op1);
  res.operator*=(_op2);
  return res;
}

template <typename Pixel>
Pixel& operator+=(Pixel& _op1, const Pixel& _op2)
{
  _op1.operator+=(_op2);
  return _op1;
}


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <>
class ImagePixel<BaseImagePixel::PixelRGB565> : public BaseImagePixel,
                                                public internal::ImagePixelRGBAccessor<5, 6, 5>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2)
    {
      loadR(  storageGet<UByte,  true>(_b1, 5, 3));
      loadG(  storageGet<UByte, false>(_b1, 3, 3)
            | storageGet<UByte,  true>(_b2, 3, 5));
      loadB(  storageGet<UByte,  true>(_b2, 5, 0));

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2) const
    {
      _b1 = storageValue<UByte,  true>(storeR(), 5, 3)
          | storageValue<UByte, false>(storeG(), 3, 3);
      _b2 = storageValue<UByte,  true>(storeG(), 3, 5)
          | storageValue<UByte,  true>(storeB(), 5, 0);

      return true;
    }
};




template <>
class ImagePixel<BaseImagePixel::PixelRGB888> : public BaseImagePixel,
                                                public internal::ImagePixelRGBAccessor<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2, const UByte& _b3)
    {
      loadR(storageGet<UByte, true>(_b1, 8, 0));
      loadG(storageGet<UByte, true>(_b2, 8, 0));
      loadB(storageGet<UByte, true>(_b3, 8, 0));

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2, UByte& _b3) const
    {
      _b1 = storageValue<UByte, true>(storeR(), 8, 0);
      _b2 = storageValue<UByte, true>(storeG(), 8, 0);
      _b3 = storageValue<UByte, true>(storeB(), 8, 0);

      return true;
    }
};


} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_
