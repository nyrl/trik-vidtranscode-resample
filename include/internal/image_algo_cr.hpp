#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>

#include "include/internal/image_algo.hpp"


/* **** **** **** **** **** */ namespace trik_image /* **** **** **** **** **** */ {


/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


template <size_t _windowBefore, size_t _windowAfter>
class BaseAlgoApproximationCR
{
  protected:
    static const size_t s_windowBefore = _windowBefore;
    static const size_t s_windowAfter  = _windowAfter;
    static const size_t s_windowSize   = _windowBefore+1+_windowAfter;
};




template <typename ApproximationC, typename ApproximationR,
          typename ImageIn, typename ImageOut>
class AlgoResamplerCR // first approximate one column from row set, then column set to pixel
{
  public:
    AlgoResamplerCR() {}


    bool operator()(const ImageIn& _imageIn,
                    ImageOut& _imageOut)
    {
#if 0

      // TODO
#endif

      return false;
    }

};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */


} /* **** **** **** **** **** * namespace trik_image * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_
