#ifndef TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelYUV444,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               private internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelYUV444> PixelType;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    ImageRow(_UByteCV* _ptr, ImageSize _lineLength, ImageDim _width)
     :BaseImageRow(),
      ImageRowAccessor(_ptr, _lineLength, _width)
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 4))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 4))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * 4;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;
};




template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelYUV422,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               private internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelYUV422> PixelType;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor(),
      m_readParity(false),
      m_writeParity(false)
    {
    }

    ImageRow(_UByteCV* _ptr, ImageSize _lineLength, ImageDim _width)
     :BaseImageRow(),
      ImageRowAccessor(_ptr, _lineLength, _width),
      m_readParity(false),
      m_writeParity(false)
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (m_readParity)
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        m_readParity = false;
        return _pixel.unpack(ptr[2], ptr[1], ptr[3]);
      }
      else
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        m_readParity = true;
        return _pixel.unpack(ptr[0], ptr[1], ptr[3]);
      }
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (m_writeParity)
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        m_writeParity = false;
        return _pixel.pack(ptr[2], ptr[1], ptr[3], true);
      }
      else
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        m_writeParity = true;
        return _pixel.pack(ptr[0], ptr[1], ptr[3], false);
      }
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      if (_width%2 != 0)
        return 0;
      else
        return _width/2 * 4;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;

  private:
    PixelType m_readCachedPixel;
    bool      m_readParity;
    bool      m_writeParity;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
