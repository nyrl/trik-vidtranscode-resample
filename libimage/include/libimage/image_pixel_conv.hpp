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

      return _p1.toNormalizedRGB(nr, ng, nb)
          && _p2.fromNormalizedRGB(nr, ng, nb);
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

      for (size_t i = 0; i < _s1.pixelsCount(); ++i)
        isOk &= m_convertion(_s1[i], _s2[i]);

      return isOk;
    }

  private:
    ImagePixelConvertion<typename _PixelSetType1::Pixel, typename _PixelSetType2::Pixel> m_convertion;
};




} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_PIXEL_CONV_HPP_
