#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_TEST_FILL_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_TEST_FILL_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {




template <typename _ImageIn, typename _ImageOut>
class AlgoTestFill
{
  private:
    typedef ImageRowSet<_ImageOut::PT, typename _ImageOut::UByteCV, 1> RowSetOut;
    typedef ImagePixelSet<_ImageOut::PT, 1>                            PixelSetOut;

  public:
    AlgoTestFill()
    {
    }


    bool operator()(const _ImageIn& _imageIn,
                    _ImageOut& _imageOut) const
    {
      RowSetOut   rowSetOut;

      const ImageDimFactor imageOutHeightDiv = 1.0f / _imageOut.height();
      const ImageDimFactor imageOutWidthDiv =  1.0f / _imageOut.width();

      for (ImageDim rowIdxOut = 0; rowIdxOut < _imageOut.height(); ++rowIdxOut)
      {
        if (!_imageOut.template getRowSet<0, 0>(rowSetOut, rowIdxOut))
          return false;

        for (ImageDim colIdxOut = 0; colIdxOut < _imageOut.width(); ++colIdxOut)
        {
          float nr = rowIdxOut * imageOutHeightDiv;
          float ng = colIdxOut * imageOutWidthDiv;
          float nb = 0.5f;

          typename PixelSetOut::Pixel pt;
          pt.fromNormalizedRGB(nr, ng, nb);

          PixelSetOut pixelSetOut;
          pixelSetOut.insertNewPixel() = pt;

          rowSetOut.writePixelSet(pixelSetOut);
        }
      }

      return true;
    }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <typename _ImageIn, typename _ImageOut>
class ImageAlgorithm<BaseImageAlgorithm::AlgoTestFill, _ImageIn, _ImageOut>
 : public BaseImageAlgorithm,
   public internal::AlgoTestFill<_ImageIn, _ImageOut>
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_TEST_FILL_HPP_
