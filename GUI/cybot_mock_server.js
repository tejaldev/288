const net = require('net');

// Create a TCP server
const server = net.createServer((socket) => {
    console.log('Client connected');

    socket.on('data', (data) => {
        const command = data.toString().trim();
        console.log(`Received from client: ${command}`);

        if (command === 'w') {
            // Simulate driving forward
            socket.write('wDrove Forward: 0.00 cm\n');
            socket.write('Drove Forward: 15.00 cm\n');
            socket.write('END\n');
        } else if (command === 'a') {
            // Simulate turning left
            socket.write('aTurned Left 0.00 degrees\n');
            socket.write('Turned Left 90.00 degrees\n');
            socket.write('END\n');
        } else if (command === 'd') {
            // Simulate turning right
            socket.write('dTurned Right 0.00 degrees\n');
            socket.write('Turned Right 90.00 degrees\n');
            socket.write('END\n');
        } else if (command === 'q') {
            // Simulate key release
            //socket.write('qStopping command\n');
        } else if (command === 'quit') {
            console.log("Client requested quit.");
            socket.end();
        } else {
            socket.write(`Unknown command: ${command}\n`);
        }
    });

    socket.on('end', () => {
        console.log('Client disconnected');
    });

    socket.on('error', (err) => {
        console.error('Socket error:', err);
    });
});

// Listen on port 288 (same as Python client expects)
server.listen(288, () => {
    console.log('Mock CyBot TCP server listening on port 288');
});
