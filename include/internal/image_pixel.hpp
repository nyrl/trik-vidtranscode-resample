#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <climits>

#include "include/internal/stdcpp.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImagePixelStorage
{
  protected:
    BaseImagePixelStorage() {}

    template <typename UType>
    static UType storageBitmask(size_t _size)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      return (static_cast<UType>(1u) << _size) - 1;
    }

    template <typename UType, bool _ofsPositive>
    static UType storageGet(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value >> _ofs) & storageBitmask<UType>(_size);
      else
        return (_value & storageBitmask<UType>(_size)) << _ofs;
    }

    template <typename UType, bool _ofsPositive>
    static UType storageValue(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value & storageBitmask<UType>(_size)) << _ofs;
      else
        return (_value >> _ofs) & storageBitmask<UType>(_size);
    }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImagePixel
{
  public:
    enum PixelType
    {
      PixelRGB565,
      PixelRGB888
    };

    virtual ~BaseImagePixel() {}

    virtual bool convertTo(BaseImagePixel& _dst) const
    {
      float nr;
      float nb;
      float ng;

      return toNormalizedRGB(nr, nb, ng)
          && _dst.fromNormalizedRGB(nr, nb, ng);
    }

  protected:
    BaseImagePixel() {}

    virtual bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const = 0;
    virtual bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb) = 0;
};


template <BaseImagePixel::PixelType PT>
class ImagePixel : public BaseImagePixel,
                   private internal::BaseImagePixelStorage,
                   private assert_inst<false> // Generic instance, non-functional
{
};




class BaseImagePixelSet
{
  protected:
    BaseImagePixelSet() {}
};


template <typename BaseImagePixel::PixelType PT, size_t _pixelsCount>
class ImagePixelSet : public BaseImagePixelSet,
                      private assert_inst<(_pixelsCount > 0)> // sanity check
{
  public:
    typedef ImagePixel<PT> Pixel;

    ImagePixelSet()
     :BaseImagePixelSet(),
      m_pixels(),
      m_pixelFirst(0)
    {
    }

    size_t pixelsCount() const
    {
      return _pixelsCount;
    }

    Pixel& insertNewPixel()
    {
      const size_t currentPixel = m_pixelFirst;
      m_pixelFirst = pixelIndex(1);
      return m_pixels[currentPixel];
    }

    bool insertLastPixelCopy()
    {
      insertNewPixel() = operator[](pixelsCount()-1);
      return true;
    }

    Pixel& operator[](size_t _index)
    {
      return m_pixels[pixelIndex(_index)];
    }

    const Pixel& operator[](size_t _index) const
    {
      return m_pixels[pixelIndex(_index)];
    }

  protected:
    size_t pixelIndex(size_t _index) const
    {
      return (m_pixelFirst + _index) % _pixelsCount;
    }

  private:
    Pixel  m_pixels[_pixelsCount];
    size_t m_pixelFirst;
};




} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include "include/internal/image_pixel_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
