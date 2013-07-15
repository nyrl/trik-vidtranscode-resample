#ifndef TRIK_LIBIMAGE_IMAGE_HPP_
#define TRIK_LIBIMAGE_IMAGE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <algorithm>

#include <libimage/stdcpp.hpp>
#include <libimage/image_row.hpp>
#include <libimage/image_pixel.hpp>


#warning TODO: add 'restrict' to UByteCVs

/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageAccessor
{
  protected:
    BaseImageAccessor(size_t _imageSize, size_t _width, size_t _height, size_t _lineLength)
     :m_imageSize(_imageSize),
      m_width(_width),
      m_height(_height),
      m_lineLength(_lineLength)
    {
    }

    bool rowRangeCheck(size_t _rowIndex, size_t& _ofs) const
    {
      if (   _rowIndex >= m_height
          || (_rowIndex+1)*m_lineLength > m_imageSize)
        return false;

      _ofs = _rowIndex*m_lineLength;
      return true;
    }

    size_t width() const
    {
      return m_width;
    }

    size_t height() const
    {
      return m_height;
    }

    size_t lineLength() const
    {
      return m_lineLength;
    }

    size_t imageSize() const
    {
      return m_imageSize;
    }

    size_t actualImageSize() const
    {
      return m_height * m_lineLength;
    }

    size_t lastRow() const
    {
      return m_height == 0 ? 0 : m_height-1;
    }

  private:
    size_t   m_imageSize;
    size_t   m_width;
    size_t   m_height;
    size_t   m_lineLength;
};


template <typename _UByteCV>
class ImageAccessor : protected BaseImageAccessor
{
  protected:
    ImageAccessor(_UByteCV* _imagePtr, size_t _imageSize, size_t _width, size_t _height, size_t _lineLength)
     :BaseImageAccessor(_imageSize, _width, _height, _lineLength),
      m_ptr(_imagePtr)
    {
    }

    _UByteCV* getPtr() const
    {
      return m_ptr;
    }

    bool getRowPtr(_UByteCV*& _rowPtr, size_t _rowIndex) const
    {
      if (m_ptr == NULL)
        return false;

      size_t ofs;
      if (!rowRangeCheck(_rowIndex, ofs))
        return false;

      _rowPtr = m_ptr + ofs;
      return true;
    }

  private:
    _UByteCV* m_ptr;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImage
{
  public:
    BaseImage() {}
};




template <BaseImagePixel::PixelType _PT, typename _UByteCV>
class Image : public BaseImage,
              private internal::ImageAccessor<_UByteCV>
{
  protected:
    typedef internal::ImageAccessor<_UByteCV> ImageAccessor;

  public:
    static const BaseImagePixel::PixelType PT = _PT;
    typedef _UByteCV                UByteCV;
    typedef ImageRow<_PT, _UByteCV> RowType;


    Image()
     :BaseImage(),
      ImageAccessor(NULL, 0, 0, 0, 0)
    {
    }

    Image(size_t	_width,
          size_t	_height)
     :BaseImage(),
      ImageAccessor(NULL, 0, _width, _height, fixupLineLength(_width, 0))
    {
    }

    Image(_UByteCV*	_imagePtr,
          size_t	_imageSize,
          size_t	_width,
          size_t	_height,
          size_t	_lineLength)
     :BaseImage(),
      ImageAccessor(_imagePtr, _imageSize, _width, _height, fixupLineLength(_width, _lineLength))
    {
    }


    bool getRow(RowType& _row, size_t _rowIndex) const
    {
      _UByteCV* rowPtr;
      if (!ImageAccessor::getRowPtr(rowPtr, _rowIndex))
        return false;

      _row = RowType(rowPtr, ImageAccessor::lineLength(), width());
      return true;
    }

    template <size_t _rowsBefore, size_t _rowsAfter>
    bool getRowSet(ImageRowSet<_PT, _UByteCV, _rowsBefore+1+_rowsAfter>& _rowSet, size_t _baseRow) const
    {
      assert(_rowSet.rowsCount() == _rowsBefore+1+_rowsAfter);

      for (size_t idx = _rowsBefore; idx > 0; --idx)
      {
#warning TODO: minor optimization might be applied if conditional statement moved out of loop
        const size_t ridx = (idx >= _baseRow) ? 0 : _baseRow-idx;
        if (!getRow(_rowSet.prepareNewRow(), ridx))
          return false;
      }

      if (!getRow(_rowSet.prepareNewRow(), _baseRow))
        return false;

      for (size_t idx = 1; idx <= _rowsAfter; ++idx)
      {
#warning TODO: minor optimization might be applied if conditional statement moved out of loop
        const size_t ridx = std::min(_baseRow+idx, ImageAccessor::lastRow());
        if (!getRow(_rowSet.prepareNewRow(), ridx))
          return false;
      }

      return true;
    }

    using ImageAccessor::width;
    using ImageAccessor::height;
    using ImageAccessor::imageSize;
    using ImageAccessor::actualImageSize;
    using ImageAccessor::getPtr;

  protected:
    static size_t fixupLineLength(size_t _width, size_t _lineLength)
    {
      return _lineLength==0 ? RowType::calcLineLength(_width) : _lineLength;
    }
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_HPP_
