import * as ws from './websocketHandler.js';
//import * as dv from './3DView.js';
import * as tv from './tableHandler.js';
import * as cv from './chartHandler.js';

/*
console.log('MAIN JS EXEC', Date.now());
console.log('MAIN JS URL:', import.meta.url);
console.log('SCRIPT TAG:', document.currentScript?.src);
console.log('READY STATE:', document.readyState);
console.log('init (HMR)', import.meta.hot);
console.trace('ENTRY STACK');
*/

function updateData(data) {
	tv.dataUpdate(data);
	cv.dataUpdate(data);
	//dv.dataUpdate(data);
}
/*
let latestData = null;

function updateData(data) {
    latestData = data;
}

function renderLoop() {
    if (latestData) {
        tv.dataUpdate(latestData);
        cv.dataUpdate(latestData);
    }
    requestAnimationFrame(renderLoop);
}

renderLoop();
*/

function setup() {
	//console.log('setup start...', location.hostname);
	cv.initChart();
	//dv.init3DView();
	ws.initWebSocket(updateData);
}

// НАДЁЖНЫЙ GUARD
if (!window.__APP_STARTED__) {
    window.__APP_STARTED__ = true;
    setup();
} else {
    console.warn('App already started');
}

