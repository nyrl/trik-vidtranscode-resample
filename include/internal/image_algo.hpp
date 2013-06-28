#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>


/* **** **** **** **** **** */ namespace trik_image /* **** **** **** **** **** */ {


class BaseImageAlgorithm
{
  public:
    enum AlgorithmType
    {
      AlgoResampleBicubic
    };

  protected:
    BaseImageAlgorithm() {}
    /*virtual*/ ~BaseImageAlgorithm() {}

  private:
    BaseImageAlgorithm(const BaseImageAlgorithm&);
    BaseImageAlgorithm& operator=(const BaseImageAlgorithm&);
};


template <BaseImageAlgorithm::AlgorithmType ALG, typename ImageIn, typename ImageOut>
class ImageAlgorithm : public BaseImageAlgorithm // non-functional generic template
{
};


} /* **** **** **** **** **** * namespace trik_image * **** **** **** **** **** */


#include "include/internal/image_algo_cubic.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_HPP_
