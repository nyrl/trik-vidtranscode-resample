#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>

#include "include/internal/image_pixel.hpp"


namespace trik_image
{


class BaseImageRow
{
  public:
    BaseImageRow() {}
    virtual ~BaseImageRow() {}

  private:
    BaseImageRow(const BaseImageRow&);
    BaseImageRow& operator=(const BaseImageRow&);
};


template <typename UByteCV>
class BaseImageRowAccessor
{
  public:
    BaseImageRowAccessor(UByteCV* _rowPtr, size_t _lineLength, size_t _width)
     :m_ptr(_rowPtr),
      m_remainSize(_lineLength),
      m_width(_width),
      m_column(0)
    {
    }

  protected:
    bool accessPixel(UByteCV*& _pixelPtr, size_t _bytes)
    {
      if (   m_column >= m_width
          || m_remainSize < _bytes)
        return false;

      if (m_ptr == NULL)
        return false;

      _pixelPtr = m_ptr;

      m_ptr += _bytes;
      m_column += 1;
      m_remainSize -= _bytes;

      return true;
    }

  private:
    BaseImageRowAccessor(const BaseImageRowAccessor&);
    BaseImageRowAccessor& operator=(const BaseImageRowAccessor&);

    UByteCV* m_ptr;
    size_t   m_remainSize;
    size_t   m_width;
    size_t   m_column;
};




template <BaseImagePixel::PixelType PT, typename UByteCV>
class ImageRow : public BaseImageRow, public BaseImageRowAccessor<UByteCV> // Generic instance, non-functional
{
};


} // namespace trik_image


#include "include/internal/image_row_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
