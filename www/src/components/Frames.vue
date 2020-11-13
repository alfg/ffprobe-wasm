<template>
  <div>
    <h4>Frames</h4>
    <p class="text-right">Total: {{ data.nb_frames }}</p>

    <b-pagination
      v-model="currentPage"
      @change="onPageChanged"
      :total-rows="pages"
      :per-page="perPage"
      align="right"
    ></b-pagination>

    <b-table striped hover :items="frames">
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
</template>

<script>
export default {
  name: 'Frames',
  data() {
    return {
      data: null,
      currentPage: 1,
      perPage: 48,
    };
  },
  computed: {
    pages() {
      return this.data.nb_frames;
    },
    frames() {
      if (!this.data) return [];

      const s = [];
      for (let i = 0; i < this.data.frames.size(); i++) {
        s.push(this.data.frames.get(i));
      }
      return s;
    },
  },
  created() {
    this.data = window.Module.get_frames(0);
  },
  methods: {
    onPageChanged(page) {
      this.data = window.Module.get_frames(this.perPage * (page - 1));
      window.scrollTo(0, 0);
    },
  }
}
</script>