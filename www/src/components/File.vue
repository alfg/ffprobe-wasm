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

        <b-progress
          height="2px"
          v-if="showProgress"
          :value="progress"
          max="100"></b-progress>

        <div v-if="info">
          <div class="mt-3">Selected file: {{ file ? `${file.name}: ${file.size} bytes` : '' }}</div>

          <b-tabs class="mt-4">
            <b-tab title="Overview" class="mt-2">
              <div v-if="info">
                <Overview :info="info" />
              </div>
            </b-tab>
            <b-tab title="Frames" class="mt-2" lazy>
              <Frames />
            </b-tab>
          </b-tabs>
        </div>
    </div>
</template>

<script>
import Overview from './Overview.vue';
import Frames from './Frames.vue';

const worker = new Worker('ffprobe-worker.js');

export default {
  name: 'File',
  components: {
    Overview,
    Frames,
  },
  data() {
    return {
      file: null,
      data: null,
      info: null,
      progress: 0,
      showProgress: false,
    }
  },
  created() {
    worker.onmessage = (e) => {
      this.info = e.data;
    }
  },
  methods: {
    onFile(event) {
      this.data = null;
      this.progress = 0;
      this.showProgress = true;

      const file = event.dataTransfer ? event.dataTransfer.files[0] : event.target.files[0];
      worker.postMessage([ file ]);
    }
  }
}
</script>