#include <vector>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

const std::string c_avformat_version() {
    return AV_STRINGIFY(LIBAVFORMAT_VERSION);
}

struct Stream {
  int id;
  int start_time;
  int duration;
  int codec_type;
};
typedef struct Stream Stream;

struct FileInfoResponse {
  std::string name;
  int bit_rate;
  int duration;
  std::string url;
  int nb_streams;
  int flags;
  std::vector<Stream> streams;
};
typedef struct FileInfoResponse FileInfoResponse;

FileInfoResponse get_file_info() {
    FILE *file = fopen("file", "rb");
    if (!file) {
      printf("cannot open file\n");
    }
    fclose(file);

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
      printf("ERROR: could not allocate memory for Format Context\n");
    }

    // Open the file and read header.
    int ret;
    if ((ret = avformat_open_input(&pFormatContext, "file", NULL, NULL)) < 0) {
        printf("ERROR: could not open the file. Error: %d\n", ret);
        printf("%s", av_err2str(ret));
    }

    // Initialize response struct with format data.
    FileInfoResponse r = {
      .name = pFormatContext->iformat->name,
      .bit_rate = (int)pFormatContext->bit_rate,
      .duration = (int)pFormatContext->duration,
      .url = pFormatContext->url,
      .nb_streams = (int)pFormatContext->nb_streams,
      .flags = pFormatContext->flags
    };

    // Get streams data.
    AVCodec  *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;
    int video_stream_index = -1;

    // Loop through the streams and print its information.
    for (int i = 0; i < pFormatContext->nb_streams; i++) {
      AVCodecParameters *pLocalCodecParameters = NULL;
      pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
      Stream s = {
        .id = (int)pFormatContext->streams[i]->id,
        .start_time = (int)pFormatContext->streams[i]->start_time,
        .duration = (int)pFormatContext->streams[i]->duration,
        .codec_type = (int)pLocalCodecParameters->codec_type
      };
      r.streams.push_back(s);
    }
    return r;
}


EMSCRIPTEN_BINDINGS(my_constant_example) {
    function("c_avformat_version", &c_avformat_version);
}

EMSCRIPTEN_BINDINGS(FileInfoResponse_struct) {
  emscripten::value_object<Stream>("Stream")
  .field("id", &Stream::id)
  .field("start_time", &Stream::start_time)
  .field("duration", &Stream::duration)
  .field("codec_type", &Stream::codec_type)
  ;
  register_vector<Stream>("Stream");
  emscripten::value_object<FileInfoResponse>("FileInfoResponse")
  .field("name", &FileInfoResponse::name)
  .field("duration", &FileInfoResponse::duration)
  .field("bit_rate", &FileInfoResponse::bit_rate)
  .field("url", &FileInfoResponse::url)
  .field("nb_streams", &FileInfoResponse::nb_streams)
  .field("flags", &FileInfoResponse::flags)
  .field("streams", &FileInfoResponse::streams)
  ;
  function("get_file_info", &get_file_info);
}