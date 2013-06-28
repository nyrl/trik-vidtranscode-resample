#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>

#include "include/internal/image_pixel.hpp"


namespace trik_image
{


class BaseImageRow
{
  public:
    BaseImageRow() {}
    /*virtual*/ ~BaseImageRow() {}

  private:
    BaseImageRow(const BaseImageRow&);
    BaseImageRow& operator=(const BaseImageRow&);
};


template <typename UByteCV>
class BaseImageRowAccessor
{
  public:
    BaseImageRowAccessor()
     :m_ptr(NULL),
      m_remainSize(0),
      m_width(0),
      m_column(0)
    {
    }

    BaseImageRowAccessor(UByteCV* _rowPtr, size_t _lineLength, size_t _width)
     :m_ptr(_rowPtr),
      m_remainSize(_lineLength),
      m_width(_width),
      m_column(0)
    {
    }

  protected:
    bool accessPixel(UByteCV*& _pixelPtr, size_t _bytes)
    {
      if (   m_column >= m_width
          || m_remainSize < _bytes)
        return false;

      if (m_ptr == NULL)
        return false;

      _pixelPtr = m_ptr;

      m_ptr += _bytes;
      m_column += 1;
      m_remainSize -= _bytes;

      return true;
    }

  private:
    BaseImageRowAccessor(const BaseImageRowAccessor&);
    BaseImageRowAccessor& operator=(const BaseImageRowAccessor&);

    UByteCV* m_ptr;
    size_t   m_remainSize;
    size_t   m_width;
    size_t   m_column;
};




template <BaseImagePixel::PixelType PT, typename UByteCV>
class ImageRow : public BaseImageRow, public BaseImageRowAccessor<UByteCV> // Generic instance, non-functional
{
};




class BaseImageRowSet
{
  public:
    BaseImageRowSet() {}
    /*virtual*/ ~BaseImageRowSet() {}

  private:
    BaseImageRowSet(const BaseImageRowSet&);
    BaseImageRowSet& operator=(const BaseImageRowSet&);
};


template <typename BaseImagePixel::PixelType PT, typename UByteCV, size_t _rowsCount>
class ImageRowSet : public BaseImageRowSet
{
  public:
    typedef ImageRow<PT, UByteCV> Row;

    ImageRowSet()
     :BaseImageRowSet(),
      m_rows(),
      m_rowFirst(0)
    {
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

    Row& getRow(size_t _index)
    {
      return m_rows[rowIndex(_index)];
    }

    const Row& getRow(size_t _index) const
    {
      return m_rows[rowIndex(_index)];
    }

  protected:
    size_t rowIndex(size_t _shift) const
    {
      return (m_rowFirst + _shift) % _rowsCount;
    }

  private:
    Row    m_rows[_rowsCount];
    size_t m_rowFirst;
};

template <typename BaseImagePixel::PixelType PT, typename UByteCV>
class ImageRowSet<PT, UByteCV, 0> : public BaseImageRowSet // forbidden
{
};


} // namespace trik_image


#include "include/internal/image_row_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
