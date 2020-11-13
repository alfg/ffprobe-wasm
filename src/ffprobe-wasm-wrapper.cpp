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

typedef struct Frame {
  int frame_number;
  char pict_type;
  int pts;
  int dts;
  int pkt_size;
} Frame;

typedef struct FileInfoResponse {
  std::string name;
  int bit_rate;
  int duration;
  std::string url;
  int nb_streams;
  int flags;
  std::vector<Stream> streams;
} FileInfoResponse;

typedef struct FramesResponse {
  std::vector<Frame> frames;
  int nb_frames;
} FramesResponse;

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

    // Loop through the streams.
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

FramesResponse get_frames(int offset) {
    av_log_set_level(AV_LOG_QUIET); // No logging output for libav.

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

    // Get streams data.
    AVCodec  *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;
    int video_stream_index = -1;
    int nb_frames = 0;

    // Loop through the streams.
    for (int i = 0; i < pFormatContext->nb_streams; i++) {
      AVCodecParameters *pLocalCodecParameters = NULL;
      pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

      // Print out the decoded frame info.
      AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
      if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (video_stream_index == -1) {
          video_stream_index = i;
          nb_frames = pFormatContext->streams[i]->nb_frames;
          pCodec = pLocalCodec;
          pCodecParameters = pLocalCodecParameters;
        }

        printf("Video Codec: resolution %d x %d\n",
          pLocalCodecParameters->width, pLocalCodecParameters->height);
      } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
        printf("Audio Codec: %d channels, sample rate %d\n",
          pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
      }
    }

    FramesResponse r;
    r.nb_frames = nb_frames;

    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    avcodec_open2(pCodecContext, pCodec, NULL);

    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();

    int how_many_packets_to_process = 48; // per page.
    int frame_count = 1;

    // Read video frames.
    while (av_read_frame(pFormatContext, pPacket) >= 0) {
      if (pPacket->stream_index == video_stream_index) {
        if (frame_count >= offset) {

          int response = 0;
          response = avcodec_send_packet(pCodecContext, pPacket);

          if (response >= 0) {
            response = avcodec_receive_frame(pCodecContext, pFrame);
            if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
              continue;
            }

            Frame f = {
              .frame_number = frame_count,
              .pict_type = (char) av_get_picture_type_char(pFrame->pict_type),
              .pts = (int) pPacket->pts,
              .dts = (int) pPacket->dts,
              .pkt_size = pFrame->pkt_size,
            };
            r.frames.push_back(f);

            if (--how_many_packets_to_process <= 0) break;
          }
        }
        frame_count++;
      }
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

  emscripten::value_object<Frame>("Frame")
  .field("frame_number", &Frame::frame_number)
  .field("pict_type", &Frame::pict_type)
  .field("pts", &Frame::pts)
  .field("dts", &Frame::dts)
  .field("pkt_size", &Frame::pkt_size);
  register_vector<Frame>("Frame");

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

  emscripten::value_object<FramesResponse>("FramesResponse")
  .field("frames", &FramesResponse::frames)
  .field("nb_frames", &FramesResponse::nb_frames)
  ;
  function("get_frames", &get_frames);
}