for arch in armeabi armeabi-v7a arm64-v8a x86 x86_64
do
    bash build_ffmpeg.sh $arch
done
