#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_LINEAR_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_LINEAR_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>
#include <libimage/image_algo_resample_vh.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class AlgoInterpolationLinear : public BaseAlgoInterpolation1Dim<0, 1>
{
  public:
    AlgoInterpolationLinear(ImageDimFract _t)
    {
      assert(_t >= 0 && _t <= 1.0);

      m_t0 = 1.0-_t;
      m_t1 = _t;
    }

    template <typename PixelSetIn, typename PixelSetOut>
    bool operator()(const PixelSetIn& _pixelsIn,
                    PixelSetOut& _pixelsOut) const
    {
      typename PixelSetOut::Pixel result;

      result += _pixelsIn[0] * m_t0;
      result += _pixelsIn[1] * m_t1;

      _pixelsOut.insertNewPixel() = result;

      return true;
    }

  private:
    float m_t0;
    float m_t1;
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <typename _ImageIn, typename _ImageOut>
class ImageAlgorithm<BaseImageAlgorithm::AlgoResampleBilinear, _ImageIn, _ImageOut>
 : public BaseImageAlgorithm,
   public internal::AlgoResampleVH<internal::AlgoInterpolationLinear, internal::AlgoInterpolationLinear,
                                   _ImageIn, _ImageOut>
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_
