#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


namespace trik_image
{

namespace pixel_rgb
{


template <size_t _RBits, size_t _GBits, size_t _BBits>
class RGBStorage : BaseImagePixelStorage
{
  public:
    RGBStorage()
     :m_r(0.0),
      m_g(0.0),
      m_b(0.0)
    {
    }

    RGBStorage(const RGBStorage& _src)
     :m_r(_src.m_r),
      m_g(_src.m_g),
      m_b(_src.m_b)
    {
    }

    RGBStorage& operator=(const RGBStorage& _src)
    {
      m_r = _src.m_r;
      m_g = _src.m_g;
      m_b = _src.m_b;

      return *this;
    }

    float&       r()       { return m_r; }
    const float& r() const { return m_r; }

    float&       g()       { return m_g; }
    const float& g() const { return m_g; }

    float&       b()       { return m_b; }
    const float& b() const { return m_b; }

  protected:
    bool toNormalizedImpl(float& _nr, float& _ng, float& _nb) const
    {
      _nr = m_r / rMax();
      _ng = m_g / gMax();
      _nb = m_b / bMax();

      assert(   _nr >= 0.0 && _nr <= 1.0
             && _ng >= 0.0 && _ng <= 1.0
             && _nb >= 0.0 && _nb <= 1.0);

      return true;
    }

    bool fromNormalizedImpl(const float& _nr, const float& _ng, const float& _nb)
    {
      m_r = _nr * rMax();
      m_g = _ng * gMax();
      m_b = _nb * bMax();

      assert(   m_r >= 0.0 && m_r <= rMax()
             && m_g >= 0.0 && m_g <= gMax()
             && m_b >= 0.0 && m_b <= bMax());

      return true;
    }

  private:
    float m_r;
    float m_g;
    float m_b;

    static float rMax() { return 1u<<_RBits; }
    static float gMax() { return 1u<<_GBits; }
    static float bMax() { return 1u<<_BBits; }
};


} // namespace pixel_rgb




template <>
class ImagePixel<BaseImagePixel::PixelRGB565> : public BaseImagePixel,
                                                public pixel_rgb::RGBStorage<5, 6, 5>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2)
    {
      r() = storageGet<UByte,  true>(_b1, 5, 3);
      g() = storageGet<UByte, false>(_b1, 3, 3)
          | storageGet<UByte,  true>(_b2, 3, 5);
      b() = storageGet<UByte,  true>(_b2, 5, 0);

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2) const
    {
      _b1 = storageValue<UByte,  true>(r(), 5, 3)
          | storageValue<UByte, false>(g(), 3, 3);
      _b2 = storageValue<UByte,  true>(g(), 3, 5)
          | storageValue<UByte,  true>(b(), 5, 0);

      return true;
    }

  protected:
    virtual bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const
    {
      return toNormalizedImpl(_nr, _ng, _nb);
    }

    virtual bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb)
    {
      return fromNormalizedImpl(_nr, _ng, _nb);
    }
};




template <>
class ImagePixel<BaseImagePixel::PixelRGB888> : public BaseImagePixel,
                                                public pixel_rgb::RGBStorage<8, 8, 8>
{
  public:
    ImagePixel() {}

    template <typename UByte>
    bool unpack(const UByte& _b1, const UByte& _b2, const UByte& _b3)
    {
      r() = storageGet<UByte>(_b1, 8, 0);
      g() = storageGet<UByte>(_b2, 8, 0);
      b() = storageGet<UByte>(_b3, 8, 0);

      return true;
    }

    template <typename UByte>
    bool pack(UByte& _b1, UByte& _b2, UByte& _b3) const
    {
      _b1 = storageValue<UByte>(r(), 8, 0);
      _b2 = storageValue<UByte>(g(), 8, 0);
      _b3 = storageValue<UByte>(b(), 8, 0);

      return true;
    }

  protected:
    virtual bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const
    {
      return toNormalizedImpl(_nr, _ng, _nb);
    }

    virtual bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb)
    {
      return fromNormalizedImpl(_nr, _ng, _nb);
    }

};


} // namespace trik_image


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_RGB_HPP_
