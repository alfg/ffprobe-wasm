<template>
  <div>
    <h4>Version</h4>
    <b-table stacked :items="versions"></b-table>

    <h4>Metadata</h4>
    <b-table stacked :items="items"></b-table>

    <h4>Streams</h4>
    <b-table striped hover :items="info.streams"></b-table>

    <h4>Chapters</h4>
    <b-table striped hover :items="info.chapters"></b-table>
  </div>
</template>

<script>
export default {
  name: 'Overview',
  props: ['info'],
  computed: {
    items() {
      console.log(this.info);
      return [
        {
          name: this.info.name,
          duration: this.info.duration,
          bit_rate: this.info.bit_rate,
          url: this.info.url,
          nb_streams: this.info.nb_streams,
          flags: this.info.flags,
          nb_chapters: this.info.nb_chapters,
        },
      ]
    },
    versions() {
      return [
        {
          libavutil:  this.info.versions.libavutil,
          libavcodec:  this.info.versions.libavcodec,
          libavformat:  this.info.versions.libavformat,
        },
      ]
    },
  },
}
</script>