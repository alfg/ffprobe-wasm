<template>
  <div>
    <h4>Version</h4>
    <b-table stacked :items="versions"></b-table>

    <h4>Metadata</h4>
    <b-table stacked :items="items"></b-table>

    <h4>Streams</h4>
    <b-table striped hover :items="info.streams" :fields="stream_fields">
      <template #cell(show_details)="row">
        <b-button size="sm" @click="row.toggleDetails" class="mr-2">
          {{ row.detailsShowing ? 'Hide' : 'Show'}}
        </b-button>
      </template>
      <template #row-details="row">
        <b-card>
          <div v-for="(value, key) in row.item.tags" :key="key">
          <b-row class="mb-2">
            <b-col sm="3" class="text-sm-right"><b>{{ key }}:</b></b-col>
            <b-col>{{ value }}</b-col>
          </b-row>
          </div>
        </b-card>
      </template>
    </b-table>

    <div v-show="info.chapters.length > 0">
      <h4>Chapters</h4>
      <b-table striped hover :items="info.chapters" :fields="chapter_fields">
      <template #cell(show_details)="row">
        <b-button size="sm" @click="row.toggleDetails" class="mr-2">
          {{ row.detailsShowing ? 'Hide' : 'Show'}}
        </b-button>
      </template>
      <template #row-details="row">
        <b-card>
          <div v-for="(value, key) in row.item.tags" :key="key">
          <b-row class="mb-2">
            <b-col sm="3" class="text-sm-right"><b>{{ key }}:</b></b-col>
            <b-col>{{ value }}</b-col>
          </b-row>
          </div>
        </b-card>
      </template>
      </b-table>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Overview',
  props: ['info'],
  data() {
    return {
      stream_fields: [
        'id',
        'start_time',
        'duration',
        'codec_type',
        'codec_name',
        'format',
        'bit_rate',
        'profile',
        'level',
        'width',
        'height',
        'channels',
        'sample_rate',
        'frame_size',
        { key: 'show_details', label: 'Tags' },
      ],
      chapter_fields: ['id', 'time_base', 'start', 'end', { key: 'show_details', label: 'Tags' }],
    }
  },
  computed: {
    items() {
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