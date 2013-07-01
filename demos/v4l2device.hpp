#ifndef TRIK_VIDEO_RESAMPLE_DEMOS_V4L2DEVICE_HPP_
#define TRIK_VIDEO_RESAMPLE_DEMOS_V4L2DEVICE_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <cinttypes>
#include <sys/mman.h>
#include <errno.h>

#include <linux/videodev2.h>
#include <libv4l2.h>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace demos /* **** **** **** **** **** */ {


class V4L2Format
{
  public:
    typedef uint32_t PixelFormat;
    typedef std::map<PixelFormat, std::string> FormatMap;
    typedef std::shared_ptr<FormatMap> FormatMapPtr;

    explicit V4L2Format()
     :m_knownFormats(),
      m_format(0)
    {
    }

    explicit V4L2Format(const FormatMapPtr& _knownFormats)
     :m_knownFormats(_knownFormats),
      m_format((  !m_knownFormats
                || m_knownFormats->empty())
               ? 0
               : m_knownFormats->begin()->first)
    {
    }

    explicit V4L2Format(const PixelFormat& _fmt, const FormatMapPtr& _knownFormats)
     :m_knownFormats(_knownFormats),
      m_format((  !m_knownFormats
                || m_knownFormats->find(_fmt) == m_knownFormats->end())
               ? 0
               : _fmt)
    {
    }

    const PixelFormat& v4l2fmt() const
    {
      return m_format;
    }

    bool v4l2fmt(const PixelFormat& _fmt)
    {
      if (   _fmt != 0
          && (  !m_knownFormats
              || m_knownFormats->find(_fmt) == m_knownFormats->end()))
        return false;

      m_format = _fmt;
      return true;
    }

