#include <sysexits.h>
#include <unistd.h>
#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <ios>
#include <iostream>
#include <sstream>

#include <libimage/image.hpp>
#include <libimage/image_algo.hpp>

#include "v4l2device.hpp"
#include "filedevice.hpp"


using namespace std;


static trik::libimage::demos::V4L2Input  s_videoSrc(trik::libimage::demos::V4L2Config("/dev/video0", 800, 600), "RGB888");
static trik::libimage::demos::FileOutput s_videoDst(trik::libimage::demos::FileConfig("video.out", 320, 240), "RGB888");
static size_t s_repeatCount = 1;

static pair<float, float> s_detectHue(330.0f, 30.0f); // red
static pair<float, float> s_detectSat(0.5f, 1.0f);
static pair<float, float> s_detectVal(0.5f, 1.0f);



static bool parseConfig(int _argc, char* const _argv[])
{
  struct option long_opts[] = {
    { "src-path",		1,	NULL,	0 }, // 0
    { "src-width",		1,	NULL,	0 },
    { "src-height",		1,	NULL,	0 },
    { "src-format",		1,	NULL,	0 },
    { "dst-path",		1,	NULL,	0 }, // 4
    { "dst-width",		1,	NULL,	0 },
    { "dst-height",		1,	NULL,	0 },
    { "dst-format",		1,	NULL,	0 },
    { "repeat",			1,	NULL,	0 }, // 8
    { "detect-hue-from",	1,	NULL,	0 }, // 9
    { "detect-hue-to",		1,	NULL,	0 },
    { "detect-sat-from",	1,	NULL,	0 },
    { "detect-sat-to",		1,	NULL,	0 },
    { "detect-val-from",	1,	NULL,	0 },
    { "detect-val-to",		1,	NULL,	0 },
    { "help",			0,	NULL,	'?' },
    { NULL,			0,	NULL,	0 },
  };

  const char* argv0 = _argv[0];
  int opt;
  int lopt;

  while ((opt = getopt_long(_argc, _argv, "h", long_opts, &lopt)) != -1)
  {
    switch (opt)
    {
      case 0: // long opt
        switch (lopt)
        {
          case 0:
            if ((istringstream(optarg) >> s_videoSrc.config().path()).fail())
            {
              fprintf(stderr, "Cannot parse src-path argument\n");
              return false;
            }
            break;

          case 1:
            if ((istringstream(optarg) >> s_videoSrc.config().width()).fail())
            {
              fprintf(stderr, "Cannot parse src-width argument\n");
              return false;
            }
            break;

          case 2:
            if ((istringstream(optarg) >> s_videoSrc.config().height()).fail())
            {
              fprintf(stderr, "Cannot parse src-height argument\n");
              return false;
            }
            break;

          case 3:
            if ((istringstream(optarg) >> s_videoSrc.config().format()).fail())
            {
              fprintf(stderr, "Cannot parse src-format argument\n");
              return false;
            }
            break;

          case 4:
            if ((istringstream(optarg) >> s_videoDst.config().path()).fail())
            {
              fprintf(stderr, "Cannot parse dst-path argument\n");
              return false;
            }
            break;

          case 5:
            if ((istringstream(optarg) >> s_videoDst.config().width()).fail())
            {
              fprintf(stderr, "Cannot parse dst-width argument\n");
              return false;
            }
            break;

          case 6:
            if ((istringstream(optarg) >> s_videoDst.config().height()).fail())
            {
              fprintf(stderr, "Cannot parse dst-height argument\n");
              return false;
            }
            break;

          case 7:
            if ((istringstream(optarg) >> s_videoDst.config().format()).fail())
            {
              fprintf(stderr, "Cannot parse dst-format argument\n");
              return false;
            }
            break;

          case 8:
            if ((istringstream(optarg) >> s_repeatCount).fail())
            {
              fprintf(stderr, "Cannot parse repeat argument\n");
              return false;
            }
            break;

          case 9:
            if ((istringstream(optarg) >> s_detectHue.first).fail())
            {
              fprintf(stderr, "Cannot parse detect hue from argument\n");
              return false;
            }
            break;

          case 10:
            if ((istringstream(optarg) >> s_detectHue.second).fail())
            {
              fprintf(stderr, "Cannot parse detect hue to argument\n");
              return false;
            }
            break;

          case 11:
            if ((istringstream(optarg) >> s_detectSat.first).fail())
            {
              fprintf(stderr, "Cannot parse detect sat from argument\n");
              return false;
            }
            break;

          case 12:
            if ((istringstream(optarg) >> s_detectSat.second).fail())
            {
              fprintf(stderr, "Cannot parse detect sat to argument\n");
              return false;
            }
            break;

          case 13:
            if ((istringstream(optarg) >> s_detectVal.first).fail())
            {
              fprintf(stderr, "Cannot parse detect val from argument\n");
              return false;
            }
            break;

          case 14:
            if ((istringstream(optarg) >> s_detectVal.second).fail())
            {
              fprintf(stderr, "Cannot parse detect val to argument\n");
              return false;
            }
            break;

          default:
            return false;
        }
        break;

      case 'h':
      case '?':
        return false;

      default:
        fprintf(stderr, "Unknown argument %#02x/'%c'\n", opt, opt);
        return false;
    }
  }

  return true;
}




template <trik::libimage::BaseImagePixel::PixelType         _PixelTypeSrc,
          trik::libimage::BaseImagePixel::PixelType         _PixelTypeDst,
          trik::libimage::BaseImageAlgorithm::AlgorithmType _Algorithm>
