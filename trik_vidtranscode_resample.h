#ifndef TRIK_VIDTRANSCODE_RESAMPLE_H_
#define TRIK_VIDTRANSCODE_RESAMPLE_H_

#include <xdc/std.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividtranscode.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE ========
 *  Our implementation of the IVIDTRANSCODE interface
 */
extern IVIDTRANSCODE_Fxns TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE;
extern IALG_Fxns TRIK_VIDTRANSCODE_RESAMPLE_IALG;


typedef enum TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat
{
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_UNKNOWN = 0,
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB888 = XDM_CUSTOMENUMBASE,
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB565,
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB565X,
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_YUV444,
  TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_YUV422
} TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat;


typedef struct TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams {
    IVIDTRANSCODE_DynamicParams	base;

    XDAS_Int32			inputHeight;
    XDAS_Int32			inputWidth;
    XDAS_Int32			inputLineLength;

    XDAS_Int32			outputLineLength[2];
} TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams;


typedef struct TRIK_VIDTRANSCODE_RESAMPLE_Params {
    IVIDTRANSCODE_Params	base;
} TRIK_VIDTRANSCODE_RESAMPLE_Params;


typedef struct TRIK_VIDTRANSCODE_RESAMPLE_InArgs {
    IVIDTRANSCODE_InArgs        base;
    XDAS_Int32                  detectHueFrom;
    XDAS_Int32                  detectHueTo;
    XDAS_Int32                  detectSatFrom;
    XDAS_Int32                  detectSatTo;
    XDAS_Int32                  detectValFrom;
    XDAS_Int32                  detectValTo;
} TRIK_VIDTRANSCODE_RESAMPLE_InArgs;




#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // !TRIK_VIDTRANSCODE_RESAMPLE_H_
