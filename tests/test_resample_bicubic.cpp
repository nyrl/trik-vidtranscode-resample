#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "include/internal/image.hpp"
#include "include/internal/image_algo.hpp"

using namespace std;

typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB888, const uint8_t> ImgRGB888;
typedef trik::image::Image<trik::image::BaseImagePixel::PixelRGB565, uint8_t> ImgRGB565;

typedef trik::image::ImageAlgorithm<trik::image::BaseImageAlgorithm::AlgoResampleBicubic, ImgRGB888, ImgRGB565> AlgResample_RGB888_RGB565;


void test1()
{
  vector<uint8_t> bufSrc(1*1*3);
  vector<uint8_t> bufDst(1*1*2);

  ImgRGB888 imgSrc(&bufSrc.front(), bufSrc.size(), 1, 1, 3);
  ImgRGB565 imgDst(&bufDst.front(), bufDst.size(), 1, 1, 2);

  AlgResample_RGB888_RGB565 resampler;

  cout << __func__ << " ";

  for (unsigned retry = 0; retry < 1000; ++retry)
  {
    if (!resampler(imgSrc, imgDst))
    {
      cerr << __func__ << " failed, iteration " << retry << endl;
      exit(1);
    }

    cout << ".";
  }

  cout << " done" << endl;
}

int main()
{
  test1();

  return 0;
}

