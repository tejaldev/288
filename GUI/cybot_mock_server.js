const net = require('net');

const server = net.createServer((socket) => {
    console.log('Client connected');

    let interval = null;
    let currentCommand = null;
    let value = 0;
    let isFirstCycle = true; // <-- NEW FLAG

    const stopSimulation = () => {
        if (interval) {
            clearInterval(interval);
            interval = null;
            let label = '';
            let unit = '';
            if (currentCommand === 'w') {
                label = 'Drove Forward';
                unit = 'cm';
            } else if (currentCommand === 'a') {
                label = 'Turned Left';
                unit = 'degrees';
            } else if (currentCommand === 'd') {
                label = 'Turned Right';
                unit = 'degrees';
            }
            if (label) {
                socket.write(`${label}: ${value.toFixed(2)} ${unit}\n`);
                socket.write('END\n');
            }
            currentCommand = null;
            value = 0;
        }
    };

    socket.on('data', (data) => {
        const command = data.toString().trim();
        console.log(`Received from client: ${command}`);

        if (command === 'q') {
            stopSimulation();
            return;
        }

        if (interval) {
            stopSimulation();
        }

        value = 0;
        currentCommand = command;
        isFirstCycle = true;

        if (command === 'w' || command === 'a' || command === 'd') {
            interval = setInterval(() => {
                value += 1;
                let label = '';
                let unit = '';
                if (command === 'w') {
                    label = 'Drove Forward';
                    unit = 'cm';
                } else if (command === 'a') {
                    label = 'Turned Left';
                    unit = 'degrees';
                } else if (command === 'd') {
                    label = 'Turned Right';
                    unit = 'degrees';
                }

                socket.write(`${isFirstCycle ? `${command}` : ''}${label}: ${value.toFixed(2)} ${unit}\n`);
                isFirstCycle = false;
            }, 150);
        } else if (command === 'quit') {
            console.log("Client requested quit.");
            socket.end();
        } else {
            socket.write(`Unknown command: ${command}\n`);
        }
    });

    socket.on('end', () => {
        console.log('Client disconnected');
        stopSimulation();
    });

    socket.on('error', (err) => {
        console.error('Socket error:', err);
        stopSimulation();
    });
});

server.listen(288, () => {
    console.log('Mock CyBot TCP server listening on port 288');
});
