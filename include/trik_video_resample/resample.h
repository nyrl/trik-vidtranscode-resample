#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_H_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_H_

#include <xdc/std.h>
#include <ti/xdais/xdas.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


TrikVideoResampleStatus resampleImage(TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _srcFormat,
                                      const XDAS_UInt8*                      _src,
                                      size_t                                 _srcRows,
                                      size_t                                 _srcColumns,
                                      size_t                                 _srcLineLength,
                                      TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat _dstFormat,
                                      XDAS_UInt8* restrict                   _dst,
                                      size_t                                 _dstRows,
                                      size_t                                 _dstColumns,
                                      size_t                                 _dstLineLength);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_H_
