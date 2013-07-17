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
  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr(),
      m_remainLineLength(),
      m_remainWidth()
    {
    }

    ImageRowAccessor(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
     :BaseImageRowAccessor(),
      m_ptr(_rowPtr),
      m_remainLineLength(_lineLength),
      m_remainWidth(_width)
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

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels)
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
  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr()
    {
    }

    ImageRowAccessor(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
     :BaseImageRowAccessor(),
      m_ptr(_rowPtr)
    {
      (void)_lineLength;
      (void)_width;
    }

    bool accessPixel(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels=1)
    {
      assert(m_ptr != NULL);

      _pixelPtr = m_ptr;
      m_ptr += _bytes;

      return true;
    }

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels)
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




class BaseImageRowSet
{
  public:
    BaseImageRowSet() {}
};


#warning TODO !_extraCheck not implemented
template <BaseImagePixel::PixelType _PT, typename _UByteCV, ImageDim _rowsCount, bool _extraChecks>
class ImageRowSet : public BaseImageRowSet,
                    private assert_inst<(_rowsCount > 0)> // sanity check
{
  public:
    typedef ImageRow<_PT, _UByteCV, _extraChecks>  Row;
    typedef ImagePixelSet<_PT, _rowsCount>         PixelSet;

    ImageRowSet()
     :BaseImageRowSet(),
      m_rows(),
      m_remainLineLength(),
      m_remainWidth()
    {
    }

    bool resetRowSet(ImageSize _lineLength, ImageDim _width)
    {
      m_remainLineLength = _lineLength;
      m_remainWidth = _width;
      return true;
    }

    ImageDim rowsCount() const
    {
      return _rowsCount;
    }

    Row& operator[](ImageDim _rowIndex)
    {
      assert(_rowIndex < rowsCount());
      return m_rows[_rowIndex];
    }

    const Row& operator[](ImageDim _rowIndex) const
    {
      assert(_rowIndex < rowsCount());
      return m_rows[_rowIndex];
    }


    bool readPixelSet(PixelSet& _pixelSet)
    {
      bool isOk = true;

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        isOk &= this->operator[](rowIndex).readPixel(_pixelSet[rowIndex]);

      return isOk;
    }

    bool writePixelSet(const PixelSet& _pixelSet)
    {
      bool isOk = true;

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        isOk &= this->operator[](rowIndex).writePixel(_pixelSet[rowIndex]);

      return isOk;
    }


  private:
    Row        m_rows[_rowsCount];
    ImageSize  m_remainLineLength;
    ImageDim   m_remainWidth;
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_row_rgb.hpp>
#include <libimage/image_row_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_HPP_
