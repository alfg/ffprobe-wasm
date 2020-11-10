module.exports = {
  pages: {
    index: {
      entry: 'src/main.js',
      title: 'FFProbe',
    }
  },
  publicPath: process.env.NODE_ENV === 'production'
    ? '/ffprobe-wasm/'
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
