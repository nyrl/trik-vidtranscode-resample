#include <sysexits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>

#include "internal/image.hpp"
#include "internal/image_algo.hpp"

#include "demos/v4l2device.hpp"


using namespace std;


trik::demos::V4L2Input s_videoSrc(trik::demos::V4L2Config("/dev/video", 800, 600), "RGB888");



static bool parseConfig(int _argc, char* const _argv[])
{
  struct option long_opts[] = {
    { "src",			1,	NULL,	0 },
    { "src-width",		1,	NULL,	0 },
    { "src-height",		1,	NULL,	0 },
    { "src-format",		1,	NULL,	0 },
    { "dst",			1,	NULL,	0 },
    { "dst-width",		1,	NULL,	0 },
    { "dst-height",		1,	NULL,	0 },
    { "dst-line-length",	1,	NULL,	0 },
    { "dst-format",		1,	NULL,	0 },
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
              fprintf(stderr, "Cannot parse src argument\n");
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

#if 0
          case 4:
            if ((istringstream(optarg) >> s_videoDst.path()).fail())
            {
              fprintf(stderr, "Cannot parse dst argument\n");
              return false;
            }
            break;

          case 5:
            if ((istringstream(optarg) >> s_videoDst.width()).fail())
            {
              fprintf(stderr, "Cannot parse dst-width argument\n");
              return false;
            }
            break;

          case 6:
            if ((istringstream(optarg) >> s_videoDst.height()).fail())
            {
              fprintf(stderr, "Cannot parse dst-height argument\n");
              return false;
            }
            break;

          case 7:
            if ((istringstream(optarg) >> s_videoDst.lineLength()).fail())
            {
              fprintf(stderr, "Cannot parse dst-line-lenght argument\n");
              return false;
            }
            break;

          case 8:
            if ((istringstream(optarg) >> s_videoDst.format()).fail())
            {
              fprintf(stderr, "Cannot parse dst-format argument\n");
              return false;
            }
            break;
#endif

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




#if 0
typedef trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImgRGB888i, ImgRGB888o> AlgResample888;
typedef trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImgRGB888i, ImgRGB888o> AlgResample565;
#endif


int main(int _argc, char* const _argv[])
{
  int res;

  if (!parseConfig(_argc, _argv))
  {
    fprintf(stderr, "Usage:\n"
                    "  %s [opts]\n",
            _argv[0]);
    exit(EX_USAGE);
  }

  v4l2_log_file = stderr;

  if (!s_videoSrc.open())
    exit(EX_NOINPUT);

  if (!s_videoSrc.start())
    exit(EX_NOINPUT);

  const int videoSrcFd = s_videoSrc.v4l2fd();

  for (unsigned retry = 0; retry < 10; ++retry)
  {
    int fdsMax;
    fd_set fdsIn;
    FD_ZERO(&fdsIn);
    FD_SET(s_videoSrc.v4l2fd(), &fdsIn);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if ((res = select(s_videoSrc.v4l2fd()+1, &fdsIn, NULL, NULL, &timeout)) == -1)
    {
      fprintf(stderr, "select() failed: %d\n", errno);
      exit(EX_OSERR);
    }

    trik::demos::V4L2BufferDescription buf;
    if (!s_videoSrc.getFrame(buf))
      exit(EX_SOFTWARE);
    printf("got frame: %p (%zu) %"PRIu32"x%"PRIu32"\n", buf.ptr(), buf.size(), buf.width(), buf.height());
    if (!s_videoSrc.ungetFrame(buf))
      exit(EX_SOFTWARE);
  }

  // We got input frame, time to generate output

  s_videoSrc.stop();
  s_videoSrc.close();

  return EX_OK;
}