static bool execAlgorithm(const trik::libimage::demos::V4L2Input::Description&  _srcDesc,
                          const trik::libimage::demos::V4L2Input::Frame&        _srcFrame,
                          const trik::libimage::demos::FileOutput::Description& _dstDesc,
                          trik::libimage::demos::FileOutput::Frame&             _dstFrame,
                          const pair<float, float>& _detectHue,
                          const pair<float, float>& _detectSat,
                          const pair<float, float>& _detectVal)
{
  typedef trik::libimage::Image<_PixelTypeSrc, const uint8_t>            ImageSrc;
  typedef trik::libimage::Image<_PixelTypeDst, uint8_t>                  ImageDst;
  typedef trik::libimage::ImageAlgorithm<_Algorithm, ImageSrc, ImageDst> Algorithm;

  ImageSrc imageSrc(_srcFrame.ptr(), _srcFrame.size(),
                    _srcDesc.width(), _srcDesc.height(),
                    _srcDesc.bytesPerLine());
  ImageDst imageDst(_dstFrame.ptr(), _dstFrame.size(),
                    _dstDesc.width(), _dstDesc.height(),
                    _dstDesc.bytesPerLine());

  Algorithm algorithm(_detectHue, _detectSat, _detectVal);

  if (!algorithm(imageSrc, imageDst))
  {
    fprintf(stderr, "algorithm failed\n");
    return false;
  }

  _dstFrame.size(imageDst.actualImageSize());
  return true;
}


static bool detector(const trik::libimage::demos::V4L2Input::Description&  _srcDesc,
                     const trik::libimage::demos::V4L2Input::Frame&        _srcFrame,
                     const trik::libimage::demos::FileOutput::Description& _dstDesc,
                     trik::libimage::demos::FileOutput::Frame&             _dstFrame,
                     const pair<float, float>& _detectHue,
                     const pair<float, float>& _detectSat,
                     const pair<float, float>& _detectVal)
{
  if (_srcDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24 && _dstDesc.format().rawFormat() == V4L2_PIX_FMT_RGB565)
  {
    if (!execAlgorithm<trik::libimage::BaseImagePixel::PixelRGB888,
                       trik::libimage::BaseImagePixel::PixelRGB565,
                       trik::libimage::BaseImageAlgorithm::AlgoDetector>(_srcDesc, _srcFrame, _dstDesc, _dstFrame, _detectHue, _detectSat, _detectVal))
      return false;
  }
  else if (_srcDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24 && _dstDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24)
  {
    if (!execAlgorithm<trik::libimage::BaseImagePixel::PixelRGB888,
                       trik::libimage::BaseImagePixel::PixelRGB888,
                       trik::libimage::BaseImageAlgorithm::AlgoDetector>(_srcDesc, _srcFrame, _dstDesc, _dstFrame, _detectHue, _detectSat, _detectVal))
      return false;
  }
  else
  {
    fprintf(stderr, "algorithm does not know requested conversion\n");
    return false;
  }

  return true;
}




int main(int _argc, char* const _argv[])
{
  int res;

  if (!parseConfig(_argc, _argv))
  {
    fprintf(stderr, "Usage:\n"
                    "  %s [opts]\n"
                    "where opts are:\n"
                    "  --src-path   <path>\n"
                    "  --src-width  <width>\n"
                    "  --src-height <height>\n"
                    "  --src-format <format>\n"
                    "  --dst-path   <path>\n"
                    "  --dst-width  <width>\n"
                    "  --dst-height <height>\n"
                    "  --dst-format <format>\n",
                    "  --repeat     <count>\n",
            _argv[0]);
    exit(EX_USAGE);
  }

  v4l2_log_file = stderr;

  if (!s_videoSrc.open())
    exit(EX_NOINPUT);

  if (!s_videoSrc.start())
    exit(EX_NOINPUT);

  if (!s_videoDst.open())
    exit(EX_CANTCREAT);

  if (!s_videoDst.start())
    exit(EX_CANTCREAT);

  const int videoSrcFd = s_videoSrc.v4l2fd();

  for (size_t repeat = 0; repeat < s_repeatCount; ++repeat)
  {
    int fdsMax;
    fd_set fdsIn;
    FD_ZERO(&fdsIn);
    FD_SET(videoSrcFd, &fdsIn);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if ((res = select(videoSrcFd+1, &fdsIn, NULL, NULL, &timeout)) == -1)
    {
      fprintf(stderr, "select() failed: %d\n", errno);
      exit(EX_OSERR);
    }

    if (!FD_ISSET(videoSrcFd, &fdsIn))
    {
      fprintf(stderr, "video src has not provided frame\n");
      exit(EX_NOINPUT);
    }

    trik::libimage::demos::V4L2Input::Frame      srcFrame;
    trik::libimage::demos::V4L2Input::FrameIndex srcFrameIndex;
    if (!s_videoSrc.getFrame(srcFrame, srcFrameIndex))
      exit(EX_SOFTWARE);

    trik::libimage::demos::FileOutput::Frame     dstFrame;
    if (!s_videoDst.getFrame(dstFrame))
      exit(EX_SOFTWARE);

    if (!detector(s_videoSrc.description(), srcFrame, s_videoDst.description(), dstFrame, s_detectHue, s_detectSat, s_detectVal))
      exit(EX_SOFTWARE);

    if (!s_videoDst.putFrame(dstFrame))
      exit(EX_SOFTWARE);

    if (!s_videoSrc.ungetFrame(srcFrameIndex))
      exit(EX_SOFTWARE);
  }

  s_videoDst.stop();
  s_videoDst.close();
  s_videoSrc.stop();
  s_videoSrc.close();

  return EX_OK;
}

