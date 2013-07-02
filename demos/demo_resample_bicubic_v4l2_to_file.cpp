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

#include "internal/image.hpp"
#include "internal/image_algo.hpp"

#include "demos/v4l2device.hpp"
#include "demos/filedevice.hpp"


using namespace std;


static trik::demos::V4L2Input  s_videoSrc(trik::demos::V4L2Config("/dev/video", 800, 600), "RGB888");
static trik::demos::FileOutput s_videoDst(trik::demos::FileConfig("video.out", 320, 240), "RGB888");
static size_t s_repeatCount = 1;



static bool parseConfig(int _argc, char* const _argv[])
{
  struct option long_opts[] = {
    { "src-path",		1,	NULL,	0 },
    { "src-width",		1,	NULL,	0 },
    { "src-height",		1,	NULL,	0 },
    { "src-format",		1,	NULL,	0 },
    { "dst-path",		1,	NULL,	0 },
    { "dst-width",		1,	NULL,	0 },
    { "dst-height",		1,	NULL,	0 },
    { "dst-format",		1,	NULL,	0 },
    { "repeat",			1,	NULL,	0 },
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


static bool resample(const trik::demos::V4L2Input::Description&  _srcDesc,
                     const trik::demos::V4L2Input::Frame&        _srcFrame,
                     const trik::demos::FileOutput::Description& _dstDesc,
                     trik::demos::FileOutput::Frame&             _dstFrame)
{
  if (_srcDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24 && _dstDesc.format().rawFormat() == V4L2_PIX_FMT_RGB565)
  {
    typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, const uint8_t> ImageSrc;
    typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB565, uint8_t>       ImageDst;
    trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImageSrc, ImageDst> algorithm;

    ImageSrc imageSrc(_srcFrame.ptr(), _srcFrame.size(),
                      _srcDesc.width(), _srcDesc.height(),
                      _srcDesc.bytesPerLine());
    ImageDst imageDst(_dstFrame.ptr(), _dstFrame.size(),
                      _dstDesc.width(), _dstDesc.height(),
                      /*_dstDesc.bytesPerLine()*/2*_dstDesc.width());
#warning TODO dirty hack above

    if (!algorithm(imageSrc, imageDst))
    {
      fprintf(stderr, "algorithm(RGB888->RGB565) failed\n");
      return false;
    }

#warning TODO update dst image usage
  }
  else if (_srcDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24 && _dstDesc.format().rawFormat() == V4L2_PIX_FMT_RGB24)
  {
    typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, const uint8_t> ImageSrc;
    typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, uint8_t>       ImageDst;
    trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImageSrc, ImageDst> algorithm;

    ImageSrc imageSrc(_srcFrame.ptr(), _srcFrame.size(),
                      _srcDesc.width(), _srcDesc.height(),
                      _srcDesc.bytesPerLine());
    ImageDst imageDst(_dstFrame.ptr(), _dstFrame.size(),
                      _dstDesc.width(), _dstDesc.height(),
                      /*_dstDesc.bytesPerLine()*/3*_dstDesc.width());
#warning TODO dirty hack above

    if (!algorithm(imageSrc, imageDst))
    {
      fprintf(stderr, "algorithm(RGB888->RGB888) failed\n");
      return false;
    }

#warning TODO update dst image usage
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

    trik::demos::V4L2Input::Frame      srcFrame;
    trik::demos::V4L2Input::FrameIndex srcFrameIndex;
    if (!s_videoSrc.getFrame(srcFrame, srcFrameIndex))
      exit(EX_SOFTWARE);

    trik::demos::FileOutput::Frame     dstFrame;
    if (!s_videoDst.getFrame(dstFrame))
      exit(EX_SOFTWARE);

    if (!resample(s_videoSrc.description(), srcFrame, s_videoDst.description(), dstFrame))
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

