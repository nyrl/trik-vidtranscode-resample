#include <sysexits.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>

#include "include/internal/image.hpp"
#include "include/internal/image_algo.hpp"


using namespace std;

typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, const uint8_t> ImgRGB888i;
typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, uint8_t>       ImgRGB888o;

typedef trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImgRGB888i, ImgRGB888o> AlgResample;


int main(int _argc, char* _argv[])
{
  if (_argc != 7)
  {
    cerr << "Usage: " << _argv[0] << " <in-file-rgb888> <height> <width> <out-file-rgb888> <height> <width>" << endl;
    exit(_argc == 1 ? EX_OK : EX_USAGE);
  }

  size_t srcHeight = atoi(_argv[2]);
  size_t srcWidth = atoi(_argv[3]);
  vector<uint8_t> srcBuffer;
  ifstream srcFs(_argv[1]);
  copy(istream_iterator<char>(srcFs), istream_iterator<char>(), back_inserter(srcBuffer));
  srcFs.close();

  size_t dstHeight = atoi(_argv[5]);
  size_t dstWidth = atoi(_argv[6]);
  vector<uint8_t> dstBuffer(dstHeight*dstWidth*3);
  ofstream dstFs(_argv[4]);

  ImgRGB888i srcImage(&srcBuffer.front(), srcBuffer.size(), srcHeight, srcWidth, srcWidth*3);
  ImgRGB888o dstImage(&dstBuffer.front(), dstBuffer.size(), dstHeight, dstWidth, dstWidth*3);

  AlgResample resampler;
  if (!resampler(srcImage, dstImage))
  {
    cerr << "Resampler failed" << endl;
    exit(EX_DATAERR);
  }

  copy(dstBuffer.begin(), dstBuffer.end(), ostream_iterator<char>(dstFs));
  dstFs.close();

  return EX_OK;
}

