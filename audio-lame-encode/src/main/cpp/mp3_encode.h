
#include <stdio.h>
#include "include/lame.h"

class Mp3Encoder {

private:
    FILE *pcmFile;
    FILE *mp3File;

    lame_t lameClient;

public:

    Mp3Encoder();

    ~Mp3Encoder();

    int Init(const char *pcmPath, const char *mp3Path, int sampleRate, int channels, int bitRate);

    void Encode();

    void Release();

};

