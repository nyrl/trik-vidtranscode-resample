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
    AlgoInterpolationCubic()
     :m_weight0(),
      m_weight1(),
      m_weight2(),
      m_weight3()
    {
    }

    explicit AlgoInterpolationCubic(ImageDimFract _t)
    {
      assert(_t >= 0.0f && _t <= 1.0f);

      const ImageDimFract t0 = 1;
      const ImageDimFract t1 = _t;
      const ImageDimFract t2 = t1*t1;
      const ImageDimFract t3 = t2*t1;

      m_weight0 = 0.5f * ( 0.0f*t0 + -1.0f*t1 +  2.0f*t2 + -1.0f*t3 );
      m_weight1 = 0.5f * ( 2.0f*t0 +  0.0f*t1 + -5.0f*t2 +  3.0f*t3 );
      m_weight2 = 0.5f * ( 0.0f*t0 +  1.0f*t1 +  4.0f*t2 + -3.0f*t3 );
      m_weight3 = 0.5f * ( 0.0f*t0 +  0.0f*t1 + -1.0f*t2 +  1.0f*t3 );
    }

    template <typename PixelSetIn, typename PixelSetOut>
    bool operator()(const PixelSetIn& _pixelsIn,
                    PixelSetOut& _pixelsOut) const
    {
      typename PixelSetOut::Pixel result = _pixelsIn[0] * m_weight0;
      result += _pixelsIn[1] * m_weight1;
      result += _pixelsIn[2] * m_weight2;
      result += _pixelsIn[3] * m_weight3;

      _pixelsOut.insertNewPixel() = result;

      return true;
    }

  private:
    /*
     * Cached data is:
     *                             [  0,  2,  0,  0 ]
     * 0,5 * [1, _t, _t^2, _t^3] * [ -1,  0,  1,  0 ]
     *                             [  2, -5,  4, -1 ]
     *                             [ -1,  3, -3,  1 ]
     */
    float m_weight0;
    float m_weight1;
    float m_weight2;
    float m_weight3;
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
