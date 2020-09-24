module.exports = {
  pages: {
    index: {
      entry: 'src/main.js',
      title: 'FFMpeg WebTools',
    }
  },
  publicPath: process.env.NODE_ENV === 'production'
    ? '/ffmpeg-webtools/'
    : '/',
};
