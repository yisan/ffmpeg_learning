#include <jni.h>
#include <string.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"

//
//jstring Java_com_ing_ffmpeg_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject thiz) {
//    char info[10000] = {0};
//    sprintf(info, "%s\n", avcodec_configuration());
//    return (*env)->NewStringUTF(env, info);
//}

struct URLProtocol;
/**
 * 利用avio_enum_protocols迭代libavformat/protocol_list.c中的url_protocols数组，输出ffmpeg支持的IO协议
 */
JNIEXPORT jstring  Java_com_ing_ffmpeg_MainActivity_urlprotocolinfo(JNIEnv *env, jobject obj) {
    char info[40000] = {0}; //全部初始化为0
    avcodec_register_all();//注册所有的编解码器
    struct URLProtocol *pup = NULL;
    //Input
    struct URLProtocol **p_temp = &pup;
    avio_enum_protocols((void **) p_temp, 0);
    while ((*p_temp) != NULL) {
        sprintf(info, "%s[In ][%10s\n]", info, avio_enum_protocols((void **) p_temp, 0));
    }
    pup = NULL;
    avio_enum_protocols((void **) p_temp, 1);
    while ((*p_temp) != NULL) {
        sprintf(info, "%s[Out][%10s]\n", info, avio_enum_protocols((void **) p_temp, 1));
    }
    return (*env)->NewStringUTF(env, info);
}


/**
 * 输出ffmpeg支持的格式
 */
JNIEXPORT jstring Java_com_ing_ffmpeg_MainActivity_avformatinfo(JNIEnv *env, jobject obj) {
    char info[40000] = {0};
    av_register_all();
    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    //Input
    while (if_temp != NULL) {
        sprintf(info, "%s[In ][%10s]\n", info, if_temp->name);
        if_temp = if_temp->next;
    }
    //Output
    while (of_temp != NULL) {
        sprintf(info, "%s[Out ][%10s]\n", info, of_temp->name);
        of_temp = of_temp->next;
    }
    //LOGE("%s",info);
    return (*env)->NewStringUTF(env, info);
}

/**
 * 支持的编码器信息
 */
JNIEXPORT jstring Java_com_ing_ffmpeg_MainActivity_avcodecinfo(JNIEnv *env, jobject obj) {
    char info[40000] = {0};
    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%s[Dec]", info);
        } else {
            sprintf(info, "%s[Enc]", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s[Video]", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s[Audio]", info);
                break;
            default:
                sprintf(info, "%s[Other]", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);

        c_temp = c_temp->next;
    }
    return (*env)->NewStringUTF(env, info);
}
/**
 * 支持的滤波器
 */
JNIEXPORT jstring Java_com_ing_ffmpeg_MainActivity_avfilterinfo(JNIEnv *env, jobject obj) {
    char info[40000] = {0};
    avfilter_register_all();
    AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
    while (f_temp != NULL) {
        sprintf(info, "%s[%10s]\n", info, f_temp->name);
        f_temp = f_temp->next;
    }
    return (*env)->NewStringUTF(env, info);
}
/**
 * configure信息
 */
JNIEXPORT jstring Java_com_ing_ffmpeg_MainActivity_configurationinfo(JNIEnv *env, jobject obj) {
    char info[40000] = {0};
    av_register_all();
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}