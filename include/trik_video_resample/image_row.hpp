#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>


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


template <typename UByte>
class BaseImageRowAccessor
{
  public:
    BaseImageRowAccessor(UByte* _ptr, size_t _width, size_t _lineLength)
     :m_ptr(_ptr),
      m_width(_width),
      m_column(0),
      m_remainSize(_lineLength)
    {
      assert(m_ptr != NULL);
    }

    bool accessColumn(UByte*& _ptr, size_t _bytes)
    {
      if (   m_column >= m_width
          || m_remainSize < _bytes)
        return false;

      _ptr = m_ptr;

      m_ptr += _bytes;
      m_column += 1;
      m_remainSize -= _bytes;

      return true;
    }

  private:
    BaseImageRowAccessor(const BaseImageRowAccessor&);
    BaseImageRowAccessor& operator=(const BaseImageRowAccessor&);

    UByte* m_ptr;
    size_t m_width;
    size_t m_column;
    size_t m_remainSize;
};




template <BaseImagePixel::PixelType PT, typename UByte>
class ImageRow : public BaseImageRow, public BaseImageRowAccessor<UByte> // Generic instance, non-functional
{
};


} // namespace trik_image


#include "include/trik_video_resample/image_row_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_HPP_
