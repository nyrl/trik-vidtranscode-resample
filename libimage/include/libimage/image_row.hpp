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
    BaseImageRowAccessor()
     :m_remainSize(),
      m_remainWidth()
    {
    }

    BaseImageRowAccessor(ImageSize _lineLength, ImageDim _width)
     :m_remainSize(_lineLength),
      m_remainWidth(_width)
    {
    }

    bool accessPixelCheck(ImageSize _bytes, ImageDim _pixels)
    {
      return m_remainWidth >= _pixels
          && m_remainSize >= _bytes;
    }

    bool accessPixelMarkup(ImageSize _bytes, ImageDim _pixels)
    {
      if (!accessPixelCheck(_bytes, _pixels))
        return false;

      m_remainWidth -= _pixels;
      m_remainSize -= _bytes;

      return true;
    }

  private:
    ImageSize  m_remainSize;
    ImageDim   m_remainWidth;
};


template <typename _UByteCV>
class ImageRowAccessor : private BaseImageRowAccessor
{
  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr()
    {
    }

    ImageRowAccessor(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
     :BaseImageRowAccessor(_lineLength, _width),
      m_ptr(_rowPtr)
    {
    }

    bool accessPixel(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels=1)
    {
      assert(m_ptr != NULL);

      if (!accessPixelMarkup(_bytes, _pixels))
        return false;

      _pixelPtr = m_ptr;
      m_ptr += _bytes;

      return true;
    }

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes, ImageDim _pixels)
    {
      assert(m_ptr != NULL);

      if (!accessPixelCheck(_bytes, _pixels))
        return false;

      _pixelPtr = m_ptr;

      return true;
    }

  private:
    _UByteCV* m_ptr;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImageRow
{
  public:
    BaseImageRow() {}
};


template <BaseImagePixel::PixelType _PT, typename _UByteCV>
class ImageRow : public BaseImageRow,
                 private internal::ImageRowAccessor<_UByteCV>,
                 private assert_inst<false> // Generic instance, non-functional
{
};




class BaseImageRowSet
{
  public:
    BaseImageRowSet() {}
};


template <BaseImagePixel::PixelType _PT, typename _UByteCV, ImageDim _rowsCount>
class ImageRowSet : public BaseImageRowSet,
                    private assert_inst<(_rowsCount > 0)> // sanity check
{
  public:
    typedef ImageRow<_PT, _UByteCV>        Row;
    typedef ImagePixelSet<_PT, _rowsCount> PixelSet;

    ImageRowSet()
     :BaseImageRowSet(),
      m_rows(),
      m_rowFirst()
    {
    }

    void reset()
    {
      m_rowFirst = 0;
      for (ImageDim row = 0; row < rowsCount(); ++row)
        m_rows[row] = Row();
    }

    ImageDim rowsCount() const
    {
      return _rowsCount;
    }

    Row& prepareNewRow()
    {
      const ImageDim currentRow = m_rowFirst;
      m_rowFirst = rowIndex(1);
      return m_rows[currentRow];
    }

    Row& operator[](ImageDim _rowIndex)
    {
      return m_rows[rowIndex(_rowIndex)];
    }

    const Row& operator[](ImageDim _rowIndex) const
    {
      return m_rows[rowIndex(_rowIndex)];
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


  protected:
    ImageDim rowIndex(ImageDim _index) const
    {
      return (m_rowFirst + _index) % rowsCount();
    }

  private:
    Row      m_rows[_rowsCount];
    ImageDim m_rowFirst;
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_row_rgb.hpp>
#include <libimage/image_row_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_HPP_
