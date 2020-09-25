#include <emscripten.h>
#include <emscripten/bind.h>


using namespace emscripten;


extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>


const std::string c_avformat_version() {
    return AV_STRINGIFY(LIBAVFORMAT_VERSION);
}

struct Response {
  std::string name;
  int bit_rate;
  int duration;
};

Response c_openfile() {
    FILE *file = fopen("testingfs", "rb");
    if (!file) {
      printf("cannot open file\n");
    }
    fclose(file);

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
      printf("ERROR: could not allocate memory for Format Context\n");
    }
    printf("%p\n", pFormatContext);

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

    Response r;
    r.name = pFormatContext->iformat->name;
    r.duration = pFormatContext->duration;
    r.bit_rate = pFormatContext->bit_rate;
    return r;
}
};


EMSCRIPTEN_BINDINGS(my_constant_example) {
    function("c_avformat_version", &c_avformat_version);
}

EMSCRIPTEN_BINDINGS(a_struct) {
  emscripten::value_object<Response>("Response")
  .field("name", &Response::name)
  .field("duration", &Response::duration)
  .field("bit_rate", &Response::bit_rate)
  ;
  function("c_openfile", &c_openfile);
}