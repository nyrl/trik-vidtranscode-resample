#ifndef TRIK_LIBIMAGE_IMAGE_ROWSET_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROWSET_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>
#include <libimage/image_pixel.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImageRowSetAccessor
{
  public:
    BaseImageRowSetAccessor() {}
};


template <BaseImagePixel::PixelType _PT, typename _UByteCV, ImageDim _rowsCount, bool _extraChecks>
class ImageRowSetAccessor : public BaseImageRowSetAccessor,
                            private assert_inst<(_rowsCount > 0)> // sanity check
{
  public:
    typedef ImageRow<_PT, _UByteCV, _extraChecks>  RowType;
    typedef ImagePixelSet<_PT, _rowsCount>         PixelSet;

    ImageRowSetAccessor()
     :BaseImageRowSetAccessor(),
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

    RowType& operator[](ImageDim _rowIndex)
    {
      assert(_rowIndex < rowsCount());
      return m_rows[_rowIndex];
    }

    const RowType& operator[](ImageDim _rowIndex) const
    {
      assert(_rowIndex < rowsCount());
      return m_rows[_rowIndex];
    }

    bool accessPixel(ImageSize _bytes, ImageDim _pixels=1)
    {
      if (   m_remainWidth      < _pixels
          || m_remainLineLength < _bytes)
        return false;

      m_remainLineLength -= _bytes;
      m_remainWidth      -= _pixels;

      return true;
    }

    bool accessPixelDontMove(ImageSize _bytes, ImageDim _pixels) const
    {
      if (   m_remainWidth      < _pixels
          || m_remainLineLength < _bytes)
        return false;

      return true;
    }

  private:
    RowType    m_rows[_rowsCount];
    ImageSize  m_remainLineLength;
    ImageDim   m_remainWidth;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImageRowSet
{
  public:
    BaseImageRowSet() {}
};


template <BaseImagePixel::PixelType _PT, typename _UByteCV, ImageDim _rowsCount, bool _extraChecks>
class ImageRowSet : public BaseImageRowSet,
                    public internal::ImageRowSetAccessor<_PT, _UByteCV, _rowsCount, _extraChecks>,
                    private assert_inst<(ImageRow<_PT, _UByteCV, _extraChecks>::s_unifiedPixelAccess > 0)>
{
  public:
    ImageRowSet()
     :ImageRowSetAccessor()
    {
    }

    bool readPixelSet(PixelSet& _pixelSet)
    {
      if (!accessPixel(RowType::s_unifiedPixelAccess))
        return false;

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).readPixel(_pixelSet[rowIndex]))
          return false;

      return true;
    }

    bool writePixelSet(const PixelSet& _pixelSet)
    {
      if (!accessPixel(RowType::s_unifiedPixelAccess))
        return false;

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).writePixel(_pixelSet[rowIndex]))
          return false;

      return true;
    }

  protected:
    typedef internal::ImageRowSetAccessor<_PT, _UByteCV, _rowsCount, _extraChecks> ImageRowSetAccessor;
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_rowset_rgb.hpp>
#include <libimage/image_rowset_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROWSET_HPP_
