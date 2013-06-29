#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include "include/internal/stdcpp.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {


template <typename UByteCV>
class ImageRow<BaseImagePixel::PixelRGB565, UByteCV> : public BaseImageRow,
                                                       private internal::ImageRowAccessor<UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB565> PixelType;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    ImageRow(UByteCV* _ptr, size_t _lineLength, size_t _width)
     :BaseImageRow(),
      ImageRowAccessor(_ptr, _lineLength, _width)
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 2))
        return false;

      return _pixel.unpack(ptr[0], ptr[1]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 2))
        return false;

      return _pixel.pack(ptr[0], ptr[1]);
    }

  protected:
    typedef internal::ImageRowAccessor<UByteCV> ImageRowAccessor;
};




template <typename UByteCV>
class ImageRow<BaseImagePixel::PixelRGB888, UByteCV> : public BaseImageRow,
                                                       private internal::ImageRowAccessor<UByteCV>
{
  public:
    typedef ImagePixel<BaseImagePixel::PixelRGB888> PixelType;

    ImageRow()
     :BaseImageRow(),
      ImageRowAccessor()
    {
    }

    ImageRow(UByteCV* _ptr, size_t _lineLength, size_t _width)
     :BaseImageRow(),
      ImageRowAccessor(_ptr, _lineLength, _width)
    {
    }

    bool readPixel(PixelType& _pixel)
    {
      UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 3))
        return false;

      return _pixel.unpack(ptr[0], ptr[1], ptr[2]);
    }

    bool writePixel(const PixelType& _pixel)
    {
      UByteCV* ptr;
      if (!ImageRowAccessor::accessPixel(ptr, 3))
        return false;

      return _pixel.pack(ptr[0], ptr[1], ptr[2]);
    }

  protected:
    typedef internal::ImageRowAccessor<UByteCV> ImageRowAccessor;
};


} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_
