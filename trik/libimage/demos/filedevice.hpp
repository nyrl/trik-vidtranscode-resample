#ifndef TRIK_VIDEO_RESAMPLE_DEMOS_FILEDEVICE_HPP_
#define TRIK_VIDEO_RESAMPLE_DEMOS_FILEDEVICE_HPP_

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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/videodev2.h>

#include "common.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace demos /* **** **** **** **** **** */ {

typedef VideoFormat<uint32_t> FileFormat;

class FileConfig : public VideoConfig<FileFormat>
{
  public:
    FileConfig()
     :VideoConfig(),
      m_path()
    {
    }

    FileConfig(const std::string& _path, const VideoDimension& _width, const VideoDimension& _height)
     :VideoConfig(_width, _height),
      m_path(_path)
    {
    }

    explicit FileConfig(const VideoFormat::FormatMapPtr& _formatMap)
     :VideoConfig(_formatMap),
      m_path()
    {
    }

    FileConfig(const FileConfig& _config, const VideoFormat::FormatMapPtr& _formatMap)
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



class FileOutput
{
  public:
    typedef FileConfig                              Config;
    typedef VideoImageDescription<FileFormat>       Description;
    typedef VideoFrame<uint8_t*>                    Frame;

    FileOutput()
     :m_config(knownFormats()),
      m_description(),
      m_fd(-1)
    {
    }

    explicit FileOutput(const Config& _config, const std::string& _format)
     :m_config(_config, knownFormats()),
      m_description(),
      m_fd(-1)
    {
      std::istringstream is(_format);
      is >> m_config.format();
    }

    ~FileOutput()
    {
      close();
    }

    const Config& config() const { return m_config; }
    Config&       config()       { return m_config; }

    bool open()
    {
      if (   doOpen()
          && doSetFormat())
        return true;

      close();
      return false;
    }

    bool close()
    {
      bool isOk = true;
      if (!doUnsetFormat())
        isOk = false;
      if (!doClose())
        isOk = false;

      return isOk;
    }

    bool start()
    {
      return true;
    }

    bool stop()
    {
      return true;
    }

    bool getFrame(Frame& _frame)
    {
      return doGetFrame(_frame);
    }

    bool putFrame(const Frame& _frame)
    {
      return doPutFrame(_frame);
    }

    const Description& description() const
    {
      return m_description;
    }

  protected:
    static FileConfig::VideoFormat::FormatMapPtr knownFormats()
    {
      FileConfig::VideoFormat::FormatMapPtr res = std::make_shared<FileConfig::VideoFormat::FormatMap>();

      res->insert(std::make_pair(V4L2_PIX_FMT_RGB24,  "RGB888"));
      res->insert(std::make_pair(V4L2_PIX_FMT_RGB565, "RGB565"));

      return res;
    }

    bool doOpen()
    {
      if (m_fd != -1)
        return false;

      m_fd = ::open(m_config.path().c_str(), O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR);
      if (m_fd < 0)
      {
        fprintf(stderr, "open(%s) failed: %d\n", m_config.path().c_str(), errno);
        m_fd = -1;
        return false;
      }

      return true;
    }

    bool doClose()
    {
      bool isOk = true;
      if (m_fd != -1)
      {
        int res;
        if ((res = ::close(m_fd)) != 0)
        {
          fprintf(stderr, "close() failed: %d\n", errno);
          isOk = false;
        }
        m_fd = -1;
      }

      return isOk;
    }

    bool doSetFormat()
    {
      m_frameBuffer.resize(m_config.width() * m_config.height() * 4); // 4 bytes per pixel should be enought
      m_description = Description(m_config.width(), m_config.height(),
                                  m_config.format(),
                                  0, 0);
      return true;
    }

    bool doUnsetFormat()
    {
      m_description = Description();
      m_frameBuffer.resize(0);
      return true;
    }

    bool doStart()
    {
      return true;
    }

    bool doStop()
    {
      return true;
    }

    bool doGetFrame(Frame& _frame)
    {
      _frame = Frame(&m_frameBuffer.front(), m_frameBuffer.size());
      return true;
    }

    bool doPutFrame(const Frame& _frame)
    {
      ssize_t written;
      if ((written = write(m_fd, _frame.ptr(), _frame.size())) != _frame.size())
      {
        fprintf(stderr, "write(%zu) failed: %zd/%d\n", _frame.size(), written, errno);
        return false;
      }

      return true;
    }

  private:
    Config               m_config;
    Description          m_description;
    int                  m_fd;
    std::vector<uint8_t> m_frameBuffer;

    FileOutput(const FileOutput&);
    FileOutput& operator=(const FileOutput&);
};



} /* **** **** **** **** **** * namespace demos * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_VIDEO_RESAMPLE_DEMOS_FILEDEVICE_HPP_
