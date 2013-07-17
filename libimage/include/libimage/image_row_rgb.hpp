#ifndef TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelRGB565,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               private internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB565> PixelType;

    static const ImageSize s_unifiedPixelAccess = 2;

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
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.unpack(ptr[0], ptr[1]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.pack(ptr[0], ptr[1]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_unifiedPixelAccess;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;
};




template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelRGB565X,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               private internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB565X> PixelType;

    static const ImageSize s_unifiedPixelAccess = 2;

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
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.unpack(ptr[0], ptr[1]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.pack(ptr[0], ptr[1]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_unifiedPixelAccess;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;
};




template <typename _UByteCV, bool _extraChecks>
class ImageRow<BaseImagePixel::PixelRGB888,
               _UByteCV,
               _extraChecks> : public BaseImageRow,
                               private internal::ImageRowAccessor<_UByteCV, _extraChecks>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB888> PixelType;

    static const ImageSize s_unifiedPixelAccess = 3;

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
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_unifiedPixelAccess))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_unifiedPixelAccess;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV, _extraChecks> ImageRowAccessor;
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_
