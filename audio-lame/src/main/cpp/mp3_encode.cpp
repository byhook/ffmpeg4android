

#include "mp3_encode.h"


Mp3Encoder::Mp3Encoder() {
    pcmFile = NULL;
    mp3File = NULL;
}

Mp3Encoder::~Mp3Encoder() {

}

int Mp3Encoder::Init(const char *pcmPath, const char *mp3Path,
                     int sampleRate,
                     int channels,
                     int bitRate) {
    int result = -1;
    pcmFile = fopen(pcmPath, "rb");
    if (NULL != pcmFile) {
        mp3File = fopen(mp3Path, "wb");
        if (NULL != mp3File) {
            lameClient = lame_init();
            //设置采样率
            lame_set_in_samplerate(lameClient, sampleRate);
            lame_set_out_samplerate(lameClient, sampleRate);

            lame_set_num_channels(lameClient, channels);
            lame_set_brate(lameClient, bitRate / 1000);

            lame_set_quality(lameClient, 2);
            //初始化参数
            lame_init_params(lameClient);
            result = 0;
        }
    }
    return result;
}

void Mp3Encoder::Encode() {
    int bufferSize = 1024 * 256;
    short *buffer = new short[bufferSize / 2];
    short *leftBuffer = new short[bufferSize / 4];
    short *rightBuffer = new short[bufferSize / 4];
    unsigned char *mp3_buffer = new unsigned char[bufferSize];
    size_t readBufferSize = 0;
    while ((readBufferSize = fread(buffer, 2, bufferSize / 2, pcmFile)) > 0) {
        for (int i = 0; i < readBufferSize; i++) {
            if (i % 2 == 0) {
                leftBuffer[i / 2] = buffer[i];
            } else {
                rightBuffer[i / 2] = buffer[i];
            }
        }
        size_t wroteSize = lame_encode_buffer(lameClient, leftBuffer, rightBuffer,
                                              (int) (readBufferSize / 2), mp3_buffer, bufferSize);
        fwrite(mp3_buffer, 1, wroteSize, mp3File);
    }
    delete[] buffer;
    delete[] leftBuffer;
    delete[] rightBuffer;
    delete[] mp3_buffer;
}

void Mp3Encoder::Release() {
    if (pcmFile) {
        fclose(pcmFile);
    }
    if (mp3File) {
        fclose(mp3File);
        lame_close(lameClient);
    }
}


