import * as THREE from 'three';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';

// =========================
// 📡 SENSOR PIPELINE
// =========================
const imuQueue = [];

let lastSensorTime = performance.now();

// калибровка (из твоего "покоя")
const gyroOffset = {
	x: -2.9221,
	y: 1.6785,
	z: -0.2823
};

export function onSensorData(raw) {
	const now = performance.now();

	imuQueue.push({
		data: raw,
		time: now
	});
}

// =========================
// 🧠 ORIENTATION STATE
// =========================
let pitch = 0;
let roll = 0;
let yaw = 0;

const DEG2RAD = Math.PI / 180;
const alpha = 0.98;

// =========================
// 🎯 UPDATE LOGIC
// =========================
function updateOrientation(obj, raw, dt) {
	if (!raw?.imu) return;

	let { g, a } = raw.imu;

	// =========================
	// 🧹 1. gyro bias compensation
	// =========================
	const gx = (g.x - gyroOffset.x) * DEG2RAD;
	const gy = (g.y - gyroOffset.y) * DEG2RAD;
	const gz = (g.z - gyroOffset.z) * DEG2RAD;

	// =========================
	// 📐 2. normalize accelerometer
	// =========================
	const norm = Math.sqrt(a.x*a.x + a.y*a.y + a.z*a.z) || 1;

	const ax = a.x / norm;
	const ay = a.y / norm;
	const az = a.z / norm;

	// =========================
	// 📏 3. accel angles
	// =========================
	const accPitch = Math.atan2(ay, az);
	const accRoll  = Math.atan2(-ax, az);

	// =========================
	// 🔄 4. gyro integration
	// =========================
	pitch += gx * dt;
	roll  += gy * dt;
	yaw   += gz * dt;

	// =========================
	// 🧠 5. complementary filter
	// =========================
	pitch = alpha * pitch + (1 - alpha) * accPitch;
	roll  = alpha * roll  + (1 - alpha) * accRoll;

	// =========================
	// 🔁 6. apply via quaternion
	// =========================
	const euler = new THREE.Euler(
		pitch,
		yaw,
		-roll, // инверсия часто нужна для MPU
		'XYZ'
	);

	obj.quaternion.setFromEuler(euler);
}

// =========================
// 🎬 THREE INIT
// =========================
export function init3DView(sceneEl, modelUrl) {
	const width = sceneEl.clientWidth || 400;
	const height = sceneEl.clientHeight || 400;

	const scene = new THREE.Scene();
	scene.background = new THREE.Color(0x222222);

	const camera = new THREE.PerspectiveCamera(75, width / height, 0.1, 1000);
	camera.position.z = 5;

	const renderer = new THREE.WebGLRenderer({ antialias: true });
	renderer.setSize(width, height);
	sceneEl.appendChild(renderer.domElement);

	// свет
	scene.add(new THREE.HemisphereLight(0xffffff, 0x444444, 1));
	const light = new THREE.DirectionalLight(0xffffff, 1);
	light.position.set(1, 1, 1);
	scene.add(light);

	// модель
	const loader = new GLTFLoader();
	let drone = null;

	loader.load(modelUrl, (gltf) => {
		drone = gltf.scene;

		drone.scale.set(1, 1, 1);
		scene.add(drone);

		console.log('✅ model loaded');
	});

	// =========================
	// 🔁 ANIMATION LOOP
	// =========================
	let lastTime = performance.now();

	function animate() {
		requestAnimationFrame(animate);

		if (drone) {
			// обрабатываем ВСЕ накопленные данные
			while (imuQueue.length > 0) {
				const item = imuQueue.shift();

				const dt = (item.time - lastTime) / 1000;
				lastTime = item.time;

				updateOrientation(drone, item.data, dt);
			}
		}

		renderer.render(scene, camera);
	}

	animate();

	return {
		scene,
		camera,
		renderer
	};
}
