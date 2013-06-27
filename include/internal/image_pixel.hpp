#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>
#include <climits>


namespace trik_image
{


class BaseImagePixel
{
  public:
    enum PixelType
    {
      PixelRGB565,
      PixelRGB888
    };

    BaseImagePixel() {}
    virtual ~BaseImagePixel() {}

    virtual bool convertTo(BaseImagePixel& _dst) const
    {
      float nr;
      float nb;
      float ng;

      return toNormalizedRGB(nr, nb, ng)
          && _dst.fromNormalizedRGB(nr, nb, ng);
    }

  protected:
    virtual bool toNormalizedRGB(float& _nr, float& _ng, float& _nb) const = 0;
    virtual bool fromNormalizedRGB(const float& _nr, const float& _ng, const float& _nb) = 0;

  private:
    BaseImagePixel(const BaseImagePixel&);
    BaseImagePixel& operator=(const BaseImagePixel&);
};




class BaseImagePixelStorage
{
  protected:
    BaseImagePixelStorage();

    template <typename UType>
    static UType storageBitmask(size_t _size)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      return (static_cast<UType>(1u) << _size) - 1;
    }

    template <typename UType, bool _ofsPositive>
    static UType storageGet(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value >> _ofs) & storageBitmask<UType>(_size);
      else
        return (_value & storageBitmask<UType>(_size)) << _ofs;
    }

    template <typename UType, bool _ofsPositive>
    static UType storageValue(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value & storageBitmask<UType>(_size)) << _ofs;
      else
        return (_value >> _ofs) & storageBitmask<UType>(_size);
    }

  private:
    BaseImagePixelStorage(const BaseImagePixelStorage&);
    BaseImagePixelStorage& operator=(const BaseImagePixelStorage&);
};




template <BaseImagePixel::PixelType PT>
class ImagePixel : public BaseImagePixel, public BaseImagePixelStorage // Generic instance, non-functional
{
};


} // namespace trik_image


#include "include/internal/image_pixel_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
