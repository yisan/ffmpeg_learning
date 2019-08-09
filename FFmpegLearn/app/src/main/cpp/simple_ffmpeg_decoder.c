//
// Created by ing on 2019/7/31.
//最简单的基于FFmpeg的视频解码器

#include <stdio.h>
#include <time.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"
#include "libavutil/imgutils.h"

#ifdef ANDROID

#include <jni.h>
#include <android/log.h>
#include <libavformat/avformat.h>

#define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR,"(>_<)",format,##__VA_ARGS__)
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO,"(^_^)",format,##__VA_ARGS__)
#else
#define LOGE(format, ...) printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...) printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif

//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}

JNIEXPORT jint JNICALL
Java_com_ing_ffmpeg_DecodecActivity_decode(JNIEnv *env, jobject obj, jstring input_jstr,
                                           jstring output_jstr) {
    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;

    clock_t time_start, time_finish;
    double time_duration = 0.0;

    char input_str[500] = {0};
    char output_str[500] = {0};
    char info[1000] = {0};
    sprintf(input_str, "%s", (*env)->GetStringUTFChars(env, input_jstr, NULL));
    sprintf(output_str, "%s", (*env)->GetStringUTFChars(env, output_jstr, NULL));

    av_log_set_callback(custom_log);
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    /**
     * 打开音视频文件 avformat_open_input 主要负责服务器的连接和码流头部信息的拉取
     * 函数读取媒体文件的文件头并将文件格式相关的信息存储AVFormatContext上下文中。
     * 第二参数 input_str 文件的路径
     * 第三参数 用于指定媒体文件格式
     * 第四参数 文件格式的相关选项
     * 后面两个参数如果传入的NULL，那么libavformat将自动探测文件格式
     **/
    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    /**
     *
     * 媒体信息的探测和分析
     * 函数会为pFormatCtx->streams填充对应的信息
     *
     *
     * AVFormatContext 里包含了下面这些跟媒体信息有关的成员：
----------AVFormatContext-------
struct AVInputFormat *iformat; // 记录了封装格式信息
unsigned int nb_streams; // 记录了该 URL 中包含有几路流
AVStream **streams; // 一个结构体数组，每个对象记录了一路流的详细信息
int64_t start_time; // 第一帧的时间戳
int64_t duration; // 码流的总时长
int64_t bit_rate; // 码流的总码率，bps
AVDictionary *metadata; // 一些文件信息头，key/value 字符串
pFormatCtx->streams是一个AVStream指针的数组，里面包含了媒体资源的每一路流信息，数组大小为pFromatCtx->nb_streams

--------AVStream---------
AVStream 结构体中关键的成员包括：

AVCodecContext *codec; // 记录了该码流的编码信息
int64_t start_time; // 第一帧的时间戳
int64_t duration; // 该码流的时长
int64_t nb_frames; // 该码流的总帧数
AVDictionary *metadata; // 一些文件信息头，key/value 字符串
AVRational avg_frame_rate; // 平均帧率

----------AVCodecContext---------
     AVCodecContext 则记录了一路流的具体编码信息，其中关键的成员包括：

const struct AVCodec *codec; // 编码的详细信息
enum AVCodecID codec_id; // 编码类型
int bit_rate; // 平均码率
video only：
int width, height; // 图像的宽高尺寸，码流中不一定存在该信息，会由解码后覆盖
enum AVPixelFormat pix_fmt; // 原始图像的格式，码流中不一定存在该信息，会由解码后覆盖
audio only：
int sample_rate; // 音频的采样率
int channels; // 音频的通道数
enum AVSampleFormat sample_fmt; // 音频的格式，位宽
int frame_size; // 每个音频帧的 sample 个数

     */
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]/*音视频流*/->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)//查找视频流
        {
            videoindex = i;
            break;
        }

    }
    if (videoindex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }
    /**
     * pCodecCtx = pFormatCtx->streams[videoindex]->codec;//指向AVCodecContext的指针 #已废弃，不赞成使用。
     */
    pCodecCtx = avcodec_alloc_context3(NULL);
    if (pCodecCtx == NULL) {
        printf("Could not allocate AVCodecContext\n");
        return -1;
    }
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//指向AVCodec的指针，查找解码器
    if (pCodec == NULL) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }

    /**------存储数据 存储视频的帧 并转化格式------**/
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    //当转换格式时，我们需要一块内存来存储视频帧的原始数据。av_image_get_buffer_size来获取需要的内存大小，然后手动分配这块内存。
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    //关联frame和我们刚才分配的内存---存储视频帧的原始数据
    // 为已经分配空间的结构体AVPicture挂上一段用于保存数据的空间
    // AVFrame/AVPicture有一个data[4]的数据字段,buffer里面存放的只是yuv这样排列的数据，
    // 而经过fill 之后，会把buffer中的yuv分别放到data[0],data[1],data[2]中。
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1);

    /**----------------读取数据----------------**/
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //初始化一个SwsContext 图形裁剪
    //参数 源图像的宽，源图像的高，源图像的像素格式，目标图像的宽，目标图像的高，目标图像的像素格式，设定图像拉伸使用的算法
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    sprintf(info, "[Input ]$s\n", input_str);
    sprintf(info, "%s[Output ]%s\n", info, output_str);
    sprintf(info, "%s[Format ]%s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec ]%s]\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution ]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);


    fp_yuv = fopen(output_str, "wb+");
    if (fp_yuv == NULL) {
        printf("Cannot open output file.\n");
        return -1;
    }
    frame_cnt = 0;
    time_start = clock();

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoindex) {
            //解码一帧视频数据，输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame
            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret < 0) {
                LOGE("Decode Error.\n");
                return -1;
            }

            got_picture = avcodec_receive_frame(pCodecCtx, pFrame);
            if (got_picture) {
                //转换像素
                //解码后yuv格式的视频像素数据保存在AVFrame的data[0]、data[1]、data[2]中。
                // 但是这些像素值并不是连续存储的，每行有效像素之后存储了一些无效像素
                // ，以高度Y数据为例，data[0]中一共包含了linesize[0]*height个数据。
                // 但是出于优化等方面的考虑，linesize[0]实际上并不等于宽度width,而是一个比宽度大一些的值。
                // 因此需要使用ses_scale()进行转换。转换后去除了无效数据，width和linesize[0]就取值相同了

                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0,
                          pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                y_size = pCodecCtx->width * pCodecCtx->height;
                //向文件写入一个数据块
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
                //Output info
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        sprintf(pictype_str, "I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        sprintf(pictype_str, "p");
                        break;
                    case AV_PICTURE_TYPE_B:
                        sprintf(pictype_str, "B");
                        break;
                    default:
                        sprintf(pictype_str, "Other");
                        break;
                }
                LOGI("Frame Index : %5d.Type:%s", frame_cnt, pictype_str);
                frame_cnt++;
            }

        }
//        av_free_packet(packet);已废弃
        av_packet_unref(packet);
    }
    //flush_decoder
    //当av_read_frame()循环退出时，实际上解码器中可能还包含剩余的几帧数据，因此需要通过flush_decoder将这几帧数据输出。
    //flush_decoder功能简而言之即直接调用avcodec_send_packet()获得AVFrame,而不再向解码器传递AVPacket
    while (1) {
        ret = avcodec_send_packet(pCodecCtx, packet);
        if (ret < 0) {
            break;
        }
        if (!got_picture) {
            break;
        }
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);//y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);//u
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);//v
        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                break;
            case AV_PICTURE_TYPE_P:
                sprintf(pictype_str, "p");
                break;
            case AV_PICTURE_TYPE_B:
                sprintf(pictype_str, "B");
                break;
            default:
                sprintf(pictype_str, "Other");
                break;
        }
        LOGI("Frame Index:%5d. Type:%s", frame_cnt, pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double) (time_finish - time_start);
    sprintf(info, "%s[Time  ]%fms\n", info, time_duration);
    sprintf(info, "%s[Count ]%d\n", info, frame_cnt);
    fclose(fp_yuv);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    LOGI("%s", "解码完成");

    return 0;
}