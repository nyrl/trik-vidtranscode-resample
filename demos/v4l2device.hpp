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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/videodev2.h>
#include <libv4l2.h>

#include "demos/common.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace demos /* **** **** **** **** **** */ {

typedef VideoFormat<uint32_t> V4L2Format;

class V4L2Config : public VideoConfig<V4L2Format>
{
  public:
    V4L2Config()
     :VideoConfig(),
      m_path()
    {
    }

    V4L2Config(const std::string& _path, const VideoDimension& _width, const VideoDimension& _height)
     :VideoConfig(_width, _height),
      m_path(_path)
    {
    }

    explicit V4L2Config(const VideoFormat::FormatMapPtr& _formatMap)
     :VideoConfig(_formatMap),
      m_path()
    {
    }

     V4L2Config(const V4L2Config& _config, const VideoFormat::FormatMapPtr& _formatMap)
      :VideoConfig(_config, _formatMap),
       m_path(_config.m_path)
    {
    }

    const std::string& path() const { return m_path; }
    std::string&       path()       { return m_path; }
    void               path(const std::string& _path) { m_path = _path; }

  private:
    std::string m_path;
};




class V4L2Input
{
  public:
    typedef V4L2Config                              Config;
    typedef VideoImageDescription<V4L2Format>       Description;
    typedef VideoFrame<uint8_t*>                    Frame;
    typedef uint32_t                                FrameIndex;
    typedef void*                                   BufferPtr;
    typedef size_t                                  BufferSize;

    V4L2Input()
     :m_config(knownFormats()),
      m_description(),
      m_v4l2fd(-1),
      m_v4l2buffers()
    {
    }

    explicit V4L2Input(const Config& _config, const std::string& _format)
     :m_config(_config, knownFormats()),
      m_description(),
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
      if (!doUnsetFormat())
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

    const Description& description() const
    {
      return m_description;
    }

    bool getFrame(Frame& _frame, FrameIndex& _index)
    {
      return doGetFrame(_frame, _index);
    }

    bool ungetFrame(const FrameIndex& _index)
    {
      return doUngetFrame(_index);
    }

  protected:
    static V4L2Config::VideoFormat::FormatMapPtr knownFormats()
    {
      V4L2Config::VideoFormat::FormatMapPtr res = std::make_shared<V4L2Config::VideoFormat::FormatMap>();

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
      m_v4l2format.fmt.pix.pixelformat = m_config.format().rawFormat();
      m_v4l2format.fmt.pix.field = V4L2_FIELD_NONE;

      int res;
      if ((res = v4l2_ioctl(m_v4l2fd, VIDIOC_S_FMT, &m_v4l2format)) != 0)
      {
        if (v4l2_log_file)
          fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_S_FMT) failed: %d/%d\n", res, errno);
        return false;
      }

      // note that actual parametes might differ from what was set to it!
      m_description = Description(m_v4l2format.fmt.pix.width, m_v4l2format.fmt.pix.height,
                                  V4L2Format(m_config.format(), m_v4l2format.fmt.pix.pixelformat),
                                  m_v4l2format.fmt.pix.bytesperline,
                                  m_v4l2format.fmt.pix.sizeimage);

      return true;
    }

    bool doUnsetFormat()
    {
      m_description = Description();
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

      for (size_t bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
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
            fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_QUERYBUF)[%zu] failed: %d/%d\n", bufIdx, res, errno);
          return false;
        }

        m_v4l2buffers[bufIdx].m_size = buf.length;
        m_v4l2buffers[bufIdx].m_ptr = v4l2_mmap(NULL, buf.length,
                                                PROT_READ|PROT_WRITE, MAP_SHARED,
                                                m_v4l2fd, buf.m.offset);
        if (m_v4l2buffers[bufIdx].m_ptr == MAP_FAILED)
        {
          if (v4l2_log_file)
            fprintf(v4l2_log_file, "v4l2_mmap[%zu] failed: %d\n", bufIdx, errno);
          return false;
        }
      }

      return true;
    }

    bool doMunmapBuffers()
    {
      bool isOk = true;

      for (size_t bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
        if (m_v4l2buffers[bufIdx].m_ptr != MAP_FAILED)
        {
          int res;
          if ((res = v4l2_munmap(m_v4l2buffers[bufIdx].m_ptr, m_v4l2buffers[bufIdx].m_size)) != 0)
          {
            if (v4l2_log_file)
              fprintf(v4l2_log_file, "v4l2_munmap(%p)[%zu] failed: %d\n", m_v4l2buffers[bufIdx].m_ptr, bufIdx, errno);
            isOk = false;
          }
        }

      m_v4l2buffers.resize(0);
      return isOk;
    }

    bool doStart()
    {
      for (size_t bufIdx = 0; bufIdx < m_v4l2buffers.size(); ++bufIdx)
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
            fprintf(v4l2_log_file, "v4l2_ioctl(VIDIOC_QBUF)[%zu] failed: %d/%d\n", bufIdx, res, errno);
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

    bool doGetFrame(Frame& _frame, FrameIndex& _index)
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
#if 1
printf("Frame sizes: %zu/%"PRIu32"/%zu\n", m_v4l2buffers[_index].m_size, buf.length, m_description.bytesPerImage());
#endif

      _frame = Frame(reinterpret_cast<Frame::Ptr>(m_v4l2buffers[_index].m_ptr),
                     std::min<Frame::Size>(m_v4l2buffers[_index].m_size, m_description.bytesPerImage()));
      return true;
    }

    bool doUngetFrame(const FrameIndex& _index)
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
    Description m_description;
    int         m_v4l2fd;
    v4l2_format m_v4l2format;

    struct MmapBuffer
    {
      MmapBuffer() : m_ptr(MAP_FAILED), m_size(0) {}

      BufferPtr  m_ptr;
      BufferSize m_size;
    };
    std::vector<MmapBuffer> m_v4l2buffers;


    V4L2Input(const V4L2Input&);
    V4L2Input& operator=(const V4L2Input&);
};




} /* **** **** **** **** **** * namespace demos * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_DEMOS_V4L2DEVICE_HPP_
