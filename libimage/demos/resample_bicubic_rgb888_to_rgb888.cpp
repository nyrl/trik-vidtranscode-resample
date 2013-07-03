#include <sysexits.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>

#include <libimage/image.hpp>
#include <libimage/image_algo.hpp>


using namespace std;

typedef trik::libimage::Image<trik::libimage::BaseImagePixel::PixelRGB888, const uint8_t> ImgRGB888i;
typedef trik::libimage::Image<trik::libimage::BaseImagePixel::PixelRGB888, uint8_t>       ImgRGB888o;

typedef trik::libimage::ImageAlgorithm<trik::libimage::BaseImageAlgorithm::AlgoResampleBicubic, ImgRGB888i, ImgRGB888o> AlgResample;


int main(int _argc, char* _argv[])
{
  if (_argc != 7)
  {
    cerr << "Usage: " << _argv[0] << " <in-file-rgb888> <width> <height> <out-file-rgb888> <width> <height>" << endl;
    exit(EX_USAGE);
  }

  size_t srcWidth = atoi(_argv[2]);
  size_t srcHeight = atoi(_argv[3]);
  vector<uint8_t> srcBuffer;
  ifstream srcFs(_argv[1], ios_base::in | ios_base::binary);
  istreambuf_iterator<ifstream::char_type> srcFsIt(srcFs);

  copy(srcFsIt, istreambuf_iterator<ifstream::char_type>(), back_inserter(srcBuffer));
  srcFs.close();

  size_t dstWidth = atoi(_argv[5]);
  size_t dstHeight = atoi(_argv[6]);
  vector<uint8_t> dstBuffer(dstHeight*dstWidth*3);
  ofstream dstFs(_argv[4], ios_base::out | ios_base::binary | ios_base::trunc);
  ostreambuf_iterator<ofstream::char_type> dstFsIt(dstFs);

  cout << "Resampling " << _argv[1] << " " << srcWidth << "x" << srcHeight << " (" << srcBuffer.size() << ")"
              << " -> " << _argv[4] << " " << dstWidth << "x" << dstHeight << " (" << dstBuffer.size() << ")" << endl;

  ImgRGB888i srcImage(&srcBuffer.front(), srcBuffer.size(), srcWidth, srcHeight, srcWidth*3);
  ImgRGB888o dstImage(&dstBuffer.front(), dstBuffer.size(), dstWidth, dstHeight, dstWidth*3);

  AlgResample resampler;
  if (!resampler(srcImage, dstImage))
  {
    cerr << "Resampler failed" << endl;
    exit(EX_DATAERR);
  }

  copy(dstBuffer.begin(), dstBuffer.end(), dstFsIt);
  dstFs.close();

  return EX_OK;
}

