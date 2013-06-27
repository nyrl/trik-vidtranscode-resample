#include "trik_vidtranscode_resample.h"

#include <algorithm>
#include <cstring>

#include "include/trik_video_resample/iface.h"
#include "include/trik_video_resample/iface_helpers.h"
#include "include/trik_video_resample/resample.h"


static const char s_trikVideoResampleVersion[] = "1.00.00.00";


const TRIK_VIDTRANSCODE_RESAMPLE_Params* getDefaultParams(void)
{
  static const TRIK_VIDTRANSCODE_RESAMPLE_Params s_defaultParams = {
    {							/* m_base, IVIDTRANSCODE_Params */
      sizeof(TRIK_VIDTRANSCODE_RESAMPLE_Params),	/* size = sizeof this struct */
      1,						/* numOutputStreams = only one output stream by default */
      TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888,		/* formatInput = RGB888 */
      {
        TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565,	/* formatOutput[0] = RGB565 */
        TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_UNKNOWN,	/* formatOutput[1] - disabled */
      },
      1024,						/* maxHeightInput = up to 1024wx1024h */
      1024,						/* maxWidthInput = see maxHeightInput */
      60000,						/* maxFrameRateInput = up to 60fps */
      -1,						/* maxBitRateInput = undefined */
      {
        1024,						/* maxHeightOutput[0] = up to 1024wx1024h */
        1024,						/* maxHeightOutput[1] = up to 1024wx1024h */
      },
      {
        1024,						/* maxWidthOutput[0] = see maxHeightOutput */
        1024,						/* maxWidthOutput[1] = see maxHeightOutput */
      },
      {
        -1,						/* maxFrameRateOutput[0] = undefined */
        -1,						/* maxFrameRateOutput[1] = undefined */
      },
      {
        -1,						/* maxBitRateOutput[0] = undefined */
        -1,						/* maxBitRateOutput[1] = undefined */
      },
      XDM_BYTE						/* dataEndianness = byte only */
    }
  };

  return &s_defaultParams;
}


const TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams* getDefaultDynamicParams(void)
{
  static const TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams s_defaultDynamicParams = {
    {								/* m_base, IVIDTRANSCODE_DynamicParams */
      sizeof(TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams),		/* size = size of structure */
      0,							/* readHeaderOnlyFlag = false */
      {
        XDAS_FALSE,						/* keepInputResolutionFlag[0] = false, override resolution */
        XDAS_TRUE,						/* keepInputResolutionFlag[1] - disabled */
      },
      {
        240,							/* outputHeight[0] = default output 320wx240h */
        0,							/* outputHeight[1] - don't care */
      },
      {
        320,							/* outputWidth[0] = see outputHeight */
        0,							/* outputWidth[1] - don't care */
      },
      {
        XDAS_TRUE,						/* keepInputFrameRateFlag[0] = keep framerate */
        XDAS_TRUE,						/* keepInputFrameRateFlag[1] - don't care */
      },
      -1,							/* inputFrameRate = ignored when keepInputFrameRateFlag */
      {
        -1,							/* outputFrameRate[0] = ignored when keepInputFrameRateFlag */
        -1,							/* outputFrameRate[1] - don't care */
      },
      {
        -1,							/* targetBitRate[0] = ignored by codec */
        -1,							/* targetBitRate[1] - don't care */
      },
      {
        IVIDEO_NONE,						/* rateControl[0] = ignored by codec */
        IVIDEO_NONE,						/* rateControl[1] - don't care */
      },
      {
        XDAS_TRUE,						/* keepInputGOPFlag[0] = ignored by codec */
        XDAS_TRUE,						/* keepInputGOPFlag[1] - don't care */
      },
      {
        1,							/* intraFrameInterval[0] = ignored by codec */
        1,							/* intraFrameInterval[1] - don't care */
      },
      {
        0,							/* interFrameInterval[0] = ignored by codec */
        0,							/* interFrameInterval[1] - don't care */
      },
      {
        IVIDEO_NA_FRAME,					/* forceFrame[0] = ignored by codec */
        IVIDEO_NA_FRAME,					/* forceFrame[1] - don't care */
      },
      {
        XDAS_FALSE,						/* frameSkipTranscodeFlag[0] = ignored by codec */
        XDAS_FALSE,						/* frameSkipTranscodeFlag[1] - don't care */
      },
    },
    -1,								/* inputHeight - derived from params in initObj*/
    -1,								/* inputWidth  - derived from params in initObj*/
    -1,								/* inputLineLength - default, to be calculated base on width */
    {
      -1,							/* outputLineLength - default, to be calculated base on width */
      -1,							/* outputLineLength - default, to be calculated base on width */
    }
  };

  return &s_defaultDynamicParams;
}


void handleBuildDynamicParams(TrikVideoResampleHandle* _handle)
{
  _handle->m_dynamicParams.inputHeight = _handle->m_params.base.maxHeightInput;
  _handle->m_dynamicParams.inputWidth  = _handle->m_params.base.maxWidthInput;
  _handle->m_dynamicParams.inputLineLength = -1;

  for (XDAS_Int32 outIndex = 0; outIndex < _handle->m_params.base.numOutputStreams; ++outIndex)
    _handle->m_dynamicParams.outputLineLength[outIndex] = -1;
}


