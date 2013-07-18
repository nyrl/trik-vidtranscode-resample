#ifndef TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_
#define TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


template <typename _UByteCV>
class ImageRow<BaseImagePixel::PixelRGB565, _UByteCV> : public BaseImageRow,
                                                        public internal::ImageRowAccessor<_UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB565> PixelType;

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

      return _pixel.unpack(ptr[0], ptr[1]);
    }

    bool writePixel(const PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.pack(ptr[0], ptr[1]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_accessBytes;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV> ImageRowAccessor;

  private:
    static const ImageSize s_accessBytes = 2;
};




template <typename _UByteCV>
class ImageRow<BaseImagePixel::PixelRGB565X, _UByteCV> : public BaseImageRow,
                                                         public internal::ImageRowAccessor<_UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB565X> PixelType;

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

      return _pixel.unpack(ptr[0], ptr[1]);
    }

    bool writePixel(const PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.pack(ptr[0], ptr[1]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_accessBytes;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV> ImageRowAccessor;

  private:
    static const ImageSize s_accessBytes = 2;
};




template <typename _UByteCV>
class ImageRow<BaseImagePixel::PixelRGB888, _UByteCV> : public BaseImageRow,
                                                        public internal::ImageRowAccessor<_UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB888> PixelType;

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

      return _pixel.unpack(ptr[0], ptr[1], ptr[2]);
    }

    bool writePixel(const PixelType& _pixel, ImageDim _column)
    {
      _UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, s_accessBytes))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2]);
    }

    static ImageSize calcLineLength(ImageDim _width)
    {
      return _width * s_accessBytes;
    }

  protected:
    typedef internal::ImageRowAccessor<_UByteCV> ImageRowAccessor;

  private:
    static const ImageSize s_accessBytes = 3;
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ROW_RGB_HPP_
