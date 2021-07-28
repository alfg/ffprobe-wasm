module.exports = {
  pages: {
    index: {
      entry: 'src/main.js',
      title: 'FFProbe',
    }
  },
  configureWebpack: {
    devServer: {
      headers: {
        'Cross-Origin-Opener-Policy': 'same-origin',
        'Cross-Origin-Embedder-Policy': 'require-corp',
      }
    }
  }, 
};
