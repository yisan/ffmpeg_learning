# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
# 用来指定 CMake 最低版本为3.4.1，如果没指定，执行 cmake 命令时可能会出错
cmake_minimum_required(VERSION 3.4.1)

# 添加在native层log库
find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

set(distribution_DIR ${CMAKE_SOURCE_DIR}/libs)
include_directories(libs/include)

# avutil
add_library( avutil-55
        SHARED
        IMPORTED )
set_target_properties( avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libavutil-55.so )

# avdevice
add_library( avdevice-57
        SHARED
        IMPORTED )
set_target_properties( avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libavdevice-57.so )
# swresample
add_library( swresample-2
        SHARED
        IMPORTED )
set_target_properties( swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libswresample-2.so )

# avcodec
add_library( avcodec-57
        SHARED
        IMPORTED )
set_target_properties( avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libavcodec-57.so )

# avfilter
add_library( avfilter-6
        SHARED
        IMPORTED)
set_target_properties( avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libavfilter-6.so )

# swscale
add_library( swscale-4
        SHARED
        IMPORTED)
set_target_properties( swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libswscale-4.so )

# avformat
add_library( avformat-57
        SHARED
        IMPORTED)
set_target_properties( avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libavformat-57.so )
# postproc
add_library( postproc-54
        SHARED
        IMPORTED)
set_target_properties( postproc-54
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/armeabi-v7a/libpostproc-54.so )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library( native-lib
        SHARED
        src/main/cpp/native-lib.c
        # TODO 我们之后自己编写的cpp文件都会添加在这里，比如
        src/main/cpp/simple_ffmpeg_decoder.c

        )

target_link_libraries( # Specifies the target library.
        native-lib
        # 这里需要注意下,下面这些ffmpeg的so库编译是有先后顺序的
        # 下面的顺序是没问题的,如果想验证编译顺序,可以自己变换顺序试试.
        avcodec-57
        avutil-55
        avdevice-57
        swresample-2
        avfilter-6
        swscale-4
        avformat-57
        postproc-54
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )