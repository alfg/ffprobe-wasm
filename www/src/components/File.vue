<template>
    <div class="file">
      <b-form-row>
        <b-col>
          <b-form-group label="Select a file:" label-for="file">
            <b-input-group>
              <b-form-select class="protocol" v-model="protocol">
                <option v-for="o in protocols" :key="o.id" :value="o.value">{{ o.name }}</option>
              </b-form-select>

              <b-form-file
                  v-if="protocol === 'file'"
                  id="file"
                  accept=".mp4, .mkv"
                  v-model="file"
                  :state="Boolean(file)"
                  placeholder="Choose a file or drop it here..."
                  drop-placeholder="Drop file here..."
                  @change="onFile"
              ></b-form-file>

              <b-form-input
                v-if="protocol === 'url'"
                v-model="url"
                :state="Boolean(url)"
                placeholder="Enter a URL"
              ></b-form-input>

              <b-form-select v-if="protocol === 'example'" v-model="url">
                <template #first>
                  <b-form-select-option :value="null" disabled>-- Please select an option --</b-form-select-option>
                </template>
                <option v-for="o in examples" :key="o.id" :value="o.value">{{ o.name }}</option>
              </b-form-select>

              <b-input-group-append v-if="protocol !== 'file'">
                <b-button @click="onDownload">Download</b-button>
              </b-input-group-append>
            </b-input-group>
          </b-form-group>
        </b-col>
      </b-form-row>

      <b-progress
        class="mb-2"
        height="2px"
        v-if="showProgress"
        :value="progress"
        max="100">
      </b-progress>

      <div v-if="data">
        <div v-if="file">Selected file: {{ file ? `${file.name}: ${file.size} bytes` : '' }}</div>
        <div v-else>URL: {{ url ? `${url} (${size} bytes)` : '' }}</div>

        <b-tabs class="mt-4" v-model="tabIndex">
          <b-tab title="Overview" class="mt-2">
            <div v-if="data">
              <Overview :info="data" />
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
      protocols: [
        { name: 'File', value: 'file'},
        { name: 'URL', value: 'url'},
        { name: 'Example', value: 'example'},
      ],
      examples: [
        { name: 'Video Counter (10min, unfragmented, AVC Baseline)', value: 'https://video-examples-public.s3.us-west-2.amazonaws.com/video_counter_10min_unfragmented_avc.mp4' },
        { name: 'Tears of Steel 360p (00:12:14, unfragmented, AVC Baseline, 67.85 MB)', value: 'https://video-examples-public.s3.us-west-2.amazonaws.com/tears-of-steel-360p.mp4' },
      ],
      protocol: 'file',
      file: null,
      url: null,
      size: null,
      data: null,
      tabIndex: 0,
      progress: 0,
      showProgress: false,
    }
  },
  methods: {
    onFile(event) {
      this.tabIndex = 0;
      this.$worker.onmessage = (e) => {
        this.data = e.data;
      }
      const file = event.dataTransfer ? event.dataTransfer.files[0] : event.target.files[0];
      this.$worker.postMessage([ 'get_file_info', file ]);
    },
    onDownload() {
      this.showProgress = true;
      this.tabIndex = 0;
      this.$worker.onmessage = (e) => {
        this.data = e.data;
      }
      const xhr = new XMLHttpRequest();
      xhr.onprogress = (event) => {
        if (event.lengthComputable) {
          this.progress = parseInt(((event.loaded / event.total) * 100), 10);
        }
      }
      xhr.onload = (event) => {
        this.progress = 100;
        this.file = new File([event.target.response], "file");
        this.size = this.file.size;
        this.$worker.postMessage([ 'get_file_info', this.file ]);
        setTimeout(() => { this.showProgress = false; }, 2000);
      }
      xhr.open('GET', this.url, true);
      xhr.responseType = 'blob';
      xhr.send();
    },
  }
}
</script>

<style scoped>
.protocol {
  flex: 0 0 20% !important;
}
</style>