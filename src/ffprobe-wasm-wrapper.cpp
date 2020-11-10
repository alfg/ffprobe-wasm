#include <vector>
#include <string>
#include <inttypes.h>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
};

const std::string c_avformat_version() {
    return AV_STRINGIFY(LIBAVFORMAT_VERSION);
}

typedef struct Stream {
  int id;
  int start_time;
  int duration;
  int codec_type;
  std::string codec_name;
  std::string format;
  int bit_rate;
  std::string profile;
  int level;
  int width;
  int height;
  int channels;
  int sample_rate;
  int frame_size;
} Stream;

typedef struct FileInfoResponse {
  std::string name;
  int bit_rate;
  int duration;
  std::string url;
  int nb_streams;
  int flags;
  std::vector<Stream> streams;
} FileInfoResponse;

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

    // Get stream info from format.
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
      printf("ERROR: could not get stream info\n");
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

      // Convert to char byte array.
      uint32_t n = pLocalCodecParameters->codec_tag;
      char fourcc[5];
      for (int j = 0; j < 4; ++j) {
        fourcc[j] = (n >> (j * 8) & 0xFF);
      }
      fourcc[4] = 0x00; // NULL terminator.

      Stream stream = {
        .id = (int)pFormatContext->streams[i]->id,
        .start_time = (int)pFormatContext->streams[i]->start_time,
        .duration = (int)pFormatContext->streams[i]->duration,
        .codec_type = (int)pLocalCodecParameters->codec_type,
        .codec_name = fourcc,
        .format = av_get_pix_fmt_name((AVPixelFormat)pLocalCodecParameters->format),
        .bit_rate = (int)pLocalCodecParameters->bit_rate,
        .profile = avcodec_profile_name(pLocalCodecParameters->codec_id, pLocalCodecParameters->profile),
        .level = (int)pLocalCodecParameters->level,
        .width = (int)pLocalCodecParameters->width,
        .height = (int)pLocalCodecParameters->height,
        .channels = (int)pLocalCodecParameters->channels,
        .sample_rate = (int)pLocalCodecParameters->sample_rate,
        .frame_size = (int)pLocalCodecParameters->frame_size,
      };
      r.streams.push_back(stream);
      free(fourcc);
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
  .field("codec_name", &Stream::codec_name)
  .field("format", &Stream::format)
  .field("bit_rate", &Stream::bit_rate)
  .field("profile", &Stream::profile)
  .field("level", &Stream::level)
  .field("width", &Stream::width)
  .field("height", &Stream::height)
  .field("channels", &Stream::channels)
  .field("sample_rate", &Stream::sample_rate)
  .field("frame_size", &Stream::frame_size)
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