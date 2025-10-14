#include "ffmpeg.h"

namespace bubble
{
    TransCoder::TransCoder(const TransCoderSettings &settings)
        : _settings(settings)
    {}
    // 转码
    bool TransCoder::transCode()
    {
    
        // 1. 打开输入文件，并初始化输入格式化对象
        AVFormatContext *input_format_ctx = nullptr;
        // 申请输出格式化对象
        AVFormatContext *output_format_ctx = nullptr;
        int ret = avformat_open_input(&input_format_ctx, _settings.input_file.c_str(), nullptr, nullptr);
        if (ret < 0)
        {
            ERROR__LOG("打开输入文件失败: {}", av_err_str(ret));
            return false;
        }
        // 2. 查找输入视频参数
        ret = avformat_find_stream_info(input_format_ctx, nullptr);
        if (ret < 0)
        {
            ERROR__LOG("查找输入视频参数失败: {}", av_err_str(ret));
            return false;
        }
        // 3. 申请输出格式化对象
        ret = avformat_alloc_output_context2(&output_format_ctx, nullptr, "hls", _settings.output_file.c_str());
        if (ret < 0)
        {
            ERROR__LOG("申请输出格式化对象失败: {}", av_err_str(ret));
            return false;
        }
        for (int i = 0; i < input_format_ctx->nb_streams; i++)
        {
            // 4.a 为输出对象申请媒体流
            AVStream *in_stream = input_format_ctx->streams[i];
            AVStream *out_stream = avformat_new_stream(output_format_ctx, nullptr);
            if (!out_stream)
            {
                ERROR__LOG("为输出对象申请媒体流失败");
                return false;
            }
            // 4.b 从输入媒体流复制解码器参数到输出媒体流中
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0)
            {
                ERROR__LOG("从输入媒体流复制解码器参数到输出媒体流中失败: {}", av_err_str(ret));
                return false;
            }
            out_stream->avg_frame_rate = in_stream->avg_frame_rate;
            out_stream->r_frame_rate = in_stream->r_frame_rate;
        }
        // 5. 设置分片字典选项
        AVDictionary *options = nullptr;
        av_dict_set(&options, "hls_time", std::to_string(_settings.hls_time).c_str(), 0);                               // 每个分片的时长，单位秒
        av_dict_set(&options, "hls_playlist_type", "vod", 0);                     // 点播类型
        av_dict_set(&options, "hls_flags", "independent_segments", 0);            // 每个分片都是独立的
        av_dict_set(&options, "hls_base_url", _settings.hls_base_url.c_str(), 0); // 分片的基地址
        // 6. 通过输出格式化对象，输出媒体头部信息
        ret = avformat_write_header(output_format_ctx, &options);
        if (ret < 0)
        {
            ERROR__LOG("通过输出格式化对象，输出媒体头部信息失败: {}", av_err_str(ret));
            return false;
        }
        // 7. 遍历输入流中的数据帧
        AVPacket packet;
        while (av_read_frame(input_format_ctx, &packet) >= 0)
        {
            AVStream *inputStream = input_format_ctx->streams[packet.stream_index];
            AVStream *outputStream = output_format_ctx->streams[packet.stream_index];
            // 7.a 将数据包中的时间戳，从输入流的时间基转换为输出流的时间基的时间戳
            // 1️⃣ 调整 PTS/DTS 到输出时间基
            if (packet.pts != AV_NOPTS_VALUE)
                packet.pts = av_rescale_q_rnd(packet.pts,
                                            inputStream->time_base,
                                            outputStream->time_base,
                                            (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            if (packet.dts != AV_NOPTS_VALUE)
                packet.dts = av_rescale_q_rnd(packet.dts,
                                            inputStream->time_base,
                                            outputStream->time_base,
                                            (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            if (packet.duration > 0)
                packet.duration = av_rescale_q(packet.duration,
                                            inputStream->time_base,
                                            outputStream->time_base);

            packet.pos = -1; // 不保留输入文件偏移

            // 7.b 将数据帧写入输出格式化对象中
            ret = av_interleaved_write_frame(output_format_ctx, &packet);
            if (ret < 0)
            {
                ERROR__LOG("将数据帧写入输出格式化对象中失败: {}", av_err_str(ret));
                return false;
            }
            // 7.c 释放帧结构资源
            av_packet_unref(&packet);
        }
        // 8. 通过输出格式化对象，输出媒体尾部信息
        ret = av_write_trailer(output_format_ctx);
        if (ret < 0)
        {
            ERROR__LOG("通过输出格式化对象，输出媒体尾部信息失败: {}", av_err_str(ret));
            return false;
        }
        // 9. 释放资源：字典选项，输入格式化对象，输出格式化对象
        av_dict_free(&options);
        avformat_close_input(&input_format_ctx);
        avformat_free_context(output_format_ctx);
        return true; 
    }

    // 错误码转字符串
    std::string TransCoder::av_err_str(int errnum)
    {
        static thread_local char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
        av_strerror(errnum, buf, sizeof(buf));
        return std::string(buf);
    }

    M3U8Info::M3U8Info(const std::string &file_path) : _file_path(file_path) {}
    // 解析m3u8文件
    bool M3U8Info::parse()
    {
        std::ifstream ifs(_file_path, std::ios::in | std::ios::binary);
        if (!ifs.is_open())
        {
            ERROR__LOG("打开m3u8文件失败: {}", _file_path);
            return false;
        }
        // 读取整个文件内容
        std::string context;
        ifs.seekg(0, std::ios::end);
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        context.resize(size);
        ifs.read(&context[0], size);
        ifs.close();

        // 解析m3u8文件内容
        std::vector<std::string> lines;
        Util::split(context, lines);
        for(size_t i = 0; i < lines.size(); i++)
        {
            if(lines[i].find(EXTINF) != std::string::npos)
            {
                if(i + 1 < lines.size())
                {
                    _pieces.emplace_back(lines[i], lines[i + 1]);
                    i++; // 跳过下一行
                }
                else
                {
                    ERROR__LOG("m3u8文件格式异常: {}", _file_path);
                    return false;
                }
            }
            else if(lines[i].find(EXT_X_ENDLIST) != std::string::npos)
            {
                break;
            }
            else
            {
                _head.push_back(lines[i]);
            }
        }
        return true;
    }
    // 重写m3u8文件
    bool M3U8Info::write()
    {
        std::ofstream ofs(_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open())
        {
            ERROR__LOG("打开m3u8文件失败: {}", _file_path);
            return false;
        }
        // 写入m3u8文件内容
        for(const auto &line : _head)
        {
            ofs << line << std::endl;
        }
        for(const auto &piece : _pieces)
        {
            ofs << piece.first << std::endl;
            ofs << piece.second << std::endl;
        }
        ofs << EXT_X_ENDLIST << std::endl;
        ofs.close();
        return true;
    }
    
    // 获取解析后的m3u8元数据
    std::vector<std::string> &M3U8Info::lines()
    {
        return _head;
    }
    // 获取解析后的m3u8分片信息
    std::vector<std::pair<std::string, std::string>> &M3U8Info::pieces()
    {
        return _pieces;
    }
}