<template>
  <div>
    <h4>Frames</h4>
    <b-table striped hover :items="frames">
      <template #cell(frame_type)="data">
        {{ String.fromCharCode(data.value) }}
      </template>

    </b-table>
  </div>
</template>

<script>
export default {
  name: 'Frames',
  props: [''],
  data() {
    return {
      data: null,
    };
  },
  computed: {
      frames() {
          const s = [];
          for (let i = 0; i < this.data.frames.size(); i++) {
              s.push(this.data.frames.get(i));
          }
          return s;
      },
  },
  created() {
    this.data = window.Module.get_frames();
  },
}
</script>