#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>
#include <algorithm>

#include "include/internal/image_row.hpp"
#include "include/internal/image_pixel.hpp"


/* **** **** **** **** **** */ namespace trik_image /* **** **** **** **** **** */ {


/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageAccessor
{
  protected:
    BaseImageAccessor(size_t _imageSize, size_t _height, size_t _lineLength)
     :m_imageSize(_imageSize),
      m_height(_height),
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

    const size_t& lineLength() const
    {
      return m_lineLength;
    }

    const size_t& height() const
    {
      return m_height;
    }

    size_t lastRow() const
    {
      return m_height == 0 ? 0 : m_height-1;
    }

  private:
    BaseImageAccessor(const BaseImageAccessor&);
    BaseImageAccessor& operator=(const BaseImageAccessor&);

    size_t   m_imageSize;
    size_t   m_height;
    size_t   m_lineLength;
};


template <typename UByteCV>
class ImageAccessor : protected BaseImageAccessor
{
  protected:
    ImageAccessor(UByteCV* _imagePtr, size_t _imageSize, size_t _height, size_t _lineLength)
     :BaseImageAccessor(_imageSize, _height, _lineLength),
      m_ptr(_imagePtr)
    {
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
    ImageAccessor(const ImageAccessor&);
    ImageAccessor& operator=(const ImageAccessor&);

    UByteCV* m_ptr;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImage
{
  public:
    BaseImage() {}
    /*virtual*/ ~BaseImage() {}

  private:
    BaseImage(const BaseImage&);
    BaseImage& operator=(const BaseImage&);
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
      internal::ImageAccessor<UByteCV>(NULL, 0, 0, 0),
      m_width(0)
    {
    }

    Image(size_t	_height,
          size_t	_width)
     :BaseImage(),
      internal::ImageAccessor<UByteCV>(NULL, 0, _height, 0),
      m_width(_width)
    {
    }

    Image(UByteCV*	_imagePtr,
          size_t	_imageSize,
          size_t	_height,
          size_t	_width,
          size_t	_lineLength)
     :BaseImage(),
      internal::ImageAccessor<UByteCV>(_imagePtr, _imageSize, _height, _lineLength),
      m_width(_width)
    {
    }


    bool getRow(RowType& _row, size_t _rowIndex) const
    {
      UByteCV* rowPtr;
      if (!internal::ImageAccessor<UByteCV>::getRowPtr(rowPtr, _rowIndex))
        return false;

      _row = RowType(rowPtr, internal::ImageAccessor<UByteCV>::lineLength(), width());
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
                    std::min(internal::ImageAccessor<UByteCV>::lastRow(), _baseRow+idx)))
          return false;

      return true;
    }

    const size_t& height() const
    {
      return internal::ImageAccessor<UByteCV>::height();
    }

    const size_t& width() const
    {
      return m_width;
    }

  private:
    size_t m_width;
};


} /* **** **** **** **** **** * namespace trik_image * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
