const WebSocket = require('ws');

const PERIOD = 10; //  in milliseconds (1000 ms = 1 second)

const PORT = 8080;
const MODE = process.env.MODE || 'sin'; // sin | random | static

const wss = new WebSocket.Server({ port: PORT });

console.log(`Mock WS server running on ws://localhost:${PORT}`);
console.log(`Mode: ${MODE}`);

function noise(scale = 0.05) {
    return (Math.random() - 0.5) * scale;
}

function generateData(t) {
    switch (MODE) {
        case 'random':
			return {
				timestamp: Date.now(),
				imu:{
					g:{
						x: (Math.random() * 2 - 1),
						y: Math.random() * 2 - 2,
						z: Math.random() * 2 - 1

					},
					a:{
						x: (Math.random() * 2 - 1),
						y: Math.random() * 2 - 1,
						z: Math.random() * 2 - 1
					}
				},
				m:{
					x: (Math.random() * 2 - 1),
					y: Math.random() * 2 - 1,
					z: Math.random() * 2 - 1
				}
			};
        case 'static':
			return {
				timestamp: Date.now(),
				imu:{
					g:{
						x: 0,
						y: 0,
						z: 1
					},
					a:{
						x: 0,
						y: 0,
						z: 1
					}
				},
				m:{
					x: 0,
					y: 0,
					z: 1
				}
			};
		case 'sin':
		default:
		{
			const g_Ampl = 30;
			const g_faza_use = 0; // 0 or 1
			
			const g_x_period = 0.7;
			const g_y_period = 0.7;
			const g_z_period = 0.7;
			
			return {
				timestamp: Date.now(),
				imu:{
					g:{
						x: Math.sin(t * g_x_period) * g_Ampl + (30 * g_faza_use) + noise(),
						y: Math.cos(t * g_y_period) * g_Ampl + (60 * g_faza_use) + noise(),
						z: Math.sin(t * g_z_period) * g_Ampl + (90 * g_faza_use) + noise()
					},
					a:{
						x: Math.sin(t * 0.5) + noise(),
						y: Math.cos(t * 0.6) + noise(),
						z: Math.sin(t * 0.7) + noise()
					}
				},
				m:{
					x: Math.sin(t * 0.5) + noise(),
					y: Math.cos(t * 0.6) + noise(),
					z: Math.sin(t * 0.7) + noise()
				}
			};
		}	
	};
}

wss.on('connection', (ws) => {
    console.log('Client connected');

    let t = 0;

    const interval = setInterval(() => {
        t += 0.1;

        const data = generateData(t);
        const message = JSON.stringify(data);

        // 🔥 broadcast всем клиентам
        wss.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(message);
            }
        });

    }, PERIOD); 

    ws.on('close', () => {
        console.log('Client disconnected');
        clearInterval(interval);
    });

    ws.on('error', (err) => {
        console.error('WS error:', err.message);
    });

    // 🔥 можно принимать команды от клиента
    ws.on('message', (msg) => {
        try {
            const data = JSON.parse(msg);
            console.log('Received from client:', data);
        } catch {
            console.log('Raw message:', msg.toString());
        }
    });
});
