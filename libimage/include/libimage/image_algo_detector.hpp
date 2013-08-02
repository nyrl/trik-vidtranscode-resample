#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_DETECTOR_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_DETECTOR_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <cstdio>
#include <vector>
#include <utility>

#include <libimage/stdcpp.hpp>
#include <libimage/image_algo.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


template <typename _ImageIn, typename _ImageOut>
class AlgoDetector
{
  private:
    typedef ImageRowSet<_ImageIn::s_PT,  typename _ImageIn::UByteCV,  1> RowSetIn;
    typedef ImageRowSet<_ImageOut::s_PT, typename _ImageOut::UByteCV, 1> RowSetOut;

    typedef ImagePixelSet<_ImageIn::s_PT,  1, false> PixelSetIn;
    typedef ImagePixelSet<_ImageOut::s_PT, 1, false> PixelSetOut;

    typedef std::vector<ImageDim> Input2OutputCache;


    class ExecutionInstance
    {
      public:
        ExecutionInstance(ImageDim _width, ImageDim _height,
                          const std::pair<float, float>& _hue,
                          const std::pair<float, float>& _sat,
                          const std::pair<float, float>& _val)
         :m_rowSetIn(),
          m_rowSetOut(),
          m_pixelSetIn(),
          m_pixelSetOut(),
          m_columnOutCurrent(),
          m_width(_width),
          m_height(_height),
          m_detectHue(_hue),
          m_detectSat(_sat),
          m_detectVal(_val),
          m_detectCenterX(),
          m_detectCenterY(),
          m_detectCount()
        {
        }

        ~ExecutionInstance()
        {
#ifdef HAVE_DSP_INFO_OUT_BUFFER
          if (s_dspInfoOutBuffer)
          {
            long totalPoints = m_width * m_height;
            long mass = totalPoints > 0 ? (m_detectCount*10000)/totalPoints : 0;
            long centerX = m_width  > 0 ? m_detectCenterX/m_width  - m_width/2  : 0;
            long centerY = m_height > 0 ? m_detectCenterY/m_height - m_height/2 : 0;

            sprintf(s_dspInfoOutBuffer, "%li x %li  %li", centerX, centerY, mass);
          }
#endif
        }


        bool prepareRowSets(const _ImageIn& _imageIn,  ImageDim _rowIdxIn,
                            _ImageOut&      _imageOut, ImageDim _rowIdxOut)
        {
          if (!_imageIn.template getRowSet<0,0>(m_rowSetIn, _rowIdxIn))
            return false;

          if (!_imageOut.template getRowSet<0,0>(m_rowSetOut, _rowIdxOut))
            return false;

          m_columnOutCurrent = 0;

          return true;
        }

        bool processPixelSet(ImageDim _colIdxIn, ImageDim _rowIdxIn, ImageDim _colIdxOut)
        {
          if (!m_rowSetIn.readPixelSet(m_pixelSetIn, _colIdxIn))
            return false;


          assert(m_pixelSetIn.pixelsCount() == 1 && m_pixelSetOut.pixelsCount() == 1);

          float nr;
          float ng;
          float nb;

          if (!m_pixelSetIn[0].toNormalizedRGB(nr, ng, nb))
            return false;

          nr = range(0.0f, nr, 1.0f);
          ng = range(0.0f, ng, 1.0f);
          nb = range(0.0f, nb, 1.0f);

          if (!detectPixel(nr, ng, nb, nr, ng, nb, _colIdxIn, _rowIdxIn))
            return false;

          if (!m_pixelSetOut[0].fromNormalizedRGB(nr, ng, nb))
            return false;


          for (/*m_columnOutCurrent*/; m_columnOutCurrent <= _colIdxOut; ++m_columnOutCurrent)
            if (!m_rowSetOut.writePixelSet(m_pixelSetOut, m_columnOutCurrent))
              return false;

          return true;
        }

        bool filterPixelHSV(float _r, float _g, float _b) const
        {
          const float rgb_max = std::max(_r, std::max(_g, _b));
          const float rgb_min = std::min(_r, std::min(_g, _b));
          const float rgb_delta = rgb_max-rgb_min;

          const float hsv_v = rgb_max;
          if (hsv_v < m_detectVal.first || hsv_v > m_detectVal.second)
            return false;

          const float hsv_s = rgb_delta / std::max(rgb_max, 1e-20f);
          if (hsv_s < m_detectSat.first || hsv_s > m_detectSat.second)
            return false;

          float hsv_h;

          if (_r == rgb_max)
            hsv_h =        (_g - _b) / std::max(rgb_delta, 1e-20f);
          else if (_g == rgb_max)
            hsv_h = 2.0f + (_b - _r) / std::max(rgb_delta, 1e-20f);
          else
            hsv_h = 4.0f + (_r - _g) / std::max(rgb_delta, 1e-20f);
          hsv_h *= 60.0f;

          if (hsv_h < 0.0f)
            hsv_h += 360.0f;
          else if (hsv_h >= 360.0f)
            hsv_h -= 360.0f;

          if (m_detectHue.first <= m_detectHue.second)
          {
            if (hsv_h < m_detectHue.first || hsv_h > m_detectHue.second)
              return false;
          }
          else
          {
            if (hsv_h < m_detectHue.first && hsv_h > m_detectHue.second)
              return false;
          }

          return true;
        }

