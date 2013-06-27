#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>

#include "include/trik_video_resample/image_row.hpp"
#include "include/trik_video_resample/image_pixel.hpp"


namespace trik_image
{


class BaseImage
{
  public:
    BaseImage() {}
    virtual ~BaseImage() {}

  private:
    BaseImage(const BaseImage&);
    BaseImage& operator=(const BaseImage&);
};


template <typename UByteCV>
class BaseImageAccessor
{
  public:
    BaseImageAccessor(UByteCV* _imagePtr, size_t _imageSize, size_t _height, size_t _lineLength)
     :m_ptr(_imagePtr),
      m_remainSize(_imageSize),
      m_height(_height),
      m_row(0),
      m_lineLength(_lineLength)
    {
    }

  protected:
    bool accessRow(UByteCV*& _rowPtr)
    {
      if (   m_row >= m_height
          || m_remainSize < m_lineLength)
        return false;

      if (m_ptr == NULL)
        return false;

      _rowPtr = m_ptr;

      m_ptr += m_lineLength;
      m_row += 1;
      m_remainSize -= m_lineLength;

      return true;
    }

    const size_t& lineLength() const
    {
      return m_lineLength;
    }

  private:
    BaseImageAccessor(const BaseImageAccessor&);
    BaseImageAccessor& operator=(const BaseImageAccessor&);

    UByteCV* m_ptr;
    size_t   m_remainSize;
    size_t   m_height;
    size_t   m_row;
    size_t   m_lineLength;
};




template <BaseImagePixel::PixelType PT, typename UByteCV>
class Image : public BaseImage, public BaseImageAccessor<UByteCV>
{
  public:
    typedef ImageRow<PT, UByteCV> RowType;

    Image(UByteCV*	_imagePtr,
          size_t	_imageSize,
          size_t	_height,
          size_t	_width,
          size_t	_lineLength)
     :BaseImage(),
      BaseImageAccessor<UByteCV>(_imagePtr, _imageSize, _height, _lineLength),
      m_width(_width)
    {
    }

    bool nextRow(RowType& _row)
    {
      UByteCV* rowPtr;
      if (!accessRow(rowPtr))
        return false;

      _row = RowType(rowPtr, BaseImageAccessor<UByteCV>::lineLength(), m_width);
      return true;
    }

  private:
    size_t m_width;
};


} // namespace trik_image


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_HPP_
