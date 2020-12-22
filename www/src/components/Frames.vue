<template>
  <div>
    <h4>Frames</h4>
    <div v-if="!data">Loading...</div>
    <div v-if="data">
      <p class="text-right">Total: {{ data.nb_frames }}</p>

      <b-pagination
        v-model="currentPage"
        @change="onPageChanged"
        :total-rows="pages"
        :per-page="perPage"
        align="right"
      ></b-pagination>

      <b-table striped hover :items="data.frames" :busy="isBusy">
        <template #table-busy>
          <div class="text-center text-primary my-2">
            <b-spinner class="align-middle"></b-spinner>
          </div>
        </template>
        <template #cell(pict_type)="data">
          {{ String.fromCharCode(data.value) }}
        </template>
      </b-table>

      <b-pagination
        v-model="currentPage"
        @change="onPageChanged"
        :total-rows="pages"
        :per-page="perPage"
        align="right"
      ></b-pagination>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Frames',
  props: ['file'],
  data() {
    return {
      data: null,
      currentPage: 1,
      perPage: 48,
      isBusy: false,
    };
  },
  computed: {
    pages() {
      return this.data.nb_frames;
    },
  },
  created() {
    this.$worker.onmessage = (e) => {
      this.data = e.data;
      this.isBusy = false;
    }
    this.$worker.postMessage([ 'get_frames', this.file, 0 ]);

  },
  methods: {
    onPageChanged(page) {
      this.isBusy = true;
      this.$worker.postMessage([ 'get_frames', this.file, this.perPage * (page - 1) ]);
      window.scrollTo(0, 0);
    },
  }
}
</script>