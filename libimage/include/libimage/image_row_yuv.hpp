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

    bool readPixel(PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    bool writePixel(const PixelType& _pixel, ImageDim _column)
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
      ImageRowAccessor()
    {
    }

    bool readPixel(PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (_column % 2 == 0)
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4)) // 4 bytes, 2 pixels
          return false;

        return _pixel.unpack(ptr[0], ptr[1], ptr[3]);
      }
      else
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4)) // 4 bytes, 2 pixels
          return false;

        return _pixel.unpack(ptr[2], ptr[1], ptr[3]);
      }
    }

    bool writePixel(const PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (_column % 2 == 0)
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4)) // 4 bytes, 2 pixels
          return false;

        return _pixel.pack(ptr[0], ptr[1], ptr[3], false);
      }
      else
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4)) // 4 bytes, 2 pixels
          return false;

        return _pixel.pack(ptr[2], ptr[1], ptr[3], true);
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
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