bool handleVerifyParams(const TrikVideoResampleHandle* _handle)
{
  if (_handle->m_params.base.dataEndianness != XDM_BYTE)
    return false;

  if (   _handle->m_dynamicParams.inputHeight < 0
      || _handle->m_dynamicParams.inputWidth < 0)
    return false;

  if (_handle->m_params.base.numOutputStreams < 0)
    return false;

  return true;
}


bool handlePickOutputParams(const TrikVideoResampleHandle*	_handle,
                            XDAS_Int32				_iStreamIndex,
                            XDAS_Int32* restrict		_iFormat,
                            XDAS_Int32* restrict		_iHeight,
                            XDAS_Int32* restrict		_iWidth,
                            XDAS_Int32* restrict		_iLineLength)
{
  if (   _handle == NULL
      || _iStreamIndex < 0
      || _handle->m_params.base.numOutputStreams <= _iStreamIndex)
    return false;

  *_iFormat		= _handle->m_params.base.formatOutput[_iStreamIndex];

  if (_handle->m_dynamicParams.base.keepInputResolutionFlag[_iStreamIndex])
  {
    *_iHeight		= _handle->m_dynamicParams.inputHeight;
    *_iWidth		= _handle->m_dynamicParams.inputWidth;
    *_iLineLength	= -1;
  }
  else
  {
    *_iHeight		= _handle->m_dynamicParams.base.outputHeight[_iStreamIndex];
    *_iWidth		= _handle->m_dynamicParams.base.outputWidth[_iStreamIndex];
    *_iLineLength	= _handle->m_dynamicParams.outputLineLength[_iStreamIndex];
  }

  return true;
}


bool handlePickInputParams(const TrikVideoResampleHandle*	_handle,
                           XDAS_Int32* restrict			_iFormat,
                           XDAS_Int32* restrict			_iHeight,
                           XDAS_Int32* restrict			_iWidth,
                           XDAS_Int32* restrict			_iLineLength)
{
  if (_handle == NULL)
    return false;

  *_iFormat	= _handle->m_params.base.formatInput;
  *_iHeight	= _handle->m_dynamicParams.inputHeight;
  *_iWidth	= _handle->m_dynamicParams.inputWidth;
  *_iLineLength	= _handle->m_dynamicParams.inputLineLength;

  return true;
}


bool reportVersion(XDAS_Int8* restrict	_iBuffer,
                   XDAS_Int32		_iBufferSize)
{
  static const size_t versionSize = sizeof(s_trikVideoResampleVersion) / sizeof(s_trikVideoResampleVersion[0]);
  if (_iBuffer == NULL || _iBufferSize < versionSize)
    return false;

  std::copy(s_trikVideoResampleVersion, s_trikVideoResampleVersion+versionSize, _iBuffer);
  return true;
}




static bool getVideoFormat(XDAS_Int32 _iFormat, TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat& _format)
{
  switch (_iFormat)
  {
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888: _format = TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888; return true;
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565: _format = TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565; return true;
    default: return false;
  }
}

static size_t calcLineLength(const TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat& _format, size_t _width)
{
  switch (_format)
  {
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB888: return _width*3;
    case TRIK_VIDEO_RESAMPLE_VIDEO_FORMAT_RGB565: return _width*2;
    default: return 0;
  }
}

static size_t calcBufferSize(size_t _height, size_t _lineLenght)
{
  return _height * _lineLenght;
}


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
                                       XDAS_Int32			_iOutLineLength)
{
  if (_iInBuf == NULL || _iOutBuf == NULL)
    return TRIK_VIDEO_RESAMPLE_STATUS_INVALID_ARGUMENTS;


  TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat inFormat;
  if (!getVideoFormat(_iInFormat, inFormat))
    return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_IN_FORMAT;

  if (_iInWidth < 0 || _iInHeight < 0)
    return TRIK_VIDEO_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  const size_t inWidth = _iInWidth;
  const size_t inHeight = _iInHeight;

  if (_iInLineLength <= 0)
    _iInLineLength = calcLineLength(inFormat, inWidth);
  if (_iInLineLength <= 0)
    return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_IN_FORMAT;
  const size_t inLineLength = _iInLineLength;

  if (   _iInBufSize < 0
      || _iInBufSize < calcBufferSize(inHeight, inLineLength))
    return TRIK_VIDEO_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  const XDAS_UInt8* restrict inBuffer = reinterpret_cast<const XDAS_UInt8*>(_iInBuf);


  TRIK_VIDTRANSCODE_RESAMPLE_VideoFormat outFormat;
  if (!getVideoFormat(_iInFormat, outFormat))
    return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT;

  if (_iOutWidth < 0 || _iOutHeight < 0)
    return TRIK_VIDEO_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  const size_t outWidth = _iOutWidth;
  const size_t outHeight = _iOutHeight;

  if (_iOutLineLength <= 0)
    _iOutLineLength = calcLineLength(outFormat, outWidth);
  if (_iOutLineLength <= 0)
    return TRIK_VIDEO_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT;
  const size_t outLineLength = _iOutLineLength;

  *_iOutBufUsed = calcBufferSize(outHeight, outLineLength);
  if (   _iOutBufSize < 0
      || _iOutBufSize < *_iOutBufUsed)
    return TRIK_VIDEO_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  XDAS_UInt8* restrict outBuffer = reinterpret_cast<XDAS_UInt8*>(_iOutBuf);


  return resampleImage( inFormat,  inBuffer,  inHeight,  inWidth,  inLineLength,
                       outFormat, outBuffer, outHeight, outWidth, outLineLength);

}

