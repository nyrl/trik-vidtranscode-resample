#ifndef TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


template <typename _UByteCV>
class ImageRow<BaseImagePixel::PixelYUV444, _UByteCV> : public BaseImageRow,
                                                        public internal::ImageRowAccessor<_UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelYUV444> PixelType;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_accessBytes;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV> ImageRowAccessor;

  private:
    static const ImageSize s_accessBytes = 4;
};




template <typename _UByteCV>
class ImageRow<BaseImagePixel::PixelYUV422, _UByteCV> : public BaseImageRow,
                                                        public internal::ImageRowAccessor<_UByteCV>
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

    bool reset(_UByteCV* _rowPtr, ImageSize _lineLength, ImageDim _width)
    {
      m_readParity  = false;
      m_writeParity = false;
      return ImageRowAccessor::reset(_rowPtr, _lineLength, _width);
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
    typedef internal::ImageRowAccessor<_UByteCV> ImageRowAccessor;

  private:
    PixelType m_readCachedPixel;
    bool      m_readParity;
    bool      m_writeParity;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
