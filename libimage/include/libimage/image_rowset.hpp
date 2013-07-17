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


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




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


//#include <libimage/image_rowset_rgb.hpp>
//#include <libimage/image_rowset_yuv.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ROWSET_HPP_
