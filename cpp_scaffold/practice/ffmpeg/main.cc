/*
    将mp4文件转化为hls格式
    步骤:
        1. 打开输入文件，并初始化输入格式化对象
        2. 查找输入视频参数
        3. 申请输出格式化对象
        4. 遍历输入媒体流
            a. 为输出对象申请媒体流
            b. 从输入媒体流复制解码器参数到输出媒体流中
        5. 设置分片字典选项
        6. 通过输出格式化对象，输出媒体头部信息
        7. 遍历输入流中的数据帧
            a. 将数据包中的时间戳，从输入流的时间基转换为输出流的时间基的时间戳
                i. 若数据帧的显示时间戳无效(AV_NOPTS_VALUE)，则默认为从0开始的默认时间基
                ii. 将从0开始的默认时间基转换为输入流时间基，重新进行a操作
            b. 将数据帧写入输出格式化对象中
            c. 释放帧结构资源
        8. 通过输出格式化对象，输出媒体尾部信息
        9. 释放资源：字典选项，输入格式化对象，输出格式化对象

*/
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}
#include <string>
#include <iostream>

// 错误码转字符串
static std::string av_err_str(int errnum)
{
    char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(errnum, buf, sizeof(buf));
    return std::string(buf);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return -1;
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    // 1. 打开输入文件，并初始化输入格式化对象
    AVFormatContext *input_format_ctx = nullptr;
    // 申请输出格式化对象
    AVFormatContext *output_format_ctx = nullptr;
    int ret = avformat_open_input(&input_format_ctx, input_file.c_str(), nullptr, nullptr);
    if (ret < 0)
    {
        std::cout << "打开输入文件失败: " << av_err_str(ret) << std::endl;
        return -1;
    }
    // 2. 查找输入视频参数
    ret = avformat_find_stream_info(input_format_ctx, nullptr);
    if (ret < 0)
    {
        std::cout << "查找输入视频参数失败: " << av_err_str(ret) << std::endl;
        return -1;
    }
    // 3. 申请输出格式化对象
    ret = avformat_alloc_output_context2(&output_format_ctx, nullptr, "hls", output_file.c_str());
    if (ret < 0)
    {
        std::cout << "申请输出格式化对象失败: " << av_err_str(ret) << std::endl;
        return -1;
    }
    for (int i = 0; i < input_format_ctx->nb_streams; i++)
    {
        // 4.a 为输出对象申请媒体流
        AVStream *in_stream = input_format_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(output_format_ctx, nullptr);
        if (!out_stream)
        {
            std::cout << "为输出对象申请媒体流失败" << std::endl;
            return -1;
        }
        // 4.b 从输入媒体流复制解码器参数到输出媒体流中
        ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        if (ret < 0)
        {
            std::cout << "从输入媒体流复制解码器参数到输出媒体流中失败: " << av_err_str(ret) << std::endl;
            return -1;
        }
        out_stream->avg_frame_rate = in_stream->avg_frame_rate;
        out_stream->r_frame_rate = in_stream->r_frame_rate;
    }
    // 5. 设置分片字典选项
    AVDictionary *options = nullptr;
    av_dict_set(&options, "hls_time", "10", 0);                               // 每个分片的时长，单位秒
    av_dict_set(&options, "hls_playlist_type", "vod", 0);                     // 点播类型
    av_dict_set(&options, "hls_flags", "independent_segments", 0);            // 每个分片都是独立的
    av_dict_set(&options, "hls_base_url", "http://localhost:9000/video/", 0); // 分片的基地址
    // 6. 通过输出格式化对象，输出媒体头部信息
    ret = avformat_write_header(output_format_ctx, &options);
    if (ret < 0)
    {
        std::cout << "通过输出格式化对象，输出媒体头部信息失败: " << av_err_str(ret) << std::endl;
        return -1;
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
            std::cout << "将数据帧写入输出格式化对象中失败: " << av_err_str(ret) << std::endl;
            return -1;
        }
        // 7.c 释放帧结构资源
        av_packet_unref(&packet);
    }
    // 8. 通过输出格式化对象，输出媒体尾部信息
    ret = av_write_trailer(output_format_ctx);
    if (ret < 0)
    {
        std::cout << "通过输出格式化对象，输出媒体尾部信息失败: " << av_err_str(ret) << std::endl;
        return -1;
    }
    // 9. 释放资源：字典选项，输入格式化对象，输出格式化对象
    av_dict_free(&options);
    avformat_close_input(&input_format_ctx);
    avformat_free_context(output_format_ctx);
    return 0;
}