<template>
  <div>
    <h4>Version</h4>
    <b-table stacked :items="versions"></b-table>

    <h4>Metadata</h4>
    <b-table stacked :items="items"></b-table>

    <h4>Streams</h4>
    <b-table striped hover :items="streams"></b-table>
  </div>
</template>

<script>
export default {
  name: 'Overview',
  props: ['info'],
  computed: {
      streams() {
          const s = [];
          for (let i = 0; i < this.info.streams.size(); i++) {
              s.push(this.info.streams.get(i));
          }
          return s;
      }
  },
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
          libavutil:  window.Module.avutil_version(),
          libavcodec:  window.Module.avcodec_version(),
          libavformat:  window.Module.avformat_version(),
        }
      ]
    }
  }
}
</script>