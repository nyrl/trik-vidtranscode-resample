#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {


class BaseImageAlgorithm
{
  public:
    enum AlgorithmType
    {
      AlgoTestFill,

      AlgoResampleBicubic,
      AlgoResampleBilinear
    };

  protected:
    BaseImageAlgorithm() {}
};


template <BaseImageAlgorithm::AlgorithmType _ALG, typename _ImageIn, typename _ImageOut>
class ImageAlgorithm : public BaseImageAlgorithm,
                       private assert_inst<false> // non-functional generic template
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include <libimage/image_algo_test_fill.hpp>
#include <libimage/image_algo_cubic.hpp>
#include <libimage/image_algo_linear.hpp>


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_HPP_
