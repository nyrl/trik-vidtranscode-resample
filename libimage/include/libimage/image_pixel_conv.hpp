#ifndef TRIK_LIBIMAGE_IMAGE_PIXEL_CONV_HPP_
#define TRIK_LIBIMAGE_IMAGE_PIXEL_CONV_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {




template <typename _PixelType1, typename _PixelType2>
class ImagePixelConvertion
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const _PixelType1& _p1, _PixelType2& _p2) const
    {
      float nr;
      float ng;
      float nb;

      if (!_p1.toNormalizedRGB(nr, ng, nb))
        return false;

#warning RGB to HSV convertion for rover
#if 1
//      nr = range(0.0f, nr, 1.0f);
//      ng = range(0.0f, ng, 1.0f);
//      nb = range(0.0f, nb, 1.0f);
      const float rgb_max = std::max(nr, std::max(ng, nb));
      const float rgb_min = std::min(nr, std::min(ng, nb));
      const float rgb_delta = rgb_max-rgb_min;
      const float hsv_v = rgb_max;
      const float hsv_s = rgb_delta / std::max(rgb_max, 1e-20f);
      float hsv_h;

#if 1 // partial hsv+detection
      if (nr == rgb_max && hsv_v > 0.4f && hsv_s > 0.5f)
      {
        hsv_h = (ng - nb) / std::max(rgb_delta, 1e-20f);
        if (hsv_h > -0.05f && hsv_h < 0.20f)
        {
          nr = 1.0f;
          ng = 1.0f;
          nb = 0.0f;
          s_TMP_pixelDetected = true;
        }
      }
#else // full hsv
      if (nr == rgb_max)
        hsv_h =        (ng - nb) / std::max(rgb_delta, 1e-20f);
      else if (ng == rgb_max)
        hsv_h = 2.0f + (nb - nr) / std::max(rgb_delta, 1e-20f);
      else
        hsv_h = 4.0f + (nr - ng) / std::max(rgb_delta, 1e-20f);
      hsv_h *= 60.0f;
      if (hsv_h < 0.0f)
        hsv_h += 360.0f;
      else if (hsv_h >= 360.0f)
        hsv_h -= 360.0f;
#endif
#endif


      if (!_p2.fromNormalizedRGB(nr, ng, nb))
        return false;

      return true;
    }
};

template <typename _PixelType>
class ImagePixelConvertion<_PixelType, _PixelType> // specialization for same type copy
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const _PixelType& _p1, _PixelType& _p2) const
    {
      _p2 = _p1;
      return true;
    }
};


template <typename _PixelSetType1, typename _PixelSetType2>
class ImagePixelSetConvertion
{
  public:
    ImagePixelSetConvertion()
     :m_convertion()
    {
    }

    bool operator()(const _PixelSetType1& _s1, _PixelSetType2& _s2) const
    {
      bool isOk = true;

      for (ImageDim pixIdx = 0; pixIdx < _s1.pixelsCount(); ++pixIdx)
        if (!m_convertion(_s1[pixIdx], _s2[pixIdx]))
          isOk = false;

      return isOk;
    }

  private:
    ImagePixelConvertion<typename _PixelSetType1::Pixel, typename _PixelSetType2::Pixel> m_convertion;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_PIXEL_CONV_HPP_
