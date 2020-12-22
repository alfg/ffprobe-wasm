onmessage = (e) => {
    const file = e.data[0];

    // Mount FS for files.
    if (!FS.analyzePath('/work').exists) {
        FS.mkdir('/work');
    }
    FS.mount(WORKERFS, { files: [file] }, '/work');

    // Call the wasm module.
    const info = Module.get_file_info('/work/' + file.name);

    // Remap streams into collection.
    const s = [];
    for (let i = 0; i < info.streams.size(); i++) {
        s.push(info.streams.get(i));
    }

    const versions = {
        libavutil:  Module.avutil_version(),
        libavcodec:  Module.avcodec_version(),
        libavformat:  Module.avformat_version(),
    };

    // Send back data response.
    const data = {
        ...info,
        streams: s,
        versions,
    }
    postMessage(data);

    // Cleanup mount.
    FS.unmount('/work');
}
self.importScripts('ffprobe-wasm.js'); // Load ffprobe into worker context.