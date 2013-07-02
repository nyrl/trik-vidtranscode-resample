#ifndef TRIK_VIDEO_RESAMPLE_DEMOS_COMMON_HPP_
#define TRIK_VIDEO_RESAMPLE_DEMOS_COMMON_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <map>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace demos /* **** **** **** **** **** */ {


template <typename _RawFormat>
class VideoFormat
{
  public:
    typedef _RawFormat RawFormat;
    typedef std::map<RawFormat, std::string> FormatMap;
    typedef std::shared_ptr<FormatMap> FormatMapPtr;

    VideoFormat()
     :m_knownFormats(),
      m_rawFormat()
    {
    }

    explicit VideoFormat(const FormatMapPtr& _knownFormats)
     :m_knownFormats(_knownFormats),
      m_rawFormat((  !m_knownFormats
                   || m_knownFormats->empty())
                  ? RawFormat()
                  : m_knownFormats->begin()->first)
    {
    }

    VideoFormat(const VideoFormat& _format, const FormatMapPtr& _knownFormats)
     :m_knownFormats(_knownFormats),
      m_rawFormat((  !m_knownFormats
                   || m_knownFormats->find(_format.m_rawFormat) == m_knownFormats->end())
                  ? RawFormat()
                  : _format.m_rawFormat)
    {
    }

    VideoFormat(const VideoFormat& _format, const RawFormat& _rawFormat)
     :m_knownFormats(_format.m_knownFormats),
      m_rawFormat((  !m_knownFormats
                   || m_knownFormats->find(_rawFormat) == m_knownFormats->end())
                  ? RawFormat()
                  : _rawFormat)
    {
    }

    const RawFormat& rawFormat() const
    {
      return m_rawFormat;
    }

    bool rawFormat(const RawFormat& _rawFormat)
    {
      if (   _rawFormat != RawFormat()
          && (  !m_knownFormats
              || m_knownFormats->find(_rawFormat) == m_knownFormats->end()))
        return false;

      m_rawFormat = _rawFormat;
      return true;
    }

    void reportKnownFormats(std::ostream& _os) const
    {
      if (m_knownFormats)
      {
        const typename FormatMap::const_iterator& end(m_knownFormats->end());
        bool first = true;
        for (typename FormatMap::const_iterator it(m_knownFormats->begin()); it != end; ++it)
        {
          if (first)
            first = false;
          else
            _os << ", ";
          _os << it->second;
        }
      }
      else
        _os << "(null)";
    }

    friend std::istream& operator>>(std::istream& _is, VideoFormat& _format)
    {
      if (_format.m_knownFormats)
      {
        std::string s;
        _is >> s;

        const typename FormatMap::const_iterator& end(_format.m_knownFormats->end());
        for (typename FormatMap::const_iterator it(_format.m_knownFormats->begin()); it != end; ++it)
          if (it->second.compare(s) == 0)
          {
            _format.m_rawFormat = it->first;
            return _is;
          }
      }

      _is.setstate(std::ios::failbit);
      return _is;
    }

    friend std::ostream& operator<<(std::ostream& _os, const VideoFormat& _format)
    {
      if (!_format.m_knownFormats)
        _os.setstate(std::ios::failbit);
      else
      {
        const typename FormatMap::const_iterator& it = _format.m_knownFormats->find(_format.m_rawFormat);
        if (it == _format.m_knownFormats->end())
          _os.setstate(std::ios::failbit);
        else
          _os << it->second;
      }

      return _os;
    }

  private:
    FormatMapPtr m_knownFormats;
    RawFormat    m_rawFormat;
};




template <typename _VideoFormat>
class VideoConfig
{
  public:
    typedef uint32_t           VideoDimension;
    typedef _VideoFormat       VideoFormat;

    VideoConfig()
     :m_width(),
      m_height(),
      m_format()
    {
    }

    VideoConfig(const VideoDimension& _width, const VideoDimension& _height)
     :m_width(_width),
      m_height(_height),
      m_format()
    {
    }

    VideoConfig(const VideoDimension& _width, const VideoDimension& _height, const VideoFormat& _format)
     :m_width(_width),
      m_height(_height),
      m_format(_format)
    {
    }

    VideoConfig(const typename VideoFormat::FormatMapPtr& _formatMapPtr)
     :m_width(),
      m_height(),
      m_format(_formatMapPtr)
    {
    }

     VideoConfig(const VideoConfig& _config, const typename VideoFormat::FormatMapPtr& _formatMapPtr)
     :m_width(_config.width()),
      m_height(_config.height()),
      m_format(_config.format(), _formatMapPtr)
    {
    }

    const VideoDimension& width()      const { return m_width; }
    VideoDimension&       width()            { return m_width; }
    const VideoDimension& height()     const { return m_height; }
    VideoDimension&       height()           { return m_height; }
    const VideoFormat&    format()     const { return m_format; }
    VideoFormat&          format()           { return m_format; }

  private:
    VideoDimension   m_width;
    VideoDimension   m_height;
    VideoFormat      m_format;
};




template <typename _VideoFormat>
class VideoImageDescription : public VideoConfig<_VideoFormat>
{
  public:
    typedef VideoConfig<_VideoFormat> Config;
    typedef size_t                    VideoSize;

    VideoImageDescription()
     :Config(),
      m_bytesPerLine(),
      m_bytesPerImage()
    {
    }

    VideoImageDescription(const typename Config::VideoDimension& _width, const typename Config::VideoDimension& _height,
                          const typename Config::VideoFormat& _format,
                          const VideoSize& _bytesPerLine,
                          const VideoSize& _bytesPerImage)
     :Config(_width, _height, _format),
      m_bytesPerLine(_bytesPerLine),
      m_bytesPerImage(_bytesPerImage)
    {
    }

    const VideoSize&      bytesPerLine()  const { return m_bytesPerLine; }
    const VideoSize&      bytesPerImage() const { return m_bytesPerImage; }

  private:
    VideoSize        m_bytesPerLine;
    VideoSize        m_bytesPerImage;
};




template <typename _Ptr>
class VideoFrame
{
  public:
    typedef _Ptr              Ptr;
    typedef size_t            Size;

    VideoFrame()
     :m_ptr(),
      m_size()
    {
    }

    VideoFrame(const Ptr& _ptr, const Size& _size)
     :m_ptr(_ptr),
      m_size(_size)
    {
    }

    const Ptr&    ptr()  const { return m_ptr; }
    const Size&   size() const { return m_size; }
    void          size(const size_t& _size) { m_size = _size; }

  private:
    Ptr   m_ptr;
    Size  m_size;
};



} /* **** **** **** **** **** * namespace demos * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_DEMOS_V4L2DEVICE_HPP_
