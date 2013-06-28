#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <cassert>

#include "include/internal/image.hpp"
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

    template <typename ApproximationC, typename ApproximationR,
              typename ImageIn, typename ImageOut>
    friend class AlgoResampleCR;
};




template <typename ApproximationC, typename ApproximationR,
          typename ImageIn, typename ImageOut>
class AlgoResampleCR // first approximate one column from row set, then column set to pixel
{
  private:
    typedef ImageRowSet<ImageIn::s_pixelType,  typename ImageIn::UByteCVType,  ApproximationC::s_windowSize> RowSetIn;
    typedef ImageRowSet<ImageOut::s_pixelType, typename ImageOut::UByteCVType, 1>                            RowSetOut;

    typedef ImagePixelSet<ImageIn::s_pixelType,  ApproximationR::s_windowSize> PixelSetIn;
    typedef ImagePixelSet<ImageOut::s_pixelType, 1>                            PixelSetOut;

  public:
    AlgoResampleCR()
    {
      // TODO build row/columns approximation algorithms cache?
    }


    bool operator()(const ImageIn& _imageIn,
                    ImageOut& _imageOut)
    {
      RowSetIn rowSetIn; // row scanner of s_windowSize high
      PixelSetIn pixSet; // single row history (column-processed) of s_windowSize lenght
      RowSetOut rowSetOut;

      const float in2outRFactor = static_cast<float>(_imageIn.height()) / static_cast<float>(_imageOut.height());
      const float in2outCFactor = static_cast<float>(_imageIn.width())  / static_cast<float>(_imageOut.width() );

      for (size_t rowIdxOut = 0; rowIdxOut < _imageOut.height(); ++rowIdxOut)
      {
        size_t rowIdxIn;
        float rowIdxInFract;
        if (!convertCoord(rowIdxOut, in2outRFactor, rowIdxIn, rowIdxInFract))
          return false;

        if (!prepareRowSet(rowSetIn, rowIdxIn, rowSetOut, rowIdxOut))
          return false;

        // TODO cache?
        const ApproximationC& approxC(rowIdxInFract);

        size_t colIdxInLast;
        if (!initializePixelSet(rowSetIn, pixSet, approxC, colIdxInLast))
          return false;

        for (size_t colIdxOut = 0; colIdxOut < _imageOut.width(); ++colIdxOut)
        {
          size_t colIdxIn;
          float colIdxInFract;
          if (!convertCoord(colIdxOut, in2outCFactor, colIdxIn, colIdxInFract))
            return false;

          if (!updatePixelSet(rowSetIn, pixSet, approxC, colIdxInLast, colIdxIn))
            return false;

          // TODO cache?
          const ApproximationR& approxR(colIdxInFract);

          if (!outputPixelSet(pixSet, rowSetOut, approxR))
            return false;
        }
      }

      return true;
    }

  private:
    static bool convertCoord(size_t _idx1, float _factor, size_t& _idx2, float& _fract)
    {
      const float idx2f = _idx1 * _factor;
      _idx2 = /*trunc*/idx2f;
      _fract = idx2f - _idx2;
      return true;
    }

    static bool prepareRowSet(const ImageIn& _imageIn,  RowSetIn&  _rowSetIn,  size_t _rowIdxIn,
                              ImageOut&      _imageOut, RowSetOut& _rowSetOut, size_t _rowIdxOut)
    {
      if (!_imageIn.getRowSet<ApproximationC::s_windowBefore,
                              ApproximationC::s_windowAfter>(_rowSetIn, _rowIdxIn))
          return false;

      if (!_imageOut.getRowSet<0, 0>(_rowSetOut, _rowIdxOut))
        return false;

      return true;
    }

    static bool outputPixel(const PixelSetIn& _pixSet,
                            RowSetOut& _rowSetOut,
                            const ApproximationR& _approxR)
    {
      PixelSetIn  resIn;
      PixelSetOut resOut;

      if (!_approxR(_pixSet, resIn))
        return false;

      // TODO convert resIn to resOut
      resIn = resOut;

      if (!_rowSetOut.writePixelSet(resOut))
        return false;

      return true;
    }

};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */


} /* **** **** **** **** **** * namespace trik_image * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_ALGO_CR_HPP_
