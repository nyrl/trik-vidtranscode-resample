#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>
#include <libimage/image_defs.hpp>
#include <libimage/image_algo_resample_vh.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class AlgoInterpolationCubic : public BaseAlgoInterpolation1Dim<1, 2>
{
  public:
    typedef int_fast8_t InterpolationMatrixDim;

    AlgoInterpolationCubic(ImageDimFract _t = 0.0f)
    {
      assert(_t >= 0.0f && _t <= 1.0f);

      const ImageDimFract t0 = 1;
      const ImageDimFract t1 = _t;
      const ImageDimFract t2 = t1*t1;
      const ImageDimFract t3 = t2*t1;

      m_weight[0] = 0.5f * ( 0.0f*t0 + -1.0f*t1 +  2.0f*t2 + -1.0f*t3 );
      m_weight[1] = 0.5f * ( 2.0f*t0 +  0.0f*t1 + -5.0f*t2 +  3.0f*t3 );
      m_weight[2] = 0.5f * ( 0.0f*t0 +  1.0f*t1 +  4.0f*t2 + -3.0f*t3 );
      m_weight[3] = 0.5f * ( 0.0f*t0 +  0.0f*t1 + -1.0f*t2 +  1.0f*t3 );
    }

    template <typename PixelSetIn, typename PixelSetOut>
    bool operator()(const PixelSetIn& _pixelsIn,
                    PixelSetOut& _pixelsOut) const
    {
      typename PixelSetOut::Pixel result;

      for (InterpolationMatrixDim idx = 0; idx < s_weightDimension; ++idx)
        result += _pixelsIn[idx] * m_weight[idx];

      _pixelsOut.insertNewPixel() = result;

      return true;
    }

  private:
    static const InterpolationMatrixDim s_weightDimension = 4;

    /*
     * Cached data is:
     *                             [  0,  2,  0,  0 ]
     * 0,5 * [1, _t, _t^2, _t^3] * [ -1,  0,  1,  0 ]
     *                             [  2, -5,  4, -1 ]
     *                             [ -1,  3, -3,  1 ]
     */
    float m_weight[s_weightDimension];
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <typename _ImageIn, typename _ImageOut>
class ImageAlgorithm<BaseImageAlgorithm::AlgoResampleBicubic, _ImageIn, _ImageOut>
 : public BaseImageAlgorithm,
   public internal::AlgoResampleVH<internal::AlgoInterpolationCubic, internal::AlgoInterpolationCubic,
                                   _ImageIn, _ImageOut>
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_
