
let ws;

export function initWebSocket(onData) {
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    
    const url = import.meta.env.DEV
		? `${protocol}//localhost:8080`
		: `${protocol}//${location.host}/ws`;
		
	console.log('setup WebSocket', url);
	
    function connect() {
        ws = new WebSocket(url);

        ws.onopen = () => {
            console.log('WS connected');
        };

        ws.onmessage = (event) => {
			//console.error('ws.onmessage', event);
            try {
                const data = JSON.parse(event.data);

                // ожидаем что-то вроде:
                // { x: 1, y: 2, z: 3, roll: 0, pitch: 0, yaw: 0 }

                if (onData) {
                    onData(data);
                }
            } catch (e) {
                console.error('WS parse error', e);
            }
        };

        ws.onclose = () => {
            console.log('WS disconnected, reconnecting...');
            setTimeout(connect, 1000); // авто-реконнект
        };

        ws.onerror = (err) => {
            console.error('WS error', err);
            ws.close();
        };
    }

    connect();

	/*
    return {
        send: (data) => {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(data));
            }
        }
    };
    */
}



