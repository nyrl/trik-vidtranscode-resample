#include <stdlib.h>

#include "include/trik_video_resample/iface_helpers.h"
#include "include/trik_video_resample/resample.h"
#include "include/trik_video_resample/resample_bicubic_rgb.hpp"


TrikVideoResampleStatus resampleImage_rgb888_to_xxx(const XDAS_UInt8*                      _src,
                                                    size_t                                 _srcRows,
                                                    size_t                                 _srcColumns,
                                                    size_t                                 _srcLineLength,
                                                    TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _dstFormat,
                                                    XDAS_UInt8* restrict                   _dst,
                                                    size_t                                 _dstRows,
                                                    size_t                                 _dstColumns,
                                                    size_t                                 _dstLineLength)
{
  switch (_dstFormat)
  {
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888:
      if (!TrikRGBBicubicResampler<XDAS_UInt8,
                                   TrikRGBAccessor<Trik_RGB888, XDAS_UInt8>,
                                   TrikRGBAccessor<Trik_RGB888, XDAS_UInt8> >::resampleImage(_src, _srcRows, _srcColumns, _srcLineLength,
                                                                                             _dst, _dstRows, _dstColumns, _dstLineLength))
        return TRIK_VIDEO_RESAMPLE_STATUS_FAILED;
      else
        return TRIK_VIDEO_RESAMPLE_STATUS_OK;

    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565:
      if (!TrikRGBBicubicResampler<XDAS_UInt8,
                                   TrikRGBAccessor<Trik_RGB888, XDAS_UInt8>,
                                   TrikRGBAccessor<Trik_RGB565, XDAS_UInt8> >::resampleImage(_src, _srcRows, _srcColumns, _srcLineLength,
                                                                                             _dst, _dstRows, _dstColumns, _dstLineLength))
        return TRIK_VIDEO_RESAMPLE_STATUS_FAILED;
      else
        return TRIK_VIDEO_RESAMPLE_STATUS_OK;

    default:
      return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT;
  }
}


TrikVideoResampleStatus resampleImage_rgb565_to_xxx(const XDAS_UInt8*                      _src,
                                                    size_t                                 _srcRows,
                                                    size_t                                 _srcColumns,
                                                    size_t                                 _srcLineLength,
                                                    TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _dstFormat,
                                                    XDAS_UInt8* restrict                   _dst,
                                                    size_t                                 _dstRows,
                                                    size_t                                 _dstColumns,
                                                    size_t                                 _dstLineLength)
{
  switch (_dstFormat)
  {
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888:
      if (!TrikRGBBicubicResampler<XDAS_UInt8,
                                   TrikRGBAccessor<Trik_RGB565, XDAS_UInt8>,
                                   TrikRGBAccessor<Trik_RGB888, XDAS_UInt8> >::resampleImage(_src, _srcRows, _srcColumns, _srcLineLength,
                                                                                             _dst, _dstRows, _dstColumns, _dstLineLength))
        return TRIK_VIDEO_RESAMPLE_STATUS_FAILED;
      else
        return TRIK_VIDEO_RESAMPLE_STATUS_OK;

    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565:
      if (!TrikRGBBicubicResampler<XDAS_UInt8,
                                   TrikRGBAccessor<Trik_RGB565, XDAS_UInt8>,
                                   TrikRGBAccessor<Trik_RGB565, XDAS_UInt8> >::resampleImage(_src, _srcRows, _srcColumns, _srcLineLength,
                                                                                             _dst, _dstRows, _dstColumns, _dstLineLength))
        return TRIK_VIDEO_RESAMPLE_STATUS_FAILED;
      else
        return TRIK_VIDEO_RESAMPLE_STATUS_OK;

    default:
      return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT;
  }
}


TrikVideoResampleStatus resampleImage(TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _srcFormat,
                                      const XDAS_UInt8*                      _src,
                                      size_t                                 _srcRows,
                                      size_t                                 _srcColumns,
                                      size_t                                 _srcLineLength,
                                      TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _dstFormat,
                                      XDAS_UInt8* restrict                   _dst,
                                      size_t                                 _dstRows,
                                      size_t                                 _dstColumns,
                                      size_t                                 _dstLineLength)
{
  switch (_srcFormat)
  {
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888:
      return resampleImage_rgb888_to_xxx(_src, _srcRows, _srcColumns, _srcLineLength,
                                         _dstFormat,
                                         _dst, _dstRows, _dstColumns, _dstLineLength);

    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565:
      return resampleImage_rgb565_to_xxx(_src, _srcRows, _srcColumns, _srcLineLength,
                                         _dstFormat,
                                         _dst, _dstRows, _dstColumns, _dstLineLength);
    default:
      return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_IN_FORMAT;
  }
}


