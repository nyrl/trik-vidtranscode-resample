#ifndef TRIK_LIBIMAGE_IMAGE_PIXEL_YUV_HPP_
#define TRIK_LIBIMAGE_IMAGE_PIXEL_YUV_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <stdio.h>
#include <cmath>
#include <iostream>

#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


template <size_t _YBits, size_t _UBits, size_t _VBits>
class ImagePixelYUVAccessor : protected BaseImagePixelAccessor,
                              private assert_inst<(_YBits>=1 && _UBits>=1 && _VBits>=1)>
{
  public:
    bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const
    {
      const float y = m_y/yMax();
      const float u = m_u/uMax() - 0.5f;
      const float v = m_v/uMax() - 0.5f;

      _nr = range(0.0f, ( y +  0.0f      +  1.4075f*v), 1.0f);
      _ng = range(0.0f, ( y + -0.3455f*u + -0.7169f*v), 1.0f);
      _nb = range(0.0f, ( y +  1.7790f*u +  0.0f     ), 1.0f);
      return true;
    }

    bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb)
    {
      m_y =  ( 0.2990f*_nr +  0.5870f*_ng +  0.1140f*_nb) * yMax();
      m_u = ((-0.1687f*_nr + -0.3312f*_ng +  0.5000f*_nb) + 0.5f) * uMax();
      m_v = (( 0.5000f*_nr + -0.4186f*_ng + -0.0813f*_nb) + 0.5f) * vMax();
      return true;
    }

  protected:
    ImagePixelYUVAccessor()
     :m_y(0.0f),
      m_u(0.0f),
      m_v(0.0f)
    {
    }

    ImagePixelYUVAccessor(const ImagePixelYUVAccessor& _src)
     :m_y(_src.m_y),
      m_u(_src.m_u),
      m_v(_src.m_v)
    {
    }

    void loadY(unsigned _y)
    {
      m_y = _y;
    }

    void loadU(unsigned _u)
    {
      m_u = _u;
    }

    void loadV(unsigned _v)
    {
      m_v = _v;
    }

    unsigned storeY() const
    {
      return roundf(range(0.0f, m_y, yMax()));
    }

    unsigned storeU() const
    {
      return roundf(range(0.0f, m_u, uMax()));
    }

    unsigned storeV() const
    {
      return roundf(range(0.0f, m_v, vMax()));
    }

    void operatorMultiplyImpl(const float& _f)
    {
      m_y *= _f;
      m_u *= _f;
      m_v *= _f;
    }

    void operatorIncrementImpl(const ImagePixelYUVAccessor& _p)
    {
      m_y += _p.m_y;
      m_u += _p.m_u;
      m_v += _p.m_v;
    }

    void operatorExtractImpl(std::ostream& _os) const
    {
      float nr;
      float ng;
      float nb;

      toNormalizedRGB(nr, ng, nb);
      _os << '(' << nr << ' ' << ng << ' ' << nb << ')';
    }

  private:
    float m_y;
    float m_u;
    float m_v;

    static float yMax() { return (1u<<_YBits) - 1; }
    static float uMax() { return (1u<<_UBits) - 1; }
    static float vMax() { return (1u<<_VBits) - 1; }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <>
class ImagePixel<BaseImagePixel::PixelYUV444> : public BaseImagePixel,
                                                public internal::ImagePixelYUVAccessor<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2, const UByte& _b3, const UByte& _b4)
    {
      (void)_b1;
      loadY(utypeGet<UByte, true>(_b2, 8, 0));
      loadU(utypeGet<UByte, true>(_b3, 8, 0));
      loadV(utypeGet<UByte, true>(_b4, 8, 0));

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2, UByte& _b3, UByte& _b4) const
    {
      _b1 = 0;
      _b2 = utypeValue<UByte, true>(storeY(), 8, 0);
      _b3 = utypeValue<UByte, true>(storeU(), 8, 0);
      _b4 = utypeValue<UByte, true>(storeV(), 8, 0);

      return true;
    }

    ImagePixel operator*(const float& _f) const
    {
      ImagePixel p(*this);
      p.operatorMultiplyImpl(_f);
      return p;
    }

    ImagePixel& operator+=(const ImagePixel& _p)
    {
      operatorIncrementImpl(_p);
      return *this;
    }

  private:
    friend std::ostream& operator<<(std::ostream& _os, const ImagePixel& _p)
    {
      _p.operatorExtractImpl(_os);
      return _os;
    }
};




template <>
class ImagePixel<BaseImagePixel::PixelYUV422> : public BaseImagePixel,
                                                public internal::ImagePixelYUVAccessor<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2, const UByte& _b3)
    {
      loadY(utypeGet<UByte, true>(_b1, 8, 0));
      loadU(utypeGet<UByte, true>(_b2, 8, 0));
      loadV(utypeGet<UByte, true>(_b3, 8, 0));

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2, UByte& _b3, bool _inc) const
    {
      _b1 = utypeValue<UByte, true>(storeY(), 8, 0);
      if (_inc)
      {
        _b2 += utypeValue<UByte, true>(storeU(), 8, 0) / 2;
        _b3 += utypeValue<UByte, true>(storeV(), 8, 0) / 2;
      }
      else
      {
        _b2 = utypeValue<UByte, true>(storeU(), 8, 0) / 2;
        _b3 = utypeValue<UByte, true>(storeV(), 8, 0) / 2;
      }

      return true;
    }

    ImagePixel operator*(const float& _f) const
    {
      ImagePixel p(*this);
      p.operatorMultiplyImpl(_f);
      return p;
    }

    ImagePixel& operator+=(const ImagePixel& _p)
    {
      operatorIncrementImpl(_p);
      return *this;
    }

  private:
    friend std::ostream& operator<<(std::ostream& _os, const ImagePixel& _p)
    {
      _p.operatorExtractImpl(_os);
      return _os;
    }
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_PIXEL_YUV_HPP_
