<template>
    <div class="file">
        <b-form-group label="Select a file:" label-for="file">
          <b-form-file
              id="file"
              accept="video/mp4"
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

        <div v-if="data">
          <div class="mt-3">Selected file: {{ file ? `${file.name}: ${file.size} bytes` : '' }}</div>
          <hr />
          {{ info }}
        </div>
    </div>
</template>

<script>
export default {
  name: 'File',
  components: {
  },
  data() {
    return {
      file: null,
      data: null,
      progress: 0,
      showProgress: false,
    }
  },
  computed: {
    info() {

      const data = this.data;
      console.log('writing data to memfs');
      window.Module.FS.writeFile('testingfs', new Uint8Array(data));
      console.log('writing data to memfs done');

      window.Module.ccall('version');
      // Window.Module.ccall('openfile');
      return 'info';
    }
  },
  methods: {
    onFile(event) {
      this.data = null;
      this.progress = 0;
      this.showProgress = true;

      const file = event.dataTransfer ? event.dataTransfer.files[0] : event.target.files[0];
      const reader = new FileReader();

      // reader.onload = e => this.$emit("load", event.target.result);
      reader.onload = (event) => {
        this.progress = 100;
        this.data = new Uint8Array(event.target.result);
        setTimeout(() => { this.showProgress = false; }, 2000);
      }
      reader.onprogress = (event) => {
        if (event.lengthComputable) {
          this.progress = parseInt(((event.loaded / event.total) * 100), 10);
        }
      }
      reader.readAsArrayBuffer(file);
    }
  }
}
</script>

<style scoped>
.tree-view {
  overflow: auto;
  height: 60vh;
}
</style>
