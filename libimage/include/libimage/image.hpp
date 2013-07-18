#ifndef TRIK_LIBIMAGE_IMAGE_HPP_
#define TRIK_LIBIMAGE_IMAGE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <algorithm>

#include <libimage/stdcpp.hpp>
#include <libimage/image_defs.hpp>
#include <libimage/image_row.hpp>
#include <libimage/image_pixel.hpp>


#warning TODO: add 'restrict' to UByteCVs

/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageDimension
{
  protected:
    BaseImageDimension(ImageSize _imageSize, ImageDim _width, ImageDim _height, ImageSize _lineLength)
     :m_imageSize(_imageSize),
      m_width(_width),
      m_height(_height),
      m_lineLength(_lineLength)
    {
    }

    ImageDim width() const
    {
      return m_width;
    }

    ImageDim height() const
    {
      return m_height;
    }

    ImageSize lineLength() const
    {
      return m_lineLength;
    }

    ImageSize imageSize() const
    {
      return m_imageSize;
    }

    ImageSize actualImageSize() const
    {
      return m_height * m_lineLength;
    }

    ImageDim firstRow() const
    {
      return 0;
    }

    ImageDim lastRow() const
    {
      return m_height == 0 ? 0 : m_height-1;
    }

  private:
    ImageSize m_imageSize;
    ImageDim  m_width;
    ImageDim  m_height;
    ImageSize m_lineLength;
};




template <typename _UByteCV>
class ImageAccessor : protected BaseImageDimension
{
  protected:
    ImageAccessor(_UByteCV* _imagePtr, ImageSize _imageSize, ImageDim _width, ImageDim _height, ImageSize _lineLength)
     :BaseImageDimension(_imageSize, _width, _height, _lineLength),
      m_ptr(_imagePtr),
      m_accessableRows(std::min(_height, _imageSize/_lineLength))
    {
    }

    operator bool() const
    {
      return m_ptr != NULL;
    }

    bool getRowPtr(_UByteCV*& _rowPtr, ImageDim _rowIndex) const
    {
      assert(m_ptr != NULL);

      // always return _rowPtr as it is likely to be correct and simplifies branch
      _rowPtr = m_ptr + _rowIndex*lineLength();
      return _rowIndex < m_accessableRows;
    }

  private:
    _UByteCV*          m_ptr;
    ImageDim           m_accessableRows;
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
    static const BaseImagePixel::PixelType s_PT = _PT;
    typedef _UByteCV                       UByteCV;
    typedef ImageRow<_PT, _UByteCV>        RowType;


    Image()
     :BaseImage(),
      ImageAccessor(NULL, ImageSize(), ImageDim(), ImageDim(), ImageSize())
    {
    }

    Image(ImageDim	_width,
          ImageDim	_height)
     :BaseImage(),
      ImageAccessor(NULL, ImageSize(), _width, _height, fixupLineLength(_width, ImageSize()))
    {
    }

    Image(_UByteCV*	_imagePtr,
          ImageSize	_imageSize,
          ImageDim	_width,
          ImageDim	_height,
          ImageSize	_lineLength)
     :BaseImage(),
      ImageAccessor(_imagePtr, _imageSize, _width, _height, fixupLineLength(_width, _lineLength))
    {
    }

    bool getRow(RowType& _row, ImageDim _rowIndex) const
    {
      _UByteCV* rowPtr;
      if (!ImageAccessor::getRowPtr(rowPtr, _rowIndex))
        return false;

      _row.reset(rowPtr, ImageAccessor::lineLength(), width());

      return true;
    }

    template <ImageDim _rowsBefore, ImageDim _rowsAfter>
    bool getRowSet(ImageRowSet<_PT, _UByteCV, _rowsBefore+1+_rowsAfter>& _rowSet, ImageDim _baseRow) const
    {
      assert(_rowSet.rowsCount() == _rowsBefore+1+_rowsAfter);

      if (!_rowSet.reset())
        return false;

      ImageDim rowIdx = 0;

      bool isOk = true; // always pick full row set, even with errors

      for (ImageDim idx = _rowsBefore; idx > 0; --idx)
      {
        const ImageDim ridx = (idx >= _baseRow) ? ImageAccessor::firstRow() : _baseRow-idx;
        isOk &= getRow(_rowSet[rowIdx++], ridx);
      }

      isOk &= getRow(_rowSet[rowIdx++], _baseRow);

      for (ImageDim idx = 1; idx <= _rowsAfter; ++idx)
      {
        const ImageDim ridx = std::min(_baseRow+idx, ImageAccessor::lastRow());
        isOk &= getRow(_rowSet[rowIdx++], ridx);
      }

      return isOk;
    }

    using ImageAccessor::operator bool;
    using ImageAccessor::width;
    using ImageAccessor::height;
    using ImageAccessor::imageSize;
    using ImageAccessor::actualImageSize;

  protected:
    static ImageSize fixupLineLength(ImageDim _width, ImageSize _lineLength)
    {
      return _lineLength==0 ? RowType::calcLineLength(_width) : _lineLength;
    }
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_HPP_
