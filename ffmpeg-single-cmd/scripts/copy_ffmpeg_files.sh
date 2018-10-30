
FFMPEG_DIR=ffmpeg-3.3.8


mkdir include
mkdir include/compat
mkdir include/libavcodec
mkdir include/libavformat
mkdir include/libavutil
mkdir include/libavdevice
mkdir include/libpostproc

cp $FFMPEG_DIR/compat/va_copy.h include/compat/va_copy.h

cp $FFMPEG_DIR/libavcodec/mathops.h include/libavcodec/mathops.h

cp $FFMPEG_DIR/libavdevice/avdevice.h include/libavdevice/avdevice.h
cp $FFMPEG_DIR/libavdevice/version.h include/libavdevice/version.h

cp $FFMPEG_DIR/libavformat/ffm.h include/libavformat/ffm.h
cp $FFMPEG_DIR/libavformat/network.h include/libavformat/network.h
cp $FFMPEG_DIR/libavformat/os_support.h include/libavformat/os_support.h
cp $FFMPEG_DIR/libavformat/url.h include/libavformat/url.h


cp $FFMPEG_DIR/libavutil/libm.h include/libavutil/libm.h
cp $FFMPEG_DIR/libavutil/internal.h include/libavutil/internal.h
cp $FFMPEG_DIR/libavutil/reverse.h include/libavutil/reverse.h
cp $FFMPEG_DIR/libavutil/timer.h include/libavutil/timer.h


cp $FFMPEG_DIR/libpostproc/postprocess.h include/libpostproc/postprocess.h
cp $FFMPEG_DIR/libpostproc/version.h include/libpostproc/version.h



#cpp
mkdir cpp

cp $FFMPEG_DIR/cmdutils.c cpp/cmdutils.c
cp $FFMPEG_DIR/cmdutils.h cpp/cmdutils.h
cp $FFMPEG_DIR/cmdutils_common_opts.h cpp/cmdutils_common_opts.h

#
cp $FFMPEG_DIR/config.h cpp/config.h

cp $FFMPEG_DIR/ffmpeg.h cpp/ffmpeg.h
cp $FFMPEG_DIR/ffmpeg.c cpp/ffmpeg_mod.c
cp $FFMPEG_DIR/ffmpeg_filter.c cpp/ffmpeg_filter.c
cp $FFMPEG_DIR/ffmpeg_opt.c cpp/ffmpeg_opt.c

