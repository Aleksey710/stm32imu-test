import * as THREE from 'three';
import Chart from 'chart.js/auto';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, 1, 0.1, 1000);
const renderer = new THREE.WebGLRenderer();
renderer.setSize(400, 400);
document.getElementById('scene').appendChild(renderer.domElement);

const geometry = new THREE.BoxGeometry();
const material = new THREE.MeshStandardMaterial({ color: 0x00ffcc });
const cube = new THREE.Mesh(geometry, material);
scene.add(cube);

camera.position.z = 5;

const light = new THREE.DirectionalLight(0xffffff, 1);
light.position.set(0,1,1).normalize();
scene.add(light);

function animate() {
  requestAnimationFrame(animate);
  renderer.render(scene, camera);
}
animate();

// WebSocket
const ws = new WebSocket('ws://192.168.234.17/ws'); // замените на IP ESP32
ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  cube.rotation.x = data.x;
  cube.rotation.y = data.y;
  cube.rotation.z = data.z;

  // Chart update
  if (window.chart) {
    window.chart.data.datasets[0].data.push(data.x);
    window.chart.data.datasets[1].data.push(data.y);
    window.chart.data.datasets[2].data.push(data.z);
    window.chart.update();
  }

  // Table update
  const table = document.getElementById('data-table');
  table.innerHTML = `<tr><td>X</td><td>${data.x.toFixed(2)}</td></tr>
                     <tr><td>Y</td><td>${data.y.toFixed(2)}</td></tr>
                     <tr><td>Z</td><td>${data.z.toFixed(2)}</td></tr>`;
};

// Chart
const ctx = document.getElementById('chart').getContext('2d');
window.chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: [],
    datasets: [
      { label: 'X', data: [], borderColor: 'red' },
      { label: 'Y', data: [], borderColor: 'green' },
      { label: 'Z', data: [], borderColor: 'blue' }
    ]
  },
  options: {
    animation: false,
    responsive: true
  }
});
