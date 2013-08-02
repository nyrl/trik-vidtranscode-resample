#include "trik_vidtranscode_resample.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include "internal/vidtranscode_resample_iface.h"
#include "internal/vidtranscode_resample_helpers.h"
#include <libimage/image.hpp>
#include <libimage/image_algo.hpp>


static const char s_trikVideoResampleVersion[] = "1.00.00.00";


const TRIK_VIDTRANSCODE_RESAMPLE_Params* getDefaultParams(void)
{
  static const TRIK_VIDTRANSCODE_RESAMPLE_Params s_defaultParams = {
    {							/* m_base, IVIDTRANSCODE_Params */
      sizeof(TRIK_VIDTRANSCODE_RESAMPLE_Params),	/* size = sizeof this struct */
      1,						/* numOutputStreams = only one output stream by default */
      TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB888,	/* formatInput = RGB888 */
      {
        TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB565X,/* formatOutput[0] = RGB565X */
        TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_UNKNOWN,/* formatOutput[1] - disabled */
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

  for (XDAS_Int32 outIndex = 0;
       outIndex < sizeof(_handle->m_dynamicParams.outputLineLength)/sizeof(*_handle->m_dynamicParams.outputLineLength);
       ++outIndex)
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




static bool convertVideoFormat(XDAS_Int32 _iFormat, trik::libimage::BaseImagePixel::PixelType& _pixelType)
{
  switch (_iFormat)
  {
    case TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB888:  _pixelType = trik::libimage::BaseImagePixel::PixelRGB888;  return true;
    case TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB565:  _pixelType = trik::libimage::BaseImagePixel::PixelRGB565;  return true;
    case TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_RGB565X: _pixelType = trik::libimage::BaseImagePixel::PixelRGB565X; return true;
    case TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_YUV422:  _pixelType = trik::libimage::BaseImagePixel::PixelYUV422;  return true;
    case TRIK_VIDTRANSCODE_RESAMPLE_VIDEO_FORMAT_YUV444:  _pixelType = trik::libimage::BaseImagePixel::PixelYUV444;  return true;
    default: return false;
  }
}


template <trik::libimage::BaseImagePixel::PixelType         _PixelTypeSrc,
          trik::libimage::BaseImagePixel::PixelType         _PixelTypeDst,
          trik::libimage::BaseImageAlgorithm::AlgorithmType _Algorithm>
static bool resampleBufferImpl(const XDAS_UInt8* restrict _inBuffer,
                               const size_t&              _inBufferSize,
                               const size_t&              _inWidth,
                               const size_t&              _inHeight,
                               const size_t&              _inLineLength,
                               XDAS_UInt8* restrict       _outBuffer,
                               size_t&                    _outBufferSize,
                               const size_t&              _outWidth,
                               const size_t&              _outHeight,
                               const size_t&              _outLineLength)
{
  typedef trik::libimage::Image<_PixelTypeSrc, const XDAS_UInt8> ImageSrc;
  typedef trik::libimage::Image<_PixelTypeDst, XDAS_UInt8>       ImageDst;
  typedef trik::libimage::ImageAlgorithm<_Algorithm, ImageSrc, ImageDst> Algorithm;

  ImageSrc imageSrc(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength);
  ImageDst imageDst(_outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength);

  Algorithm algorithm(std::make_pair(s_dspDetectHueFrom, s_dspDetectHueTo),
                      std::make_pair(s_dspDetectSatFrom, s_dspDetectSatTo),
                      std::make_pair(s_dspDetectValFrom, s_dspDetectValTo));

  if (!algorithm(imageSrc, imageDst))
    return false;

  _outBufferSize = imageDst.actualImageSize();
  return true;
}

template <trik::libimage::BaseImageAlgorithm::AlgorithmType _Algorithm>
static TrikVideoResampleStatus resampleBufferAlgorithmImpl(const XDAS_UInt8* restrict _inBuffer,
                                                           const size_t&              _inBufferSize,
                                                           const trik::libimage::BaseImagePixel::PixelType& _inPixelType,
                                                           const size_t&              _inWidth,
                                                           const size_t&              _inHeight,
                                                           const size_t&              _inLineLength,
                                                           XDAS_UInt8* restrict       _outBuffer,
                                                           size_t&                    _outBufferSize,
                                                           const trik::libimage::BaseImagePixel::PixelType& _outPixelType,
                                                           const size_t&              _outWidth,
                                                           const size_t&              _outHeight,
                                                           const size_t&              _outLineLength)
{
  // YUV422 camera -> RGB565X (trik lcd)
  if (        _inPixelType  == trik::libimage::BaseImagePixel::PixelYUV422
           && _outPixelType == trik::libimage::BaseImagePixel::PixelRGB565X)
  {
    if (!resampleBufferImpl<trik::libimage::BaseImagePixel::PixelYUV422,
                            trik::libimage::BaseImagePixel::PixelRGB565X,
                            _Algorithm>(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength,
                                        _outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength))
      return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED;
  }
  // YUV422 camera -> RGB888 plane
  else if (   _inPixelType  == trik::libimage::BaseImagePixel::PixelYUV422
           && _outPixelType == trik::libimage::BaseImagePixel::PixelRGB888)
  {
    if (!resampleBufferImpl<trik::libimage::BaseImagePixel::PixelYUV422,
                            trik::libimage::BaseImagePixel::PixelRGB888,
                            _Algorithm>(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength,
                                        _outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength))
      return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED;
  }
  // RGB888 compat camera -> RGB565X (trik lcd)
  else if (   _inPixelType  == trik::libimage::BaseImagePixel::PixelRGB888
           && _outPixelType == trik::libimage::BaseImagePixel::PixelRGB565X)
  {
    if (!resampleBufferImpl<trik::libimage::BaseImagePixel::PixelRGB888,
                            trik::libimage::BaseImagePixel::PixelRGB565X,
                            _Algorithm>(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength,
                                        _outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength))
      return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED;
  }
  // For testing purposes
  else if (   _inPixelType  == trik::libimage::BaseImagePixel::PixelRGB888
           && _outPixelType == trik::libimage::BaseImagePixel::PixelRGB888)
  {
    if (!resampleBufferImpl<trik::libimage::BaseImagePixel::PixelRGB888,
                            trik::libimage::BaseImagePixel::PixelRGB888,
                            _Algorithm>(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength,
                                        _outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength))
      return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED;
  }
  else if (   _inPixelType  == trik::libimage::BaseImagePixel::PixelRGB888
           && _outPixelType == trik::libimage::BaseImagePixel::PixelRGB565)
  {
    if (!resampleBufferImpl<trik::libimage::BaseImagePixel::PixelRGB888,
                            trik::libimage::BaseImagePixel::PixelRGB565,
                            _Algorithm>(_inBuffer,  _inBufferSize,  _inWidth,  _inHeight,  _inLineLength,
                                        _outBuffer, _outBufferSize, _outWidth, _outHeight, _outLineLength))
      return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_FAILED;
  }
  else
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INCOMPATIBLE_FORMATS;

  return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OK;
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
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INVALID_ARGUMENTS;


  trik::libimage::BaseImagePixel::PixelType inPixelType;
  if (!convertVideoFormat(_iInFormat, inPixelType))
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_UNKNOWN_IN_FORMAT;

  if (_iInBufSize < 0 || _iInWidth < 0 || _iInHeight < 0)
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  const size_t               inBufferSize = _iInBufSize;
  const size_t               inWidth      = _iInWidth;
  const size_t               inHeight     = _iInHeight;
  const size_t               inLineLength = _iInLineLength<=0 ? 0 : _iInLineLength;
  const XDAS_UInt8* restrict inBuffer     = reinterpret_cast<const XDAS_UInt8*>(_iInBuf);


  trik::libimage::BaseImagePixel::PixelType outPixelType;
  if (!convertVideoFormat(_iOutFormat, outPixelType))
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_UNKNOWN_OUT_FORMAT;

  if (_iOutBufSize < 0 || _iOutWidth < 0 || _iOutHeight < 0)
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INVALID_ARGUMENTS;
  size_t                     outBufferSize = _iOutBufSize;
  const size_t               outWidth      = _iOutWidth;
  const size_t               outHeight     = _iOutHeight;
  const size_t               outLineLength = _iOutLineLength<=0 ? 0 : _iOutLineLength;
  XDAS_UInt8* restrict       outBuffer     = reinterpret_cast<XDAS_UInt8*>(_iOutBuf);

  if (_iOutBufUsed == NULL)
    return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_INVALID_ARGUMENTS;


  TrikVideoResampleStatus result
    = resampleBufferAlgorithmImpl<trik::libimage::BaseImageAlgorithm::AlgoDetector>(inBuffer,  inBufferSize,  inPixelType,
                                                                                    inWidth,  inHeight,  inLineLength,
                                                                                    outBuffer, outBufferSize, outPixelType,
                                                                                    outWidth, outHeight, outLineLength);
  if (result != TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OK)
    return result;

  *_iOutBufUsed = outBufferSize;
  return TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OK;
}

