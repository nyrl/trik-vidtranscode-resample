#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <map>
#include <utility>

#include <libimage/stdcpp.hpp>
#include <libimage/image_algo.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


template <ImageDim _windowBefore, ImageDim _windowAfter>
class BaseAlgoInterpolation1Dim
{
  public:
    static const bool s_isAlgorithmInterpolation1Dim = true;

  protected:
    static const ImageDim s_windowBefore = _windowBefore;
    static const ImageDim s_windowAfter  = _windowAfter;
    static const ImageDim s_windowSize   = _windowBefore+1+_windowAfter;

    template <typename _VertialInterpolation, typename _HorizontalInterpolation,
              typename _ImageIn, typename _ImageOut>
    friend class AlgoResampleVH;
};




/*
 * First interpolate one column from row set using single dimension vertical interpolation algorithm
 * Then interpolate one row of results using horizontal interpolation algorithm to get single output point
 * Then output point is converted from input color space to output
 */
template <typename _VerticalInterpolation, typename _HorizontalInterpolation,
          typename _ImageIn, typename _ImageOut>
class AlgoResampleVH : private assert_inst<(   _VerticalInterpolation::s_isAlgorithmInterpolation1Dim
                                            || _HorizontalInterpolation::s_isAlgorithmInterpolation1Dim)> // algorithm kind sanity check
{
  private:
    typedef ImageRowSet<_ImageIn::PT,  typename _ImageIn::UByteCV,  _VerticalInterpolation::s_windowSize> RowSetIn;
    typedef ImageRowSet<_ImageOut::PT, typename _ImageOut::UByteCV, 1>                                    RowSetOut;

    typedef ImagePixelSet<_ImageIn::PT,  _VerticalInterpolation::s_windowSize>   PixelSetInVertical;
    typedef ImagePixelSet<_ImageIn::PT,  _HorizontalInterpolation::s_windowSize> PixelSetInHorizontal;
    typedef ImagePixelSet<_ImageIn::PT,  1>                                      PixelSetInResult;
    typedef ImagePixelSet<_ImageOut::PT, 1>                                      PixelSetOutResult;

    typedef ImagePixelSetConvertion<PixelSetInResult, PixelSetOutResult> PixelSetIn2OutConvertion;

    typedef std::map<ImageDimFract, _VerticalInterpolation>   VerticalInterpolationCache;
    typedef std::map<ImageDimFract, _HorizontalInterpolation> HorizontalInterpolationCache;

  public:
    AlgoResampleVH()
    {
      // TODO build interpolation algorithms cache now?
    }


    bool operator()(const _ImageIn& _imageIn,
                    _ImageOut& _imageOut) const
    {
      RowSetIn    rowSetIn;
      RowSetOut   rowSetOut;

      PixelSetInHorizontal horizontalPixelSet;
      const PixelSetIn2OutConvertion resultPixelSetConvertion;

      VerticalInterpolationCache verticalInterpolationCache;
      HorizontalInterpolationCache horizontalInterpolationCache;

      const ImageDimFactor in2outCFactor = static_cast<ImageDimFactor>(_imageIn.width())  / static_cast<ImageDimFactor>(_imageOut.width() );
      const ImageDimFactor in2outRFactor = static_cast<ImageDimFactor>(_imageIn.height()) / static_cast<ImageDimFactor>(_imageOut.height());

      for (ImageDim rowIdxOut = 0; rowIdxOut < _imageOut.height(); ++rowIdxOut)
      {
        ImageDim rowIdxIn;
        ImageDimFract rowIdxInFract;
        if (!convertCoord(rowIdxOut, in2outRFactor, rowIdxIn, rowIdxInFract))
          return false;

        if (!prepareRowSet(_imageIn, rowSetIn, rowIdxIn, _imageOut, rowSetOut, rowIdxOut))
          return false;

        const _VerticalInterpolation& verticalInterpolation = getInterpolationCache(verticalInterpolationCache,
                                                                                    rowIdxInFract);

        ImageDim colIdxInLast;
        if (!initializeHorizontalPixelSet(rowSetIn, horizontalPixelSet, verticalInterpolation, colIdxInLast))
          return false;

        for (ImageDim colIdxOut = 0; colIdxOut < _imageOut.width(); ++colIdxOut)
        {
          ImageDim colIdxIn;
          ImageDimFract colIdxInFract;
          if (!convertCoord(colIdxOut, in2outCFactor, colIdxIn, colIdxInFract))
            return false;

          if (!updateHorizontalPixelSet(rowSetIn, horizontalPixelSet, verticalInterpolation, colIdxInLast, colIdxIn))
            return false;

          const _HorizontalInterpolation& horizontalInterpolation = getInterpolationCache(horizontalInterpolationCache,
                                                                                          colIdxInFract);

          if (!outputHorizontalPixelSet(horizontalPixelSet, rowSetOut, horizontalInterpolation, resultPixelSetConvertion))
            return false;
        }
      }

      return true;
    }

  private:
    static bool convertCoord(ImageDim _idx1, ImageDimFactor _factor, ImageDim& _idx2, ImageDimFract& _fract)
    {
      const ImageDimFactor idx2f = _idx1 * _factor;
      _idx2 = /*trunc*/idx2f;
      _fract = idx2f - _idx2;

      return true;
    }

    bool prepareRowSet(const _ImageIn& _imageIn,  RowSetIn&  _rowSetIn,  ImageDim _rowIdxIn,
                       _ImageOut&      _imageOut, RowSetOut& _rowSetOut, ImageDim _rowIdxOut) const
    {
      if (!_imageIn.template getRowSet<_VerticalInterpolation::s_windowBefore,
                                       _VerticalInterpolation::s_windowAfter>(_rowSetIn, _rowIdxIn))
        return false;

      if (!_imageOut.template getRowSet<0, 0>(_rowSetOut, _rowIdxOut))
        return false;

      return true;
    }

    bool readNextHorizontalPixel(RowSetIn& _rowSetIn, PixelSetInHorizontal& _pixelSetH,
                                 const _VerticalInterpolation& _interpolation) const
    {
      PixelSetInVertical pixelSetV;

      if (_rowSetIn.readPixelSet(pixelSetV))
        return _interpolation(pixelSetV, _pixelSetH);
      else
        return _pixelSetH.insertLastPixelCopy();
    }

    bool initializeHorizontalPixelSet(RowSetIn& _rowSetIn, PixelSetInHorizontal& _pixelSetH,
                                      const _VerticalInterpolation& _interpolation,
                                      ImageDim& _colIdxLast) const
    {
      bool isOk = true;
      PixelSetInVertical pixelSetV;

      isOk &= _rowSetIn.readPixelSet(pixelSetV);
      isOk &= _interpolation(pixelSetV, _pixelSetH);

      for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowBefore; ++idx)
        isOk &= _pixelSetH.insertLastPixelCopy();

      for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowAfter; ++idx)
        isOk &= readNextHorizontalPixel(_rowSetIn, _pixelSetH, _interpolation);

      _colIdxLast = 0;
      return isOk;
    }

    bool updateHorizontalPixelSet(RowSetIn& _rowSetIn, PixelSetInHorizontal& _pixelSetH,
                                  const _VerticalInterpolation& _interpolation,
                                  ImageDim& _colIdxLast, ImageDim _colIdxDesired) const
    {
      bool isOk = true;
      for (/*_colIdxLast*/; _colIdxLast < _colIdxDesired; ++_colIdxLast)
        isOk &= readNextHorizontalPixel(_rowSetIn, _pixelSetH, _interpolation);
      return true;
    }

    bool outputHorizontalPixelSet(const PixelSetInHorizontal& _pixSet,
                                  RowSetOut& _rowSetOut,
                                  const _HorizontalInterpolation& _interpolation,
                                  const PixelSetIn2OutConvertion& _convertion) const
    {
      PixelSetInResult  resIn;
      PixelSetOutResult resOut;

      if (!_interpolation(_pixSet, resIn))
        return false;

      if (!_convertion(resIn, resOut))
        return false;

      if (!_rowSetOut.writePixelSet(resOut))
        return false;

      return true;
    }

    template <typename _InterpolationCache>
    const typename _InterpolationCache::mapped_type& getInterpolationCache(_InterpolationCache& _cache,
                                                                           ImageDimFract _fract) const
    {
      const typename _InterpolationCache::const_iterator it(_cache.find(_fract));
      if (it != _cache.end())
        return it->second;

      return _cache.insert(std::make_pair(_fract, typename _InterpolationCache::mapped_type(_fract))).first->second;
    }

};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_
