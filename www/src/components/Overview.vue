<template>
  <div>
    <h4>Version</h4>
    <b-table stacked :items="versions"></b-table>

    <h4>Metadata</h4>
    <b-table stacked :items="items"></b-table>

    <h4>Streams</h4>
    <b-table striped hover :items="info.streams"></b-table>
  </div>
</template>

<script>
export default {
  name: 'Overview',
  props: ['info'],
  data() {
    return {
      items: [
        {
          name: this.info.name,
          duration: this.info.duration,
          bit_rate: this.info.bit_rate,
          url: this.info.url,
          nb_streams: this.info.nb_streams,
          flags: this.info.flags,
        },
      ],
      versions: [
        {
          libavutil:  this.info.versions.libavutil,
          libavcodec:  this.info.versions.libavcodec,
          libavformat:  this.info.versions.libavformat,
        }
      ]
    }
  }
}
</script>