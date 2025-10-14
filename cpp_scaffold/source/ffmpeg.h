/*
    1. 针对视频文件进行转码，生成转码后的视频文件
    2. 解析生成M3U8文件内容，获取内容结构，并能够生成新的M3U8文件
*/
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}
#include <string>
#include <vector>
#include <fstream>
#include "logger.h"
#include "util.h"

namespace bubble
{
    struct TransCoderSettings
    {
        std::string input_file; // 输入文件路径
        std::string output_file; // 输出文件路径
        int hls_time = 10; // hls片段时间
        std::string hls_base_url; // hls基础url
    };
    // 视频转码为hls格式
    class TransCoder
    {
    public:
        TransCoder() = default;
        ~TransCoder() = default;
        TransCoder(const TransCoderSettings &settings);
        // 转码
        bool transCode();
    private:
        std::string av_err_str(int errnum);
    private:
        TransCoderSettings _settings;
    };


    class M3U8Info
    {
        const std::string EXTM3U = "#EXTM3U";
        const std::string EXT_X_VERSION = "#EXT-X-VERSION";
        const std::string EXT_X_TARGETDURATION = "#EXT-X-TARGETDURATION";
        const std::string EXT_X_MEDIA_SEQUENCE = "#EXT-X-MEDIA-SEQUENCE";
        const std::string EXT_X_PLAYLIST_TYPE = "#EXT-X-PLAYLIST-TYPE";
        const std::string EXT_X_INDEPENDENT_SEGMENTS = "#EXT-X-INDEPENDENT-SEGMENTS";

        const std::string EXTINF = "#EXTINF";

        const std::string EXT_X_ENDLIST = "#EXT-X-ENDLIST";
    public:
        M3U8Info() = default;
        ~M3U8Info() = default;
        M3U8Info(const std::string &file_path);
        // 解析m3u8文件
        bool parse();
        // 重写m3u8文件
        bool write();
        // 获取解析后的m3u8元数据
        std::vector<std::string> &lines();
        // 获取解析后的m3u8分片信息
        std::vector<std::pair<std::string, std::string>> &pieces();
    private:
        std::string _file_path;
        // 解析后的m3u8元数据
        std::vector<std::string> _head;
        // 解析后的m3u8分片信息
        std::vector<std::pair<std::string, std::string>> _pieces;
    };
}



