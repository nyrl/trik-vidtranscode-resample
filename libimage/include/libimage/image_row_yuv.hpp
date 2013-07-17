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
                               public internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelYUV444> PixelType;

    static const ImageSize s_unifiedPixelAccess = 4;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_unifiedPixelAccess;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;
};




template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelYUV422,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               public internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelYUV422> PixelType;

    static const ImageSize s_unifiedPixelAccess = 0;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    bool readPixel(PixelType& _pixel, bool _parity)
    {
      _UByteCV* ptr;
      if (_parity)
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        return _pixel.unpack(ptr[2], ptr[1], ptr[3]);
      }
      else
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        return _pixel.unpack(ptr[0], ptr[1], ptr[3]);
      }
    }

    bool writePixel(const PixelType& _pixel, bool _parity)
    {
      _UByteCV* ptr;
      if (_parity)
      {
        if (!ImageRowAccessor::accessPixel(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

        return _pixel.pack(ptr[2], ptr[1], ptr[3], true);
      }
      else
      {
        if (!ImageRowAccessor::accessPixelDontMove(ptr, 4, 2)) // 4 bytes, 2 pixels
          return false;

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
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_YUV_HPP_
