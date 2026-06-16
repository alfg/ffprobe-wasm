const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');

const modulePath = process.env.FFPROBE_MODULE || '../dist/ffprobe.js';
const createFFprobe = require(path.resolve(__dirname, modulePath));

async function main() {
  const inputPath = path.resolve(process.argv[2] || '../sample.mp4');
  const requireVideoDetails = process.env.REQUIRE_VIDEO_DETAILS === '1';
  const input = fs.readFileSync(inputPath);
  const legacy = await createFFprobe({ noInitialRun: true });

  assert.equal(typeof legacy.get_file_info, 'function');
  assert.equal(typeof legacy.get_frames, 'function');
  legacy.FS.writeFile('/legacy-input', input);

  const legacyInfo = legacy.get_file_info('/legacy-input');
  assert.ok(legacyInfo.streams.size() > 0);

  const output = [];
  const errors = [];
  const ffprobe = await createFFprobe({
    print: (line) => output.push(line),
    printErr: (line) => errors.push(line),
  });

  ffprobe.FS.writeFile('/input', input);
  const status = ffprobe.callMain([
    '-hide_banner',
    '-loglevel', 'fatal',
    '-show_format',
    '-show_streams',
    '-show_chapters',
    '-show_programs',
    '-show_stream_groups',
    '-show_private_data',
    '-print_format', 'json',
    '/input',
  ]);

  assert.equal(status, 0, errors.join('\n'));

  const raw = output.join('\n');
  const data = JSON.parse(raw);
  assert.ok(Array.isArray(data.programs));
  assert.ok(Array.isArray(data.stream_groups));
  assert.ok(Array.isArray(data.streams));
  assert.ok(Array.isArray(data.chapters));
  assert.ok(data.format);

  const video = data.streams.find((stream) => stream.codec_type === 'video');
  if (requireVideoDetails) {
    assert.ok(video, 'expected a video stream');
    for (const field of ['pix_fmt', 'profile', 'refs', 'color_range', 'coded_width', 'coded_height']) {
      assert.notEqual(video[field], undefined, `expected video stream field: ${field}`);
    }
  }

  console.log(JSON.stringify({
    format_name: data.format.format_name,
    programs: data.programs.length,
    stream_groups: data.stream_groups.length,
    streams: data.streams.length,
    chapters: data.chapters.length,
    raw_bytes: Buffer.byteLength(raw),
    legacy_streams: legacyInfo.streams.size(),
    libavformat: legacy.avformat_version(),
    video_fields: video ? {
      pix_fmt: video.pix_fmt,
      profile: video.profile,
      refs: video.refs,
      color_range: video.color_range,
      coded_width: video.coded_width,
      coded_height: video.coded_height,
    } : null,
  }, null, 2));
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
