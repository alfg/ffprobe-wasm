<template>
  <div>
    <div v-if="!data" class="text-center mt-4">Loading...</div>
    <div v-if="data">
      <b-table
        class="col-4"
        :items="metadata"
        thead-class="d-none"
        small outlined
      ></b-table>

      <div>
        <b-form-group label="Timestamp:" label-for="timestamp">
          <b-input class="float-left col-2" v-model="value" @change="inputHandler"></b-input>
          <b-button class="float-right ml-2" @click="onNext">Next</b-button>
          <b-button class="float-right" @click="onPrev">Prev</b-button>
          <b-input id="timestamp" v-model="value" type="range" min="0" :max="data.duration" @change="inputHandler"></b-input>
        </b-form-group>
      </div>

      <b-table striped hover :fields="fields" :items="data.frames" :busy="isBusy">
        <template #cell(frame_number)="data">
          {{ data.value + 1 }}
        </template>
        <template #cell(pict_type)="data">
          {{ String.fromCharCode(data.value) }}
        </template>
      </b-table>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Frames',
  props: ['file'],
  data() {
    return {
      fields: [
        { key: 'frame_number', label: 'Index' },
        'pict_type', 'pts', 'dts', 'pos', 'pkt_size'
      ],
      data: null,
      isBusy: false,
      value: 0,
      lastPts: 0,
      ptsPageSize: 0,
    };
  },
  computed: {
    metadata() {
      return [
        { name: 'GOP Size', value: this.data.gop_size },
        { name: 'Duration', value: this.data.duration },
        { name: 'Timebase', value: this.data.time_base },
        { name: 'Total Frames', value: this.data.nb_frames },
      ]
    }
  },
  created() {
    this.$worker.onmessage = (e) => {
      this.data = e.data;
      this.isBusy = false;
      this.value = e.data.frames.length > 0 ? e.data.frames[0].pts : 0;
    }
    this.$worker.postMessage([ 'get_frames', this.file, 0 ]);
  },
  methods: {
    inputHandler(value) {
      this.getFrames(value);
    },
    onNext() {
      this.lastPts = this.data.frames[0].pts;
      const nextPts = this.data.frames[this.data.frames.length - 1].pts;
      this.getFrames(nextPts);
    },
    onPrev() {
      if (this.ptsPageSize === 0) {
        this.ptsPageSize = this.data.frames[0].pts - this.lastPts;
      }
      const prevPts = this.data.frames[0].pts - this.ptsPageSize;
      this.getFrames(prevPts);
    },
    getFrames(value) {
      this.isBusy = true;
      this.$worker.postMessage([ 'get_frames', this.file, parseInt(value) ]);
      window.scrollTo(0, 0);
    }
  }
}
</script>