    void reportKnownFormats(std::ostream& _os) const
    {
      if (m_knownFormats)
      {
        const FormatMap::const_iterator& end(m_knownFormats->end());
        bool first = true;
        for (FormatMap::const_iterator it(m_knownFormats->begin()); it != end; ++it)
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

    friend std::istream& operator>>(std::istream& _is, V4L2Format& _fmt)
    {
      if (_fmt.m_knownFormats)
      {
        std::string s;
        _is >> s;

        const FormatMap::const_iterator& end(_fmt.m_knownFormats->end());
        for (FormatMap::const_iterator it(_fmt.m_knownFormats->begin()); it != end; ++it)
          if (it->second.compare(s) == 0)
          {
            _fmt.m_format = it->first;
            return _is;
          }
      }

      _is.setstate(std::ios::failbit);
      return _is;
    }

    friend std::ostream& operator<<(std::ostream& _os, const V4L2Format& _fmt)
    {
      if (!_fmt.m_knownFormats)
        _os.setstate(std::ios::failbit);
      else
      {
        const FormatMap::const_iterator& it = _fmt.m_knownFormats->find(_fmt.m_format);
        if (it == _fmt.m_knownFormats->end())
          _os.setstate(std::ios::failbit);
        else
          _os << it->second;
      }

      return _os;
    }

  private:
    FormatMapPtr m_knownFormats;
    PixelFormat  m_format;
};




class V4L2Config
{
  public:
    typedef uint32_t Dimension;
    typedef V4L2Format Format;

    V4L2Config()
     :m_path(),
      m_width(),
      m_height(),
      m_format()
    {
    }

    explicit V4L2Config(const std::string& _path)
     :m_path(_path),
      m_width(),
      m_height(),
      m_format()
    {
    }

    V4L2Config(const std::string& _path, const Dimension& _width, const Dimension& _height)
     :m_path(_path),
      m_width(_width),
      m_height(_height),
      m_format()
    {
    }

    explicit V4L2Config(const Format::FormatMapPtr& _formatMap)
     :m_path(),
      m_width(),
      m_height(),
      m_format(_formatMap)
    {
    }

     V4L2Config(const V4L2Config& _config, const Format::FormatMapPtr& _formatMap)
     :m_path(_config.m_path),
      m_width(_config.m_width),
      m_height(_config.m_height),
      m_format(_formatMap)
    {
    }

    const std::string& path() const { return m_path; }
    std::string&       path()       { return m_path; }
    void               path(const std::string& _path) { m_path = _path; }

    const Dimension&   width() const { return m_width; }
    Dimension&         width()       { return m_width; }
    void               width(const Dimension& _width) { m_width = _width; }

    const Dimension&   height() const { return m_height; }
    Dimension&         height()       { return m_height; }
    void               height(const Dimension& _height) { m_height = _height; }

    const Format&      format() const { return m_format; }
    Format&            format() { return m_format; }
    void               format(const Format& _format) { m_format = _format; }

  private:
    std::string m_path;
    Dimension   m_width;
    Dimension   m_height;
    Format      m_format;
};




class V4L2BufferDescription
{
  public:
    typedef void*    BufferPtr;
    typedef size_t   BufferSize;
    typedef uint32_t BufferIndex;
    typedef uint32_t Dimension;
    typedef V4L2Format Format;

    V4L2BufferDescription()
     :m_ptr(),
      m_size(),
      m_index(),
      m_width(),
      m_height(),
      m_lineLength(),
      m_format()
    {
    }

    V4L2BufferDescription(const BufferPtr& _ptr, const BufferSize& _size,
                          const BufferIndex& _index,
                          const Dimension& _width, const Dimension& _height,
                          const BufferSize& _lineLength,
                          const Format& _format)
     :m_ptr(_ptr),
      m_size(_size),
      m_index(_index),
      m_width(_width),
      m_height(_height),
      m_lineLength(_lineLength),
      m_format(_format)
    {
    }

    const BufferPtr&    ptr()        const { return m_ptr; }
    const BufferSize&   size()       const { return m_size; }
    const BufferIndex&  index()      const { return m_index; }
    const Dimension&    width()      const { return m_width; }
    const Dimension&    height()     const { return m_height; }
    const BufferSize&   lineLength() const { return m_lineLength; }

  private:
    BufferPtr   m_ptr;
    BufferSize  m_size;
    BufferIndex m_index;
    Dimension   m_width;
    Dimension   m_height;
    BufferSize  m_lineLength;
    Format      m_format;
};



class V4L2Input
{
  public:
    typedef V4L2Config Config;
    typedef void*    BufferPtr;
    typedef size_t   BufferSize;
    typedef uint32_t BufferIndex;

    V4L2Input()
     :m_config(knownFormats()),
      m_v4l2fd(-1),
      m_v4l2buffers()
    {
    }

    explicit V4L2Input(const Config& _config, const std::string& _format)
     :m_config(_config, knownFormats()),
      m_v4l2fd(-1),
      m_v4l2buffers()
    {
      std::istringstream is(_format);
      is >> m_config.format();
    }

    ~V4L2Input()
    {
      close();
    }

    const Config& config() const { return m_config; }
    Config&       config()       { return m_config; }

    bool open()
    {
      if (   doOpen()
          && doSetFormat()
          && doMmapBuffers())
        return true;

      close();
      return false;
    }

    bool close()
    {
      bool isOk = true;
      if (!doMunmapBuffers())
        isOk = false;
      if (!doClose())
        isOk = false;

      return isOk;
    }

    bool start()
    {
      if (doStart())
        return true;

      return false;
    }

    bool stop()
    {
      bool isOk = true;
      if (!doStop())
        isOk = false;

      return isOk;
    }

    int v4l2fd() const
    {
      return m_v4l2fd;
    }

    bool getFrame(V4L2BufferDescription& _frame)
    {
      BufferPtr ptr;
      BufferSize size;
      BufferIndex index;

      if (!doGetFrame(ptr, size, index))
        return false;

      _frame = V4L2BufferDescription(ptr, size,
                                     index,
                                     m_v4l2format.fmt.pix.width, m_v4l2format.fmt.pix.height,
                                     m_v4l2format.fmt.pix.bytesperline,
                                     m_config.format());
      return true;
    }

    bool ungetFrame(const V4L2BufferDescription& _frame)
    {
      return doUngetFrame(_frame.index());
    }

  protected:
    static V4L2Format::FormatMapPtr knownFormats()
    {
      V4L2Format::FormatMapPtr res = std::make_shared<V4L2Format::FormatMap>();

      res->insert(std::make_pair(V4L2_PIX_FMT_RGB24,  "RGB888"));
      res->insert(std::make_pair(V4L2_PIX_FMT_RGB565, "RGB565"));

      return res;
    }

    bool doOpen()
    {
      if (m_v4l2fd != -1)
        return false;

      m_v4l2fd = v4l2_open(m_config.path().c_str(), O_RDWR|O_NONBLOCK, 0);
      if (m_v4l2fd < 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_open(%s) failed: %d\n", m_config.path().c_str(), errno);
        m_v4l2fd = -1;
        return false;
      }

      return true;
    }

    bool doClose()
    {
      bool isOk = true;
      int res;
      if ((res = v4l2_close(m_v4l2fd)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_close() failed: %d\n", errno);
        isOk = false;
      }
      m_v4l2fd = -1;

      return isOk;
    }

    bool doSetFormat()
    {
      memset(&m_v4l2format, 0, sizeof(m_v4l2format));
      m_v4l2format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      m_v4l2format.fmt.pix.width = m_config.width();
      m_v4l2format.fmt.pix.height = m_config.height();
      m_v4l2format.fmt.pix.pixelformat = m_config.format().v4l2fmt();
      m_v4l2format.fmt.pix.field = V4L2_FIELD_INTERLACED;

      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_S_FMT, &m_v4l2format)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_S_FMT) failed: %d/%d\n", res, errno);
        return false;
      }

      // note that actual parametes might differ from what was set to it!
      // check format, other are not important
      if (m_v4l2format.fmt.pix.pixelformat != m_config.format().v4l2fmt())
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_S_FMT) changed format to %"PRIu32", configured %"PRIu32"\n",
                  m_v4l2format.fmt.pix.pixelformat, m_config.format().v4l2fmt());
        return false;
      }

      return true;
    }

    bool doMmapBuffers()
    {
      struct v4l2_requestbuffers reqBufs;
      memset(&reqBufs, 0, sizeof(reqBufs));
      reqBufs.count = 2;
      reqBufs.type = m_v4l2format.type;
      reqBufs.memory = V4L2_MEMORY_MMAP;

      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_REQBUFS, &reqBufs)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_REQBUFS) failed: %d/%d\n", res, errno);
        return false;
      }

      if (reqBufs.count <= 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_REQBUFS) requested no buffers\n");
        return false;
      }
      m_v4l2buffers.resize(reqBufs.count);

      for (BufferIndex bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
      {
        v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.index = bufIdx;
        buf.type = m_v4l2format.type;
        buf.memory = V4L2_MEMORY_MMAP;

        int res;
        if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_QUERYBUF, &buf)) != 0)
        {
          if (v4l2_log_file)
            fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_QUERYBUF)[%"PRIu32"] failed: %d/%d\n", bufIdx, res, errno);
          return false;
        }

        m_v4l2buffers[bufIdx].m_size = buf.length;
        m_v4l2buffers[bufIdx].m_ptr = v4l2_mmap(NULL, buf.length,
                                                PROT_READ|PROT_WRITE, MAP_SHARED,
                                                m_v4l2fd, buf.m.offset);
        if (m_v4l2buffers[bufIdx].m_ptr == MAP_FAILED)
        {
          if (v4l2_log_file)
            fprintf(v4l2_log_file, "v4l2_mmap[%"PRIu32"] failed: %d\n", bufIdx, errno);
          return false;
        }
      }

      return true;
    }

    bool doMunmapBuffers()
    {
      bool isOk = true;

      for (BufferIndex bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
        if (m_v4l2buffers[bufIdx].m_ptr != MAP_FAILED)
        {
          int res;
          if ((res = v4l2_munmap(m_v4l2buffers[bufIdx].m_ptr, m_v4l2buffers[bufIdx].m_size)) != 0)
          {
            if (v4l2_log_file)
              fprintf(v4l2_log_file, "v4l2_munmap(%p)[%"PRIu32"] failed: %d\n", m_v4l2buffers[bufIdx].m_ptr, bufIdx, errno);
            isOk = false;
          }
        }

      m_v4l2buffers.resize(0);
      return isOk;
    }

    bool doStart()
    {
      for (BufferIndex bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
      {
        v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.index = bufIdx;
        buf.type = m_v4l2format.type;
        buf.memory = V4L2_MEMORY_MMAP;
        int res;
        if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_QBUF, &buf)) != 0)
        {
          if (v4l2_log_file)
            fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_QBUF)[%"PRIu32"] failed: %d/%d\n", bufIdx, res, errno);
          return false;
        }
      }

      v4l2_buf_type capture = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_STREAMON, &capture)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_STREAMON) failed: %d/%d\n", res, errno);
        return false;
      }

      return true;
    }

    bool doStop()
    {
      bool isOk = true;
      v4l2_buf_type capture = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_STREAMOFF, &capture)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_STREAMOFF) failed: %d/%d\n", res, errno);
        isOk = false;
      }

      return isOk;
    }

    bool doGetFrame(BufferPtr& _ptr, BufferSize& _size, BufferIndex& _index)
    {
      v4l2_buffer buf;
      memset(&buf, 0, sizeof(buf));
      buf.type = m_v4l2format.type;
      buf.memory = V4L2_MEMORY_MMAP;
      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_DQBUF, &buf)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_DQBUF) failed: %d/%d\n", res, errno);
        return false;
      }

      _index = buf.index;
      if (buf.index >= m_v4l2buffers.size())
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_DQBUF) returned index out range: %"PRIu32"\n", _index);
        return false;
      }

      _ptr  = m_v4l2buffers[_index].m_ptr;
      _size = m_v4l2buffers[_index].m_size;
      return true;
    }

    bool doUngetFrame(const BufferIndex& _index)
    {
      v4l2_buffer buf;
      memset(&buf, 0, sizeof(buf));
      buf.index = _index;
      buf.type = m_v4l2format.type;
      buf.memory = V4L2_MEMORY_MMAP;
      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_QBUF, &buf)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_QBUF)[%"PRIu32"] failed: %d/%d\n", _index, res, errno);
        return false;
      }

      return true;
    }

  private:
    Config      m_config;
    int         m_v4l2fd;
    v4l2_format m_v4l2format;

    struct MmapBuffer
    {
      MmapBuffer() : m_ptr(MAP_FAILED), m_size(0) {}

      BufferPtr   m_ptr;
      BufferSize  m_size;
    };
    std::vector<MmapBuffer> m_v4l2buffers;


    V4L2Input(const V4L2Input&);
    V4L2Input& operator=(const V4L2Input&);
};


} /* **** **** **** **** **** * namespace demos * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_DEMOS_V4L2DEVICE_HPP_
