#ifndef TRIK_LIBIMAGE_IMAGE_PIXEL_HPP_
#define TRIK_LIBIMAGE_IMAGE_PIXEL_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <climits>
#include <iostream>

#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImagePixelAccessor
{
  protected:
    BaseImagePixelAccessor() {}

    template <typename _UType>
    static _UType utypeBitmask(size_t _size)
    {
      assert(_size <= sizeof(_UType) * CHAR_BIT);
      return (static_cast<_UType>(1u) << _size) - 1;
    }

    template <typename _UType, bool _ofsPositive>
    static _UType utypeGet(const _UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(_UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(_UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value >> _ofs) & utypeBitmask<_UType>(_size);
      else
        return (_value & utypeBitmask<_UType>(_size)) << _ofs;
    }

    template <typename _UType, bool _ofsPositive>
    static _UType utypeValue(const _UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(_UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(_UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value & utypeBitmask<_UType>(_size)) << _ofs;
      else
        return (_value >> _ofs) & utypeBitmask<_UType>(_size);
    }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImagePixel
{
  public:
    enum PixelType
    {
      PixelRGB565,
      PixelRGB565X,
      PixelRGB888,
      PixelYUV444,
      PixelYUV422
    };

  protected:
    BaseImagePixel() {}
};


template <BaseImagePixel::PixelType _PT>
class ImagePixel : public BaseImagePixel,
                   private internal::BaseImagePixelAccessor,
                   private assert_inst<false> // Generic instance, non-functional
{
};




class BaseImagePixelSet
{
  protected:
    BaseImagePixelSet() {}
};




template <BaseImagePixel::PixelType _PT, size_t _pixelsCount>
class ImagePixelSet : public BaseImagePixelSet,
                      private assert_inst<(_pixelsCount > 0)> // sanity check
{
  public:
    typedef ImagePixel<_PT> Pixel;

    ImagePixelSet()
     :BaseImagePixelSet(),
      m_pixels(),
      m_pixelFirst(0)
    {
    }

    void reset()
    {
      m_pixelFirst = 0;
      for (size_t pix = 0; pix < pixelsCount(); ++pix)
        m_pixels[pix] = Pixel();
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
      const Pixel cp = operator[](pixelsCount()-1);
      // must copy a pixel to be safe
      insertNewPixel() = cp;
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

    friend std::ostream& operator<<(std::ostream& _os, const ImagePixelSet& _s)
    {
      _os << "[" << _s.pixelsCount() << ":";
      for (size_t i = 0; i < _s.pixelsCount(); ++i)
        _os << " " << _s[i];
      _os << "]";
      return _os;
    }
};




template <typename _PixelType1, typename _PixelType2>
class ImagePixelConvertion
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const _PixelType1& _p1, _PixelType2& _p2) const
    {
      float nr;
      float ng;
      float nb;

      return _p1.toNormalizedRGB(nr, ng, nb)
          && _p2.fromNormalizedRGB(nr, ng, nb);
    }
};

template <typename _PixelType>
class ImagePixelConvertion<_PixelType, _PixelType> // specialization for same type copy
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const _PixelType& _p1, _PixelType& _p2) const
    {
      _p2 = _p1;
      return true;
    }
};


template <typename _PixelSetType1, typename _PixelSetType2>
class ImagePixelSetConvertion
{
  public:
    ImagePixelSetConvertion()
     :m_convertion()
    {
    }

    bool operator()(const _PixelSetType1& _s1, _PixelSetType2& _s2) const
    {
      bool isOk = true;

      for (size_t i = 0; i < _s1.pixelsCount(); ++i)
        isOk &= m_convertion(_s1[i], _s2[i]);

      return isOk;
    }

  private:
    ImagePixelConvertion<typename _PixelSetType1::Pixel, typename _PixelSetType2::Pixel> m_convertion;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_pixel_rgb.hpp>
#include <libimage/image_pixel_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_PIXEL_HPP_
