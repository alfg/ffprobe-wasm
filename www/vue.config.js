module.exports = {
  pages: {
    index: {
      entry: 'src/main.js',
      title: 'FFProbe Web Analyzer',
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
