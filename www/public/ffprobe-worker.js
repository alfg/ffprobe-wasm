importScripts('ffprobe.js');

const legacyModulePromise = createFFprobe({ noInitialRun: true });

async function getRawInfo(file, outputType) {
    const output = [];
    const errors = [];
    const ffprobe = await createFFprobe({
        noInitialRun: true,
        print: (line) => output.push(line),
        printErr: (line) => errors.push(line),
    });

    if (!ffprobe.FS.analyzePath('/work').exists) {
        ffprobe.FS.mkdir('/work');
    }

    ffprobe.FS.mount(ffprobe.FS.filesystems.WORKERFS, { files: [file] }, '/work');

    try {
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
            '/work/' + file.name,
        ]);
        if (status !== 0) {
            throw new Error(errors.join('\n') || `ffprobe exited with status ${status}`);
        }
    } catch (error) {
        if (!error || error.status !== 0) {
            throw error;
        }
    } finally {
        ffprobe.FS.unmount('/work');
    }

    const raw = output.join('\n') + '\n';
    if (!raw) {
        throw new Error(errors.join('\n') || 'ffprobe produced no output');
    }

    if (outputType === 'string') {
        return raw;
    }
    if (outputType === 'json' || outputType === undefined) {
        return JSON.parse(raw);
    }
    throw new Error("Output type must be 'string' or 'json'");
}

onmessage = async (e) => {
    const type = e.data[0];
    const file = e.data[1];

    let data;

    switch (type) {
        case 'get_file_info':
            {
            const Module = await legacyModulePromise;
            const FS = Module.FS;

            // Mount FS for files.
            if (!FS.analyzePath('/work').exists) {
                FS.mkdir('/work');
            }
            FS.mount(FS.filesystems.WORKERFS, { files: [file] }, '/work');

            // Call the wasm module.
            const info = Module.get_file_info('/work/' + file.name);

            // Remap streams into collection.
            const s = [];
            for (let i = 0; i < info.streams.size(); i++) {
                const tags = {};
                for (let j = 0; j < info.streams.get(i).tags.size(); j++) {
                    const t = info.streams.get(i).tags.get(j);
                    tags[t.key] = t.value;
                }
                s.push({...info.streams.get(i), ...{ tags}});
            }

            // Remap chapters into collection.
            const c = [];
            for (let i = 0; i < info.chapters.size(); i++) {
                const tags = {};
                for (let j = 0; j < info.chapters.get(i).tags.size(); j++) {
                    const t = info.chapters.get(i).tags.get(j);
                    tags[t.key] = t.value;
                }
                c.push({...info.chapters.get(i), ...{tags}});
            }

            const versions = {
                libavutil:  Module.avutil_version(),
                libavcodec:  Module.avcodec_version(),
                libavformat:  Module.avformat_version(),
            };

            // Send back data response.
            data = {
                ...info,
                streams: s,
                chapters: c,
                versions,
            }
            postMessage(data);

            // Cleanup mount.
            FS.unmount('/work');
            break;
            }
        
        case 'get_frames':
            {
            const Module = await legacyModulePromise;
            const FS = Module.FS;

            if (!FS.analyzePath('/work').exists) {
                FS.mkdir('/work');
            }
            FS.mount(FS.filesystems.WORKERFS, { files: [file] }, '/work');

            const offset = e.data[2];
            const frames = Module.get_frames('/work/' + file.name, offset);

            // Remap frames into collection.
            const f = [];
            for (let i = 0; i < frames.frames.size(); i++) {
                f.push(frames.frames.get(i));
            }

            data = {
                ...frames,
                frames: f,
            }
            postMessage(data);

            // Cleanup mount.
            FS.unmount('/work');
            break;
            }

        case 'get_raw_info':
            try {
                data = await getRawInfo(file, e.data[2]);
                postMessage(data);
            } catch (error) {
                postMessage({
                    error: error instanceof Error ? error.message : String(error),
                });
            }
            break;
    
        default:
            break;
    }

}
