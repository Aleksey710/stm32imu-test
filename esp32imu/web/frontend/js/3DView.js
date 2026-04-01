import * as THREE from 'three';

let cube;

export function init3DView() {
	console.log('setup 3DView start...');
	
	console.log('WebGL available:', !!window.WebGLRenderingContext);

	try {
		renderer = new THREE.WebGLRenderer();
	} catch (e) {
		document.body.innerHTML = "WebGL not supported";
		return;
	}

	const scene = new THREE.Scene();
	const camera = new THREE.PerspectiveCamera(75, 1, 0.1, 1000);

	function hasWebGL() {
		try {
			const canvas = document.createElement('canvas');
			return !!window.WebGLRenderingContext &&
				   (canvas.getContext('webgl') || canvas.getContext('experimental-webgl'));
		} catch (e) {
			document.body.innerHTML = "WebGL not supported";
			return false;
		}
	}

	if (hasWebGL()) {
		// initThree();
		const renderer = new THREE.WebGLRenderer();
	} else {
		//initFallback(); // canvas/SVG
		
	}
	//const renderer = new THREE.WebGLRenderer();

	try {
		const renderer = new THREE.WebGLRenderer();
		renderer.setSize(400, 400);
		document.getElementById('scene').appendChild(renderer.domElement);

		const geometry = new THREE.BoxGeometry();
		const material = new THREE.MeshStandardMaterial({ color: 0x00ffcc });
		//const
		cube = new THREE.Mesh(geometry, material);
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
	} catch (e) {
		document.body.innerHTML = "WebGL not supported";
	}
}

export function dataUpdate(data) {

	if(cube)
	{
		cube.rotation.x = data.x;
		cube.rotation.y = data.y;
		cube.rotation.z = data.z;
	}
};
