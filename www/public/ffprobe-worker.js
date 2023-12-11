onmessage = (message) => {
  const [type, file] = message.data
  const [origin] = message.ports

  switch (type) {
  case 'get_file_info': {
    // Mount FS for files.
    if (!FS.analyzePath('/work').exists) {
      FS.mkdir('/work')
    }
    FS.mount(WORKERFS, { files: [file] }, '/work')

    // Call the wasm module.
    const { name, ...info } = Module.get_file_info(`/work/${file.name}`)

    // Remap streams into collection.
    const streams = []
    for (let i = 0; i < info.streams.size(); i++) {
      const tags = {}
      for (let j = 0; j < info.streams.get(i).tags.size(); j++) {
        const t = info.streams.get(i).tags.get(j)
        tags[t.key] = t.value
      }
      streams.push({ ...info.streams.get(i), tags })
    }

    // Remap chapters into collection.
    const chapters = []
    for (let i = 0; i < info.chapters.size(); i++) {
      const tags = {}
      for (let j = 0; j < info.chapters.get(i).tags.size(); j++) {
        const t = info.chapters.get(i).tags.get(j)
        tags[t.key] = t.value
      }
      chapters.push({ ...info.chapters.get(i), ...{ tags } })
    }

    // Send back data response.

    origin.postMessage({
      streams,
      chapters,
      format: {
        ...info,
        format_name: name,
      },
    })

    // Cleanup mount.
    FS.unmount('/work')
    break
  }

  case 'get_frames': {
    if (!FS.analyzePath('/work').exists) {
      FS.mkdir('/work')
    }
    FS.mount(WORKERFS, { files: [file] }, '/work')

    const [,, offset] = message.data
    const info = Module.get_frames(`/work/${file.name}`, offset)

    // Remap frames into collection.
    const frames = []
    for (let i = 0; i < info.frames.size(); i++) {
      frames.push(info.frames.get(i))
    }

    origin.postMessage({
      ...info,
      frames,
    })

    // Cleanup mount.
    FS.unmount('/work')
    break
  }

  default:
    break
  }
}
self.importScripts('ffprobe-wasm.js') // Load ffprobe into worker context.
