#ifndef TRIK_LIBIMAGE_IMAGE_PIXEL_RGB_HPP_
#define TRIK_LIBIMAGE_IMAGE_PIXEL_RGB_HPP_

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


template <size_t _RBits, size_t _GBits, size_t _BBits>
class ImagePixelRGBAccessor : protected BaseImagePixelAccessor,
                              private assert_inst<(_RBits>=1 && _GBits>=1 && _BBits>=1)>
{
  public:
    bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const
    {
      _nr = m_r * rNormDiv();
      _ng = m_g * gNormDiv();
      _nb = m_b * bNormDiv();
      return true;
    }

    bool fromNormalizedRGB(float _nr, float _ng, float _nb)
    {
      m_r = _nr * rNormMult();
      m_g = _ng * gNormMult();
      m_b = _nb * bNormMult();
      return true;
    }

  protected:
    ImagePixelRGBAccessor()
     :m_r(),
      m_g(),
      m_b()
    {
    }

    ImagePixelRGBAccessor(const ImagePixelRGBAccessor& _src)
     :m_r(_src.m_r),
      m_g(_src.m_g),
      m_b(_src.m_b)
    {
    }

    void loadR(ImageColorComponentIntegral _r)
    {
      m_r = _r;
    }

    void loadG(ImageColorComponentIntegral _g)
    {
      m_g = _g;
    }

    void loadB(ImageColorComponentIntegral _b)
    {
      m_b = _b;
    }

    ImageColorComponentIntegral storeR() const
    {
      return /*trunc*/range(rStoreMin(), m_r+0.5f, rStoreMax());
    }

    ImageColorComponentIntegral storeG() const
    {
      return /*trunc*/range(gStoreMin(), m_g+0.5f, gStoreMax());
    }

    ImageColorComponentIntegral storeB() const
    {
      return /*trunc*/range(bStoreMin(), m_b+0.5f, bStoreMax());
    }

    void operatorMultiplyImpl(float _f)
    {
      m_r *= _f;
      m_g *= _f;
      m_b *= _f;
    }

    void operatorIncrementImpl(const ImagePixelRGBAccessor& _p)
    {
      m_r += _p.m_r;
      m_g += _p.m_g;
      m_b += _p.m_b;
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
    float m_r;
    float m_g;
    float m_b;

    static const ImageColorComponentIntegral s_RMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_RBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);
    static const ImageColorComponentIntegral s_GMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_GBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);
    static const ImageColorComponentIntegral s_BMaxIntegral = (static_cast<ImageColorComponentIntegral>(1)<<_BBits)
                                                             - static_cast<ImageColorComponentIntegral>(1);

    static float rStoreMin() { return 0.0f; }
    static float gStoreMin() { return 0.0f; }
    static float bStoreMin() { return 0.0f; }
    static float rStoreMax() { return static_cast<float>(s_RMaxIntegral); }
    static float gStoreMax() { return static_cast<float>(s_GMaxIntegral); }
    static float bStoreMax() { return static_cast<float>(s_BMaxIntegral); }

    static float rNormMult() { return rStoreMax(); }
    static float gNormMult() { return gStoreMax(); }
    static float bNormMult() { return bStoreMax(); }
    static float rNormDiv()  { return 1.0f / rNormMult(); }
    static float gNormDiv()  { return 1.0f / gNormMult(); }
    static float bNormDiv()  { return 1.0f / bNormMult(); }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <>
class ImagePixel<BaseImagePixel::PixelRGB565> : public BaseImagePixel,
                                                public internal::ImagePixelRGBAccessor<5, 6, 5>
{
  public:
    ImagePixel() {}

    template <typename _UByte>
    bool unpack(_UByte _b1, _UByte _b2)
    {
      loadR(  utypeGet<_UByte,  true>(_b1, 5, 3));
      loadG(  utypeGet<_UByte, false>(_b1, 3, 3)
            | utypeGet<_UByte,  true>(_b2, 3, 5));
      loadB(  utypeGet<_UByte,  true>(_b2, 5, 0));
      return true;
    }

    template <typename _UByte>
    bool pack(_UByte& _b1, _UByte& _b2) const
    {
      _b1 = utypeValue<_UByte,  true>(storeR(), 5, 3)
          | utypeValue<_UByte, false>(storeG(), 3, 3);
      _b2 = utypeValue<_UByte,  true>(storeG(), 3, 5)
          | utypeValue<_UByte,  true>(storeB(), 5, 0);
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
class ImagePixel<BaseImagePixel::PixelRGB565X> : public BaseImagePixel,
                                                 public internal::ImagePixelRGBAccessor<5, 6, 5>
{
  public:
    ImagePixel() {}

    template <typename _UByte>
    bool unpack(_UByte _b1, _UByte _b2)
    {
      loadR(  utypeGet<_UByte,  true>(_b1, 5, 0));
      loadG(  utypeGet<_UByte,  true>(_b1, 3, 5)
            | utypeGet<_UByte, false>(_b2, 3, 3));
      loadB(  utypeGet<_UByte,  true>(_b2, 5, 3));
      return true;
    }

    template <typename _UByte>
    bool pack(_UByte& _b1, _UByte& _b2) const
    {
      _b1 = utypeValue<_UByte,  true>(storeR(), 5, 0)
          | utypeValue<_UByte,  true>(storeG(), 3, 5);
      _b2 = utypeValue<_UByte, false>(storeG(), 3, 3)
          | utypeValue<_UByte,  true>(storeB(), 5, 3);
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
class ImagePixel<BaseImagePixel::PixelRGB888> : public BaseImagePixel,
                                                public internal::ImagePixelRGBAccessor<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename _UByte>
    bool unpack(_UByte _b1, _UByte _b2, _UByte _b3)
    {
      loadR(utypeGet<_UByte, true>(_b1, 8, 0));
      loadG(utypeGet<_UByte, true>(_b2, 8, 0));
      loadB(utypeGet<_UByte, true>(_b3, 8, 0));

      return true;
    }

    template <typename _UByte>
    bool pack(_UByte& _b1, _UByte& _b2, _UByte& _b3) const
    {
      _b1 = utypeValue<_UByte, true>(storeR(), 8, 0);
      _b2 = utypeValue<_UByte, true>(storeG(), 8, 0);
      _b3 = utypeValue<_UByte, true>(storeB(), 8, 0);

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


#endif // !TRIK_LIBIMAGE_IMAGE_PIXEL_RGB_HPP_
