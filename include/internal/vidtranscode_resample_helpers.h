#ifndef TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_HELPERS_H_
#define TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_HELPERS_H_

#include <xdc/std.h>
#include <ti/xdais/xdas.h>

#include <stdbool.h>

#include "trik_vidtranscode_resample.h"
#include "internal/vidtranscode_resample_iface.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern char* s_dspInfoOutBuffer;
extern float s_dspDetectHueFrom;
extern float s_dspDetectHueTo;
extern float s_dspDetectSatFrom;
extern float s_dspDetectSatTo;
extern float s_dspDetectValFrom;
extern float s_dspDetectValTo;


const TRIK_VIDTRANSCODE_RESAMPLE_Params*	getDefaultParams(void);
const TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams*	getDefaultDynamicParams(void);

void handleBuildDynamicParams(TrikVideoResampleHandle* _handle);
bool handleVerifyParams(const TrikVideoResampleHandle* _handle);

bool reportVersion(XDAS_Int8* _iBuffer, XDAS_Int32 _iBufferSize);

bool handlePickOutputParams(const TrikVideoResampleHandle*	_handle,
                            XDAS_Int32				_iStreamIndex,
                            XDAS_Int32*				_iFormat,
                            XDAS_Int32*				_iHeight,
                            XDAS_Int32*				_iWidth,
                            XDAS_Int32*				_iLineLength);

bool handlePickInputParams(const TrikVideoResampleHandle*	_handle,
                           XDAS_Int32*				_iFormat,
                           XDAS_Int32*				_iHeight,
                           XDAS_Int32*				_iWidth,
                           XDAS_Int32*				_iLineLength);


typedef enum TrikVideoResampleStatus
{
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OK = 0,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INVALID_ARGUMENTS,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_UNKNOWN_IN_FORMAT,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INCOMPATIBLE_FORMATS,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED,
  TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OTHER

} TrikVideoResampleStatus;


TrikVideoResampleStatus resampleBuffer(const XDAS_Int8* restrict	_iInBuf,
                                       XDAS_Int32			_iInBufSize,
                                       XDAS_Int32			_iInFormat,
                                       XDAS_Int32			_iInHeight,
                                       XDAS_Int32			_iInWidth,
                                       XDAS_Int32			_iInLineLength,
                                       XDAS_Int8* restrict		_iOutBuf,
                                       XDAS_Int32			_iOutBufSize,
                                       XDAS_Int32*			_iOutBufUsed,
                                       XDAS_Int32			_iOutFormat,
                                       XDAS_Int32			_iOutHeight,
                                       XDAS_Int32			_iOutWidth,
                                       XDAS_Int32			_iOutLineLength);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // !TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_HELPERS_H_
