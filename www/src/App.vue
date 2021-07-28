<template>
  <div>
    <b-navbar type="dark" variant="dark">
      <div class="container">
        <b-navbar-nav>
          <b-nav-item href="#">
            FFProbe
          </b-nav-item>
        </b-navbar-nav>
      </div>
    </b-navbar>

    <GitHubCorner />

    <div id="app" class="container">
      <File />
    </div>

    <footer class="container mt-4 text-center">
      <hr />
      <div class="text-muted">
        <ul>
          <li>{{ name }}-{{ version }}</li>
          <li><a href="https://github.com/alfg/ffprobe-wasm">Source</a></li>
          <li><a href="https://github.com/alfg/ffprobe-wasm/issues">Report Bugs</a></li>
        </ul>
      </div>
    </footer>
  </div>
</template>

<script>
import { name, version } from '../package.json';
import GitHubCorner from './components/GitHubCorner.vue';
import File from './components/File.vue';

export default {
  name: 'App',
  components: {
    File,
    GitHubCorner,
  },
  data() {
    return {
      name,
      version,
    }
  }
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  margin-top: 40px;
}

footer ul {
  display: inline-block;
  padding-left: 0;
  text-align: left;
  width: 100%;
}

footer ul li {
  display: inline;
  margin: 0 6px;
  list-style: none;
}
</style>
