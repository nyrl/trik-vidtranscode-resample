#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include "include/internal/stdcpp.hpp"
#include "include/internal/image_pixel.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageRowAccessor
{
  protected:
    BaseImageRowAccessor()
     :m_remainSize(0),
      m_remainWidth(0)
    {
    }

    BaseImageRowAccessor(size_t _lineLength, size_t _width)
     :m_remainSize(_lineLength),
      m_remainWidth(_width)
    {
    }

    bool accessPixelMarkup(size_t _bytes)
    {
      if (   m_remainWidth == 0
          || m_remainSize < _bytes)
        return false;

      m_remainWidth -= 1;
      m_remainSize -= _bytes;

      return true;
    }

  private:
    size_t   m_remainSize;
    size_t   m_remainWidth;
};


template <typename _UByteCV>
class ImageRowAccessor : private BaseImageRowAccessor
{
  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr(NULL)
    {
    }

    ImageRowAccessor(_UByteCV* _rowPtr, size_t _lineLength, size_t _width)
     :BaseImageRowAccessor(_lineLength, _width),
      m_ptr(_rowPtr)
    {
    }

    bool accessPixel(_UByteCV*& _pixelPtr, size_t _bytes)
    {
      if (m_ptr == NULL)
        return false;

      if (!accessPixelMarkup(_bytes))
        return false;

      _pixelPtr = m_ptr;
      m_ptr += _bytes;

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


template <BaseImagePixel::PixelType _PT, typename _UByteCV, size_t _rowsCount>
class ImageRowSet : public BaseImageRowSet,
                    private assert_inst<(_rowsCount > 0)> // sanity check
{
  public:
    typedef ImageRow<_PT, _UByteCV>        Row;
    typedef ImagePixelSet<_PT, _rowsCount> PixelSet;

    ImageRowSet()
     :BaseImageRowSet(),
      m_rows(),
      m_rowFirst(0)
    {
    }

    void reset()
    {
      m_rowFirst = 0;
      for (size_t row = 0; row < rowsCount(); ++row)
        m_rows[row] = Row();
    }

    size_t rowsCount() const
    {
      return _rowsCount;
    }

    Row& prepareNewRow()
    {
      const size_t currentRow = m_rowFirst;
      m_rowFirst = rowIndex(1);
      return m_rows[currentRow];
    }

    Row& operator[](size_t _index)
    {
      return m_rows[rowIndex(_index)];
    }

    const Row& operator[](size_t _index) const
    {
      return m_rows[rowIndex(_index)];
    }


    bool readPixelSet(PixelSet& _pixelSet)
    {
      bool isOk = true;
      for (size_t index = 0; index < _rowsCount; ++index)
        isOk &= operator[](index).readPixel(_pixelSet[index]);
      return isOk;
    }

    bool writePixelSet(const PixelSet& _pixelSet)
    {
      bool isOk = true;
      for (size_t index = 0; index < _rowsCount; ++index)
        isOk &= operator[](index).writePixel(_pixelSet[index]);
      return isOk;
    }


  protected:
    size_t rowIndex(size_t _index) const
    {
      return (m_rowFirst + _index) % _rowsCount;
    }

  private:
    Row    m_rows[_rowsCount];
    size_t m_rowFirst;
};


} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include "include/internal/image_row_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
