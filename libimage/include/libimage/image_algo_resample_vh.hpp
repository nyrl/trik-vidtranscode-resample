#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <vector>

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
                                            && _HorizontalInterpolation::s_isAlgorithmInterpolation1Dim)> // algorithm kind sanity check
{
  private:
    typedef ImageRowSet<_ImageIn::PT,  typename _ImageIn::UByteCV,  _VerticalInterpolation::s_windowSize> RowSetIn;
    typedef ImageRowSet<_ImageOut::PT, typename _ImageOut::UByteCV, 1>                                    RowSetOut;

    typedef ImagePixelSet<_ImageIn::PT,  _VerticalInterpolation::s_windowSize>   PixelSetInVertical;
    typedef ImagePixelSet<_ImageIn::PT,  _HorizontalInterpolation::s_windowSize> PixelSetInHorizontal;
    typedef ImagePixelSet<_ImageIn::PT,  1>                                      PixelSetInResult;
    typedef ImagePixelSet<_ImageOut::PT, 1>                                      PixelSetOutResult;

    typedef ImagePixelSetConvertion<PixelSetInResult, PixelSetOutResult> PixelSetIn2OutConvertion;

    typedef std::vector<_VerticalInterpolation>   VerticalInterpolationCache;
    typedef std::vector<_HorizontalInterpolation> HorizontalInterpolationCache;
    typedef std::vector<ImageDim>                 Output2InputCache;

    class ExecutionInstance
    {
      public:
        ExecutionInstance()
         :m_rowSetIn(),
          m_rowSetOut(),
          m_pixelSetInH(),
          m_columnIn(),
          m_pixelSetInV_tmp(),
          m_pixelSetInR_tmp(),
          m_pixelSetOutR_tmp()
        {
        }

        bool prepareRowSets(const _ImageIn& _imageIn,  ImageDim _rowIdxIn,
                            _ImageOut&      _imageOut, ImageDim _rowIdxOut)
        {
          bool isOk;

          isOk  = _imageIn.template getRowSet<_VerticalInterpolation::s_windowBefore,
                                              _VerticalInterpolation::s_windowAfter>(m_rowSetIn, _rowIdxIn);
          isOk &= _imageOut.template getRowSet<0, 0>(m_rowSetOut, _rowIdxOut);

          return isOk;
        }

        bool initializeHorizontalPixelSet(const _VerticalInterpolation& _interpolation)
        {
          if (!m_rowSetIn.readPixelSet(m_pixelSetInV_tmp))
            return false;

          if (!_interpolation(m_pixelSetInV_tmp, m_pixelSetInH))
            return false;

          for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowBefore; ++idx)
            if (!m_pixelSetInH.insertLastPixelCopy())
              return false;

          for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowAfter; ++idx)
            if (!readNextHorizontalPixel(_interpolation))
              return false;

          m_columnIn = 0;

          return true;
        }

        bool updateHorizontalPixelSet(const _VerticalInterpolation& _interpolation,
                                      ImageDim _columnInDesired)
        {
          for (/*m_columnIn*/; m_columnIn < _columnInDesired; ++m_columnIn)
            if (!readNextHorizontalPixel(_interpolation))
              return false;

          return true;
        }

        bool outputHorizontalPixelSet(const _HorizontalInterpolation& _interpolation,
                                      const PixelSetIn2OutConvertion& _convertion)
        {
          if (!_interpolation(m_pixelSetInH, m_pixelSetInR_tmp))
            return false;

          if (!_convertion(m_pixelSetInR_tmp, m_pixelSetOutR_tmp))
            return false;

          if (!m_rowSetOut.writePixelSet(m_pixelSetOutR_tmp))
            return false;

          return true;
        }

      private:
        RowSetIn  m_rowSetIn;
        RowSetOut m_rowSetOut;

        PixelSetInHorizontal m_pixelSetInH;
        ImageDim m_columnIn;

        PixelSetInVertical   m_pixelSetInV_tmp;
        PixelSetInResult     m_pixelSetInR_tmp;
        PixelSetOutResult    m_pixelSetOutR_tmp;

        bool readNextHorizontalPixel(const _VerticalInterpolation& _interpolation)
        {
          if (m_rowSetIn.readPixelSet(m_pixelSetInV_tmp))
            return _interpolation(m_pixelSetInV_tmp, m_pixelSetInH);
          else
            return m_pixelSetInH.insertLastPixelCopy();
        }
    };


  public:
    AlgoResampleVH()
    {
#warning Build cache now; requires image dimensions to be known at this moment
    }


    bool operator()(const _ImageIn& _imageIn,
                    _ImageOut& _imageOut) const
    {
      if (!_imageIn || !_imageOut)
        return false;

      ExecutionInstance execution;


#warning Temporary - build caches here instead of constructor
      if (!const_cast<AlgoResampleVH*>(this)->buildCaches(_imageIn.width(),  _imageIn.height(),
                                                          _imageOut.width(), _imageOut.height()))
        return false;

      assert(m_verticalInterpolationCache.size()   == m_verticalOutput2InputCache.size());
      assert(m_horizontalInterpolationCache.size() == m_horizontalOutput2InputCache.size());

      for (ImageDim rowIdxOut = 0; rowIdxOut < _imageOut.height(); ++rowIdxOut)
      {
        assert(rowIdxOut >= 0 && rowIdxOut < m_verticalInterpolationCache.size());
        const ImageDim rowIdxIn = m_verticalOutput2InputCache[rowIdxOut];
        assert(rowIdxIn >= 0 && rowIdxIn < _imageIn.height());

        if (!execution.prepareRowSets(_imageIn, rowIdxIn, _imageOut, rowIdxOut))
          return false;

        const _VerticalInterpolation& verticalInterpolation = m_verticalInterpolationCache[rowIdxOut];

        if (!execution.initializeHorizontalPixelSet(verticalInterpolation))
          return false;

        for (ImageDim colIdxOut = 0; colIdxOut < _imageOut.width(); ++colIdxOut)
        {
          assert(colIdxOut >= 0 && colIdxOut < m_horizontalInterpolationCache.size());
          const ImageDim colIdxIn = m_horizontalOutput2InputCache[colIdxOut];
          assert(colIdxIn >= 0 && colIdxIn < _imageIn.width());

          if (!execution.updateHorizontalPixelSet(verticalInterpolation, colIdxIn))
            return false;

          const _HorizontalInterpolation& horizontalInterpolation = m_horizontalInterpolationCache[colIdxOut];

          if (!execution.outputHorizontalPixelSet(horizontalInterpolation, m_pixelSetConvertion))
            return false;
        }
      }

      return true;
    }

  private:
    const PixelSetIn2OutConvertion m_pixelSetConvertion;
    VerticalInterpolationCache     m_verticalInterpolationCache;
    Output2InputCache              m_verticalOutput2InputCache;
    HorizontalInterpolationCache   m_horizontalInterpolationCache;
    Output2InputCache              m_horizontalOutput2InputCache;

    template <typename _InterpolationCache>
    static bool buildCache(_InterpolationCache& _interpolationCache,
                           Output2InputCache& _output2inputCache,
                           ImageDim _inDim, ImageDim _outDim)
    {
      _interpolationCache.resize(_outDim);
      _output2inputCache.resize(_outDim);

      ImageDimFactor inIdxFull = 0;
      const ImageDimFactor in2outFactor = static_cast<ImageDimFactor>(_inDim) / static_cast<ImageDimFactor>(_outDim);

      for (ImageDim outIdx = 0; outIdx < _outDim; ++outIdx)
      {
        inIdxFull += in2outFactor; // same as inIdxFull = outIdx*in2outFactor

        const ImageDim inIdx = /*trunc*/inIdxFull;
        const ImageDimFract inIdxFract = inIdxFull - inIdx;

        _output2inputCache[outIdx]  = inIdx;
        _interpolationCache[outIdx] = typename _InterpolationCache::value_type(inIdxFract);
      }

      return true;
    }

    bool buildCaches(ImageDim _inWidth,  ImageDim _inHeight,
                     ImageDim _outWidth, ImageDim _outHeight)
    {
      if (!buildCache(m_horizontalInterpolationCache,
                      m_horizontalOutput2InputCache,
                      _inWidth, _outWidth))
        return false;

      if (!buildCache(m_verticalInterpolationCache,
                      m_verticalOutput2InputCache,
                      _inHeight, _outHeight))
        return false;

      return true;
    }

};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_
