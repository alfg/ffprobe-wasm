<template>
  <div>
    <b-form-group label="Output type:" label-for="raw-output-type">
      <b-form-select
        id="raw-output-type"
        class="col-3"
        v-model="outputType"
        :options="outputTypes"
        @change="load"
      ></b-form-select>
    </b-form-group>

    <div v-if="loading" class="text-center mt-4">Loading...</div>
    <b-alert v-else-if="error" show variant="danger">{{ error }}</b-alert>
    <pre v-else>{{ renderedData }}</pre>
  </div>
</template>

<script>
export default {
  name: 'Raw',
  props: ['file'],
  data() {
    return {
      data: null,
      error: null,
      loading: false,
      outputType: 'json',
      outputTypes: [
        { text: 'JSON object', value: 'json' },
        { text: 'JSON string', value: 'string' },
      ],
    };
  },
  computed: {
    renderedData() {
      return typeof this.data === 'string'
        ? this.data
        : JSON.stringify(this.data, null, 2);
    },
  },
  created() {
    this.load();
  },
  methods: {
    load() {
      this.loading = true;
      this.error = null;
      this.$worker.onmessage = ({ data }) => {
        this.loading = false;
        if (data && data.error) {
          this.error = data.error;
          this.data = null;
          return;
        }
        this.data = data;
      };
      this.$worker.postMessage(['get_raw_info', this.file, this.outputType]);
    },
  },
};
</script>

<style scoped>
pre {
  max-height: 70vh;
  overflow: auto;
  padding: 1rem;
  background: #f8f9fa;
}
</style>
