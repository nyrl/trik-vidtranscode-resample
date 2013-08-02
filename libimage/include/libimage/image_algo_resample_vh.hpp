#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_RESAMPLE_VH_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <cstdio>
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
    static const ImageDim s_outputRows = 1;
    static const ImageDim s_outputCols = 1;

    typedef ImageRowSet<_ImageIn::s_PT,  typename _ImageIn::UByteCV,  _VerticalInterpolation::s_windowSize> RowSetIn;
    typedef ImageRowSet<_ImageOut::s_PT, typename _ImageOut::UByteCV, s_outputRows>                         RowSetOut;

    typedef ImagePixelSet<_ImageIn::s_PT,  _VerticalInterpolation::s_windowSize, false>  PixelSetInVertical;
    typedef ImagePixelSet<_ImageIn::s_PT,  _HorizontalInterpolation::s_windowSize, true> PixelSetInHorizontal;
    typedef ImagePixelSet<_ImageIn::s_PT,  s_outputCols, true>                           PixelSetInResult;
    typedef ImagePixelSet<_ImageOut::s_PT, s_outputCols, false>                          PixelSetOutResult;

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
          m_columnInCurrent(),
          m_columnInNextAccessed(),
          m_pixelSetInV_tmp(),
          m_pixelSetInR_tmp(),
          m_pixelSetOutR_tmp()
        {
        }

        ~ExecutionInstance()
        {
        }

        bool prepareRowSets(const _ImageIn& _imageIn,  ImageDim _rowIdxIn,
                            _ImageOut&      _imageOut, ImageDim _rowIdxOut)
        {
          if (!_imageIn.template getRowSet<_VerticalInterpolation::s_windowBefore,
                                           _VerticalInterpolation::s_windowAfter>(m_rowSetIn, _rowIdxIn))
            return false;

          if (!_imageOut.template getRowSet<0, 0>(m_rowSetOut, _rowIdxOut))
            return false;

          return true;
        }

        bool initializeHorizontalPixelSet(const _VerticalInterpolation& _interpolation, ImageDim _width)
        {
          m_columnInNextAccessed = 0;

          if (!readNextHorizontalPixel(_interpolation, _width, false))
            return false;

          for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowBefore; ++idx)
            if (!m_pixelSetInH.insertLastPixelCopy())
              return false;

          for (ImageDim idx = 0; idx < _VerticalInterpolation::s_windowAfter; ++idx)
            if (!readNextHorizontalPixel(_interpolation, _width, true))
              return false;

          m_columnInCurrent = 0;

          return true;
        }

        bool updateHorizontalPixelSet(const _VerticalInterpolation& _interpolation,
                                      ImageDim _columnInDesired, ImageDim _width)
        {
          for (/*m_columnInCurrent*/; m_columnInCurrent < _columnInDesired; ++m_columnInCurrent)
            if (!readNextHorizontalPixel(_interpolation, _width, true))
              return false;

          return true;
        }

        bool outputHorizontalPixelSet(const _HorizontalInterpolation& _interpolation,
                                      const PixelSetIn2OutConvertion& _convertion,
                                      ImageDim _columnOut)
        {
          if (!_interpolation(m_pixelSetInH, m_pixelSetInR_tmp))
            return false;

          if (!_convertion(m_pixelSetInR_tmp, m_pixelSetOutR_tmp))
            return false;

          if (!m_rowSetOut.writePixelSet(m_pixelSetOutR_tmp, _columnOut))
            return false;

          return true;
        }

      private:
        RowSetIn  m_rowSetIn;
        RowSetOut m_rowSetOut;

        PixelSetInHorizontal m_pixelSetInH;
        ImageDim m_columnInCurrent;
        ImageDim m_columnInNextAccessed;

        PixelSetInVertical   m_pixelSetInV_tmp;
        PixelSetInResult     m_pixelSetInR_tmp;
        PixelSetOutResult    m_pixelSetOutR_tmp;


        bool readNextHorizontalPixel(const _VerticalInterpolation& _interpolation, ImageDim _width, bool _allowCopyLast)
        {
          if (m_columnInNextAccessed >= _width)
          {
            if (_allowCopyLast)
              return m_pixelSetInH.insertLastPixelCopy();
            else
              return false;
          }
          else
          {
            if (!m_rowSetIn.readPixelSet(m_pixelSetInV_tmp, m_columnInNextAccessed++))
              return false;

            return _interpolation(m_pixelSetInV_tmp, m_pixelSetInH);
          }
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

      const ImageDim imageOutWidth  = _imageOut.width();
      const ImageDim imageOutHeight = _imageOut.height();
      const ImageDim imageInWidth   = _imageIn.width();
      const ImageDim imageInHeight  = _imageIn.height();

#ifdef __TI_COMPILER_VERSION__
      if (imageOutWidth % 8 != 0 && imageOutWidth < 8*4)
        return false;
#endif

      for (ImageDim rowIdxOut = 0; rowIdxOut < imageOutHeight; ++rowIdxOut)
      {
        assert(rowIdxOut >= 0 && rowIdxOut < m_verticalInterpolationCache.size());
        const ImageDim rowIdxIn = m_verticalOutput2InputCache[rowIdxOut];
        assert(rowIdxIn >= 0 && rowIdxIn < imageInHeight);

        if (!execution.prepareRowSets(_imageIn, rowIdxIn, _imageOut, rowIdxOut))
          return false;

        const _VerticalInterpolation& verticalInterpolation = m_verticalInterpolationCache[rowIdxOut];

        if (!execution.initializeHorizontalPixelSet(verticalInterpolation, imageInWidth))
          return false;

#ifdef __TI_COMPILER_VERSION__
#pragma MUST_ITERATE(8*4, ,8)
#endif
        for (ImageDim colIdxOut = 0; colIdxOut < imageOutWidth; ++colIdxOut)
        {
          assert(colIdxOut >= 0 && colIdxOut < m_horizontalInterpolationCache.size());
          const ImageDim colIdxIn = m_horizontalOutput2InputCache[colIdxOut];
          assert(colIdxIn >= 0 && colIdxIn < imageInWidth);

          if (!execution.updateHorizontalPixelSet(verticalInterpolation, colIdxIn, imageInWidth))
            return false;

          const _HorizontalInterpolation& horizontalInterpolation = m_horizontalInterpolationCache[colIdxOut];

          if (!execution.outputHorizontalPixelSet(horizontalInterpolation, m_pixelSetConvertion, colIdxOut))
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