        bool detectPixel(float _r, float _g, float _b,
                         float& _adjR, float& _adjG, float& _adjB,
                         ImageDim _col, ImageDim _row)
        {
          if (!filterPixelHSV(_r, _g, _b))
            return true;

          _adjR = 1.0f;
          _adjG = 1.0f;
          _adjB = 0.0f;
          m_detectCenterX += _col;
          m_detectCenterY += _row;
          ++m_detectCount;

          return true;
        }


      private:
        RowSetIn    m_rowSetIn;
        RowSetOut   m_rowSetOut;
        PixelSetIn  m_pixelSetIn;
        PixelSetOut m_pixelSetOut;
        ImageDim m_columnOutCurrent;
        ImageDim m_width;
        ImageDim m_height;
        std::pair<float, float> m_detectHue;
        std::pair<float, float> m_detectSat;
        std::pair<float, float> m_detectVal;
        long m_detectCenterX;
        long m_detectCenterY;
        long m_detectCount;
    };


  public:
    AlgoDetector(const std::pair<float, float>& _hue,
                 const std::pair<float, float>& _sat,
                 const std::pair<float, float>& _val)
     :m_detectHue(_hue),
      m_detectSat(_sat),
      m_detectVal(_val)
    {
    }

    bool operator()(const _ImageIn& _imageIn,
                    _ImageOut& _imageOut) const
    {
      if (!_imageIn || !_imageOut)
        return false;

#warning Temporary - build caches here instead of constructor
      if (!const_cast<AlgoDetector*>(this)->buildCaches(_imageIn.width(),  _imageIn.height(),
                                                        _imageOut.width(), _imageOut.height()))
        return false;

      const ImageDim imageOutWidth  = _imageOut.width();
      const ImageDim imageOutHeight = _imageOut.height();
      const ImageDim imageInWidth   = _imageIn.width();
      const ImageDim imageInHeight  = _imageIn.height();

#ifdef __TI_COMPILER_VERSION__
      if (imageInWidth % 8 != 0 && imageInWidth < 8*4)
        return false;
#endif

      ExecutionInstance execution(imageInWidth, imageInHeight, m_detectHue, m_detectSat, m_detectVal);

      for (ImageDim rowIdxIn = 0; rowIdxIn < imageInHeight; ++rowIdxIn)
      {
        assert(rowIdxIn >= 0 && rowIdxIn < m_verticalInput2OutputCache.size());
        const ImageDim rowIdxOut = m_verticalInput2OutputCache[rowIdxIn];
        assert(rowIdxOut >= 0 && rowIdxOut < imageOutHeight);

        if (!execution.prepareRowSets(_imageIn, rowIdxIn, _imageOut, rowIdxOut))
          return false;

#ifdef __TI_COMPILER_VERSION__
#pragma MUST_ITERATE(8*4, ,8)
#endif
        for (ImageDim colIdxIn = 0; colIdxIn < imageInWidth; ++colIdxIn)
        {
          assert(colIdxIn >= 0 && colIdxIn < m_horizontalInput2OutputCache.size());
          const ImageDim colIdxOut = m_horizontalInput2OutputCache[colIdxIn];
          assert(colIdxOut >= 0 && colIdxOut < imageOutWidth);

          if (!execution.processPixelSet(colIdxIn, rowIdxIn, colIdxOut))
            return false;
        }
      }

      return true;
    }


  private:
    Input2OutputCache m_verticalInput2OutputCache;
    Input2OutputCache m_horizontalInput2OutputCache;
    std::pair<float, float> m_detectHue;
    std::pair<float, float> m_detectSat;
    std::pair<float, float> m_detectVal;

    static bool buildCache(Input2OutputCache& _cache, ImageDim _inDim, ImageDim _outDim)
    {
      _cache.resize(_inDim);

      ImageDimFactor outIdxFull = 0;
      const ImageDimFactor in2outFactor = static_cast<ImageDimFactor>(_outDim) / static_cast<ImageDimFactor>(_inDim);

      for (ImageDim inIdx = 0; inIdx < _inDim; ++inIdx)
      {
        outIdxFull += in2outFactor; // same as outIdxFull = inIdx*in2outFactor

        const ImageDim outIdx = /*trunc*/outIdxFull;
        _cache[inIdx]  = outIdx;
      }

      return true;
    }

    bool buildCaches(ImageDim _inWidth,  ImageDim _inHeight,
                     ImageDim _outWidth, ImageDim _outHeight)
    {
      if (!buildCache(m_horizontalInput2OutputCache, _inWidth, _outWidth))
        return false;

      if (!buildCache(m_verticalInput2OutputCache, _inHeight, _outHeight))
        return false;

      return true;
    }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */


template <typename _ImageIn, typename _ImageOut>
class ImageAlgorithm<BaseImageAlgorithm::AlgoDetector, _ImageIn, _ImageOut>
 : public BaseImageAlgorithm,
   public internal::AlgoDetector<_ImageIn, _ImageOut>
{
  public:
    ImageAlgorithm(const std::pair<float, float>& _hue,
                   const std::pair<float, float>& _sat,
                   const std::pair<float, float>& _val)
     :internal::AlgoDetector<_ImageIn, _ImageOut>(_hue, _sat, _val)
    {
    }
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_DETECTOR_HPP_
