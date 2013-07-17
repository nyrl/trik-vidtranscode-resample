#ifndef TRIK_LIBIMAGE_IMAGE_ROWSET_YUV_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROWSET_YUV_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


// YUV444 covered by generic implementation


template <typename _UByteCV, ImageDim _rowsCount, bool _extraChecks>
class ImageRowSet<BaseImagePixel::PixelYUV422,
                  _UByteCV,
                  _rowsCount,
                  _extraChecks> : public BaseImageRowSet,
                                  public internal::ImageRowSetAccessor<BaseImagePixel::PixelYUV422,
                                                                       _UByteCV,
                                                                       _rowsCount,
                                                                       _extraChecks>
{
  public:
    ImageRowSet()
     :ImageRowSetAccessor(),
      m_parity(false)
    {
    }

    bool resetRowSet(ImageSize _lineLength, ImageDim _width)
    {
      m_parity = false;
      return ImageRowSetAccessor::resetRowSet(_lineLength, _width);
    }

    bool readPixelSet(PixelSet& _pixelSet)
    {
      if (m_parity)
      {
        if (!accessPixel(4, 2)) // 4 bytes, 2 pixels
          return false;
      }
      else
      {
        if (!accessPixelDontMove(4, 2)) // 4 bytes, 2 pixels
          return false;
      }

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).readPixel(_pixelSet[rowIndex], m_parity))
          return false;

      m_parity = !m_parity;

      return true;
    }

    bool writePixelSet(const PixelSet& _pixelSet)
    {
      if (m_parity)
      {
        if (!accessPixel(4, 2)) // 4 bytes, 2 pixels
          return false;
      }
      else
      {
        if (!accessPixelDontMove(4, 2)) // 4 bytes, 2 pixels
          return false;
      }

      for (ImageDim rowIndex = 0; rowIndex < rowsCount(); ++rowIndex)
        if (!this->operator[](rowIndex).writePixel(_pixelSet[rowIndex], m_parity))
          return false;

      m_parity = !m_parity;

      return true;
    }

  protected:
    typedef internal::ImageRowSetAccessor<BaseImagePixel::PixelYUV422, _UByteCV, _rowsCount, _extraChecks> ImageRowSetAccessor;

  private:
    bool m_parity;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROWSET_YUV_HPP_
