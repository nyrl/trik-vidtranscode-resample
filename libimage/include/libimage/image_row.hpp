#ifndef TRIK_LIBIMAGE_IMAGE_ROW_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROW_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>
#include <libimage/image_pixel.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageRowAccessor
{
  protected:
    BaseImageRowAccessor() {}
};


template <typename _UByteCV, bool _extraChecks>
class ImageRowAccessor : private BaseImageRowAccessor,
                         private assert_inst<false> // Generic instance, non-functional
{
};


template <typename _UByteCV>
class ImageRowAccessor<_UByteCV, true> : private BaseImageRowAccessor
{
  public:
    bool resetRow(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
    {
      m_ptr = _rowPtr;
      m_remainLineLength = _lineLength;
      m_remainWidth = _width;
      return true;
    }

  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr(),
      m_remainLineLength(),
      m_remainWidth()
    {
    }

    bool accessPixel(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels=1)
    {
      if (   m_ptr == NULL
          || m_remainWidth      < _pixels
          || m_remainLineLength < _bytes)
        return false;

      _pixelPtr = m_ptr;

      m_ptr              += _bytes;
      m_remainLineLength -= _bytes;
      m_remainWidth      -= _pixels;

      return true;
    }

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels) const
    {
      if (   m_ptr == NULL
          || m_remainWidth      < _pixels
          || m_remainLineLength < _bytes)
        return false;

      _pixelPtr = m_ptr;

      return true;
    }

  private:
    _UByteCV*  m_ptr;
    ImageSize  m_remainLineLength;
    ImageDim   m_remainWidth;
};


template <typename _UByteCV>
class ImageRowAccessor<_UByteCV, false> : private BaseImageRowAccessor
{
  public:
    bool resetRow(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
    {
      m_ptr = _rowPtr;
      (void)_lineLength;
      (void)_width;
      return true;
    }

  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr()
    {
    }

    bool accessPixel(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels=1)
    {
      assert(m_ptr != NULL);

      _pixelPtr = m_ptr;
      m_ptr += _bytes;

      return true;
    }

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels) const
    {
      assert(m_ptr != NULL);

      _pixelPtr = m_ptr;

      return true;
    }

  private:
    _UByteCV*  m_ptr;
};




} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImageRow
{
  public:
    BaseImageRow() {}
};


template <BaseImagePixel::PixelType _PT, typename _UByteCV, bool _extraChecks>
class ImageRow : public BaseImageRow,
                 private internal::ImageRowAccessor<_UByteCV, _extraChecks>,
                 private assert_inst<false> // Generic instance, non-functional
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_row_rgb.hpp>
#include <libimage/image_row_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_HPP_
