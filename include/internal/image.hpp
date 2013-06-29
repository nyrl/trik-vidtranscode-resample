#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <algorithm>

#include "include/internal/stdcpp.hpp"
#include "include/internal/image_row.hpp"
#include "include/internal/image_pixel.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageAccessor
{
  protected:
    BaseImageAccessor(size_t _imageSize, size_t _height, size_t _width, size_t _lineLength)
     :m_imageSize(_imageSize),
      m_height(_height),
      m_width(_width),
      m_lineLength(_lineLength)
    {
    }

    bool rowRangeCheck(size_t _rowIndex) const
    {
      if (   _rowIndex >= m_height
          || (_rowIndex+1)*m_lineLength > m_imageSize)
        return false;

      return true;
    }

    const size_t& height() const
    {
      return m_height;
    }

    const size_t& width() const
    {
      return m_width;
    }

    const size_t& lineLength() const
    {
      return m_lineLength;
    }

    const size_t& imageSize() const
    {
      return m_imageSize;
    }

    size_t lastRow() const
    {
      return m_height == 0 ? 0 : m_height-1;
    }

  private:
    size_t   m_imageSize;
    size_t   m_height;
    size_t   m_width;
    size_t   m_lineLength;
};


template <typename UByteCV>
class ImageAccessor : protected BaseImageAccessor
{
  protected:
    ImageAccessor(UByteCV* _imagePtr, size_t _imageSize, size_t _height, size_t _width, size_t _lineLength)
     :BaseImageAccessor(_imageSize, _height, _width, _lineLength),
      m_ptr(_imagePtr)
    {
    }

    UByteCV* getPtr() const
    {
      return m_ptr;
    }

    bool getRowPtr(UByteCV*& _rowPtr, size_t _rowIndex) const
    {
      if (m_ptr == NULL)
        return false;

      if (!rowRangeCheck(_rowIndex))
        return false;

      _rowPtr = m_ptr + _rowIndex*lineLength();

      return true;
    }

  private:
    UByteCV* m_ptr;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImage
{
  public:
    BaseImage() {}
};




template <BaseImagePixel::PixelType PT, typename UByteCV>
class Image : public BaseImage,
              private internal::ImageAccessor<UByteCV>
{
  public:
    static const BaseImagePixel::PixelType s_pixelType = PT;
    typedef UByteCV UByteCVType;
    typedef ImageRow<PT, UByteCV> RowType;


    Image()
     :BaseImage(),
      ImageAccessor(NULL, 0, 0, 0, 0)
    {
    }

    Image(size_t	_height,
          size_t	_width)
     :BaseImage(),
      ImageAccessor(NULL, 0, _height, _width, 0)
    {
    }

    Image(UByteCV*	_imagePtr,
          size_t	_imageSize,
          size_t	_height,
          size_t	_width,
          size_t	_lineLength)
     :BaseImage(),
      ImageAccessor(_imagePtr, _imageSize, _height, _width, _lineLength)
    {
    }


    bool getRow(RowType& _row, size_t _rowIndex) const
    {
      UByteCV* rowPtr;
      if (!ImageAccessor::getRowPtr(rowPtr, _rowIndex))
        return false;

      _row = RowType(rowPtr, ImageAccessor::lineLength(), width());
      return true;
    }

    template <size_t _rowsBefore, size_t _rowsAfter>
    bool getRowSet(ImageRowSet<PT, UByteCV, _rowsBefore+1+_rowsAfter>& _rowSet, size_t _baseRow) const
    {
      assert(_rowSet.rowsCount() == _rowsBefore+1+_rowsAfter);

      _rowSet.reset();

      for (size_t idx = _rowsBefore; idx > 0; --idx)
        if (!getRow(_rowSet.prepareNewRow(),
                    (idx >= _baseRow ? 0 : _baseRow-idx)))
          return false;

      if (!getRow(_rowSet.prepareNewRow(), _baseRow))
        return false;

      for (size_t idx = 1; idx <= _rowsAfter; ++idx)
        if (!getRow(_rowSet.prepareNewRow(),
                    std::min(ImageAccessor::lastRow(), _baseRow+idx)))
          return false;

      return true;
    }

    const size_t& height() const
    {
      return ImageAccessor::height();
    }

    const size_t& width() const
    {
      return ImageAccessor::width();
    }

    const size_t& imageSize() const
    {
      return ImageAccessor::imageSize();
    }

    UByteCV* getPtr() const
    {
      return ImageAccessor::getPtr();
    }


  protected:
    typedef internal::ImageAccessor<UByteCV> ImageAccessor;

};


} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
