import { defineConfig } from 'vite';

export default defineConfig({
  root: 'src',      // указываем папку с index.html
  build: {
    outDir: '../dist',
    rollupOptions: {
      output: { entryFileNames: 'bundle.js' }
    }
  }
});
