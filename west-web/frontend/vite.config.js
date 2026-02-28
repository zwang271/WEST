import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import wasm from 'vite-plugin-wasm'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react(), wasm()],

  // For GitHub Pages with custom domain (west.temporallogic.org), use '/'.
  // For github.io/WEST (no custom domain), change to '/WEST/'.
  base: '/',

  server: {
    port: 5173,
  },

  build: {
    target: 'esnext',
  },

  optimizeDeps: {
    exclude: ['./src/wasm/west_rust.js'],
  },
})
