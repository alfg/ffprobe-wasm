<template>
    <div class="file">
        <b-form-group label="Select a file:" label-for="file">
          <b-form-file
              id="file"
              accept=".mp4, .mkv"
              v-model="file"
              :state="Boolean(file)"
              placeholder="Choose a file or drop it here..."
              drop-placeholder="Drop file here..."
              @change="onFile"
          ></b-form-file>
        </b-form-group>

        <div v-if="info">
          <div class="mt-3">Selected file: {{ file ? `${file.name}: ${file.size} bytes` : '' }}</div>

          <b-tabs class="mt-4" v-model="tabIndex">
            <b-tab title="Overview" class="mt-2">
              <div v-if="info">
                <Overview :info="info" />
              </div>
            </b-tab>
            <b-tab title="Frames" class="mt-2" lazy>
              <Frames :file="file" />
            </b-tab>
          </b-tabs>
        </div>
    </div>
</template>

<script>
import Overview from './Overview.vue';
import Frames from './Frames.vue';

export default {
  name: 'File',
  components: {
    Overview,
    Frames,
  },
  data() {
    return {
      file: null,
      info: null,
      tabIndex: 0,
    }
  },
  methods: {
    onFile(event) {
      this.tabIndex = 0;
      this.$worker.onmessage = (e) => {
        this.info = e.data;
      }
      const file = event.dataTransfer ? event.dataTransfer.files[0] : event.target.files[0];
      this.$worker.postMessage([ 'get_file_info', file ]);
    }
  }
}
</script>