module.exports = {
  pages: {
    index: {
      entry: 'src/main.js',
      title: 'FFMpeg WebTools',
    }
  },
  publicPath: process.env.NODE_ENV === 'production'
    ? '/ffprobe-web-analyzer/'
    : '/',
  configureWebpack: {
    devServer: {
      headers: {
        'Cross-Origin-Opener-Policy': 'same-origin',
        'Cross-Origin-Embedder-Policy': 'require-corp',
      }
    }
  }, 
};
