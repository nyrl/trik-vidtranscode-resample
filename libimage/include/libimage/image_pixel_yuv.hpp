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
      const float y = m_y * yNormDiv();
      const float u = m_u * uNormDiv() - 0.5f;
      const float v = m_v * vNormDiv() - 0.5f;

      _nr = y              +  1.4075f*v;
      _ng = y + -0.3455f*u + -0.7169f*v;
      _nb = y +  1.7790f*u;

      return true;
    }

    bool fromNormalizedRGB(float _nr, float _ng, float _nb)
    {
      m_y =  ( 0.2990f*_nr +  0.5870f*_ng +  0.1140f*_nb)         * yNormMult();
      m_u = ((-0.1687f*_nr + -0.3312f*_ng +  0.5000f*_nb) + 0.5f) * uNormMult();
      m_v = (( 0.5000f*_nr + -0.4186f*_ng + -0.0813f*_nb) + 0.5f) * vNormMult();

      return true;
    }

  protected:
    ImagePixelYUVAccessor()
     :m_y(),
      m_u(),
      m_v()
    {
    }

    ImagePixelYUVAccessor(const ImagePixelYUVAccessor& _src)
     :m_y(_src.m_y),
      m_u(_src.m_u),
      m_v(_src.m_v)
    {
    }

    void loadY(ImageColorComponentIntegral _y)
    {
      m_y = _y;
    }

    void loadU(ImageColorComponentIntegral _u)
    {
      m_u = _u;
    }

    void loadV(ImageColorComponentIntegral _v)
    {
      m_v = _v;
    }

    ImageColorComponentIntegral storeY() const
    {
      return /*trunc*/range(yStoreMin(), m_y+0.5f, yStoreMax());
    }

    ImageColorComponentIntegral storeU() const
    {
      return /*trunc*/range(uStoreMin(), m_u+0.5f, uStoreMax());
    }

    ImageColorComponentIntegral storeV() const
    {
      return /*trunc*/range(vStoreMin(), m_v+0.5f, vStoreMax());
    }

    void operatorMultiplyImpl(float _f)
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

    static const ImageColorComponentIntegral s_YMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_YBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);
    static const ImageColorComponentIntegral s_UMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_UBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);
    static const ImageColorComponentIntegral s_VMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_VBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);

    static float yStoreMin() { return 0.0f; }
    static float uStoreMin() { return 0.0f; }
    static float vStoreMin() { return 0.0f; }
    static float yStoreMax() { return static_cast<float>(s_YMaxIntegral); }
    static float uStoreMax() { return static_cast<float>(s_UMaxIntegral); }
    static float vStoreMax() { return static_cast<float>(s_VMaxIntegral); }

    static float yNormMult() { return yStoreMax(); }
    static float uNormMult() { return uStoreMax(); }
    static float vNormMult() { return vStoreMax(); }
    static float yNormDiv()  { return 1.0f / yNormMult(); }
    static float uNormDiv()  { return 1.0f / uNormMult(); }
    static float vNormDiv()  { return 1.0f / vNormMult(); }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <>
class ImagePixel<BaseImagePixel::PixelYUV444> : public BaseImagePixel,
                                                public internal::ImagePixelYUVAccessor<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename _UByte>
    bool unpack(_UByte _b1, _UByte _b2, _UByte _b3, _UByte _b4)
    {
      (void)_b1;
      loadY(utypeGet<_UByte, true>(_b2, 8, 0));
      loadU(utypeGet<_UByte, true>(_b3, 8, 0));
      loadV(utypeGet<_UByte, true>(_b4, 8, 0));

      return true;
    }

    template <typename _UByte>
    bool pack(_UByte& _b1, _UByte& _b2, _UByte& _b3, _UByte& _b4) const
    {
      _b1 = 0;
      _b2 = utypeValue<_UByte, true>(storeY(), 8, 0);
      _b3 = utypeValue<_UByte, true>(storeU(), 8, 0);
      _b4 = utypeValue<_UByte, true>(storeV(), 8, 0);

      return true;
    }

    ImagePixel operator*(float _f) const
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

    template <typename _UByte>
    bool unpack(_UByte _b1, _UByte _b2, _UByte _b3)
    {
      loadY(utypeGet<_UByte, true>(_b1, 8, 0));
      loadU(utypeGet<_UByte, true>(_b2, 8, 0));
      loadV(utypeGet<_UByte, true>(_b3, 8, 0));

      return true;
    }

    template <typename _UByte>
    bool pack(_UByte& _b1, _UByte& _b2, _UByte& _b3, bool _inc) const
    {
      _b1 = utypeValue<_UByte, true>(storeY(), 8, 0);
      if (_inc)
      {
        _b2 += utypeValue<_UByte, true>(storeU(), 8, 0) / 2;
        _b3 += utypeValue<_UByte, true>(storeV(), 8, 0) / 2;
      }
      else
      {
        _b2 = utypeValue<_UByte, true>(storeU(), 8, 0) / 2;
        _b3 = utypeValue<_UByte, true>(storeV(), 8, 0) / 2;
      }

      return true;
    }

    ImagePixel operator*(float _f) const
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
