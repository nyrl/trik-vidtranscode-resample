#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


namespace trik_image
{

namespace pixel_rgb
{


} // namespace pixel_rgb




template <typename UByte>
class ImageRow<BaseImagePixel::PixelRGB565, UByte> : public BaseImageRow,
                                                     public BaseImageRowAccessor<UByte>
{
};




template <typename UByte>
class ImageRow<BaseImagePixel::PixelRGB888, UByte> : public BaseImageRow,
                                                     public BaseImageRowAccessor<UByte>
{
};


} // namespace trik_image


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ROW_RGB_HPP_
