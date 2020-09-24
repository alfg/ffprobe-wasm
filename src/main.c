#include <stdio.h>
#include <math.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>


void version() {
    printf("%s\n", AV_STRINGIFY(LIBAVFORMAT_VERSION));
}

void openfile() {
    FILE *file = fopen("testingfs", "rb");
    if (!file) {
      printf("cannot open file\n");
    }
    fclose(file);

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
      printf("ERROR: could not allocate memory for Format Context\n");
    }

    printf("opening the input file: %s and loading format (container) header\n", "testingfs");

    // Open the file and read header.
    int ret;
    if ((ret = avformat_open_input(&pFormatContext, "testingfs", NULL, NULL)) < 0) {
        printf("ERROR: could not open the file. Error: %d\n", ret);
        printf("%s", av_err2str(ret));
    }

    printf("format: %s, duration: %lld us, bit_rate: %lld\n",
      pFormatContext->iformat->name,
      pFormatContext->duration,
      pFormatContext->bit_rate);
}