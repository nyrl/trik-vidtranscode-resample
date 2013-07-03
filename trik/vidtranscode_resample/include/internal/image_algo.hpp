#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include "include/internal/stdcpp.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {


class BaseImageAlgorithm
{
  public:
    enum AlgorithmType
    {
      AlgoResampleBicubic
    };

  protected:
    BaseImageAlgorithm() {}
};


template <BaseImageAlgorithm::AlgorithmType _ALG, typename _ImageIn, typename _ImageOut>
class ImageAlgorithm : public BaseImageAlgorithm,
                       private assert_inst<false> // non-functional generic template
{
};


} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include "include/internal/image_algo_cubic.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_
