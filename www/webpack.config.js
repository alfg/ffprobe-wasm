const WasmPackPlugin = require("@wasm-tool/wasm-pack-plugin");

module.exports = {
  plugins: [

    new WasmPackPlugin({
      crateDirectory: path.resolve(__dirname, "crate"),
      args: "--log-level warn",
      extraArgs: "--no-typescript",
    }),

  ]
};