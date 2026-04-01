const WebSocket = require('ws');

const PERIOD = 100; //  in milliseconds (1000 ms = 1 second)

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
                x: Math.random() * 2 - 1,
                y: Math.random() * 2 - 1,
                z: Math.random() * 2 - 1,
                roll: Math.random() * 360,
                pitch: Math.random() * 360,
                yaw: Math.random() * 360,
                timestamp: Date.now()
            };

        case 'static':
            return {
                x: 0,
                y: 0,
                z: 1,
                roll: 0,
                pitch: 0,
                yaw: 0,
                timestamp: Date.now()
            };

        case 'sin':
        default:
            return {
                x: Math.sin(t) + noise(),
                y: Math.cos(t) + noise(),
                z: Math.sin(t * 0.5) + noise(),

                roll: Math.sin(t) * 30 + noise(1),
                pitch: Math.cos(t) * 30 + noise(1),
                yaw: t * 10,

                timestamp: Date.now()
            };
    }
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
