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


template <typename _UByteCV>
class ImageRowAccessor : private BaseImageRowAccessor
{
  public:
    bool reset(_UByteCV* _rowPtr)
    {
      m_ptr              = _rowPtr;
      return true;
    }

  protected:
    ImageRowAccessor()
     :BaseImageRowAccessor(),
      m_ptr()
    {
    }

    bool accessPixel(_UByteCV*& _pixelPtr, ImageSize _bytes)
    {
      assert(m_ptr != NULL);
      _pixelPtr = m_ptr;
      m_ptr += _bytes;

      return true;
    }

    bool accessPixelDontMove(_UByteCV*& _pixelPtr, ImageSize _bytes) const
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
      m_rows()
    {
    }

    bool reset()
    {
      return true;
    }

    ImageDim rowsCount() const
    {
      return _rowsCount;
    }

    Row& operator[](ImageDim _rowIndex)
    {
      return m_rows[_rowIndex];
    }

    const Row& operator[](ImageDim _rowIndex) const
    {
      return m_rows[_rowIndex];
    }


    bool readPixelSet(PixelSet& _pixelSet, ImageDim _column)
    {
      bool isOk = true; // always complete all reads, then indicate failure (if any)

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).readPixel(_pixelSet[rowIndex], _column))
          isOk = false;

      return isOk;
    }

    bool writePixelSet(const PixelSet& _pixelSet, ImageDim _column)
    {
      bool isOk = true; // always complete all reads, then indicate failure (if any)

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).writePixel(_pixelSet[rowIndex], _column))
          isOk = false;

      return isOk;
    }

  private:
    Row      m_rows[_rowsCount];
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_row_rgb.hpp>
#include <libimage/image_row_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_HPP_
