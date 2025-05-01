const net = require('net');

const specialEvents = [
    "Hole Detected Front Left",
    "Hole Detected Left",
    "Hole Detected Front Right",
    "Hole Detected Right",
    "OB Detected Front Left",
    "OB Detected Left",
    "OB Detected Front Right",
    "OB Detected Right",
    "Hit object on left",
    "Hit object on right"
];
let specialEventIndex = 0;

const server = net.createServer((socket) => {
    console.log('Client connected');

    let interval = null;
    let currentCommand = null;
    let value = 0;
    let isFirstCycle = true;
    let scanInterval = null;
    let scanning = false;

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

        if (scanning) {
            clearInterval(scanInterval);
            scanning = false;
            socket.write('Scan stopped.\n');
        }
    };

    const startScan = () => {
        if (scanning) {
            socket.write('Scan is already in progress.\n');
            return;
        }
        scanning = true;
        socket.write('sScanning\n');
        const scanData = [
            { degree: 0, distance: 86.82 },
            { degree: 2, distance: 84.20 },
            { degree: 4, distance: 104.27 },
            { degree: 6, distance: 135.68 },
            { degree: 8, distance: 137.27 },
            { degree: 10, distance: 142.93 },
            { degree: 12, distance: 139.63 },
            { degree: 14, distance: 122.85 },
            { degree: 16, distance: 124.51 },
            { degree: 18, distance: 138.91 },
            { degree: 20, distance: 115.95 },
            { degree: 22, distance: 113.80 },
            { degree: 24, distance: 112.71 },
            { degree: 26, distance: 133.54 },
            { degree: 28, distance: 140.65 },
            { degree: 30, distance: 141.55 },
            { degree: 32, distance: 138.64 },
            { degree: 34, distance: 134.03 },
            { degree: 36, distance: 138.53 },
            { degree: 38, distance: 140.12 },
            { degree: 40, distance: 138.74 },
            { degree: 42, distance: 130.06 },
            { degree: 44, distance: 132.71 },
            { degree: 46, distance: 136.89 },
            { degree: 48, distance: 140.37 },
            { degree: 50, distance: 137.90 },
            { degree: 52, distance: 136.31 },
            { degree: 54, distance: 135.72 },
            { degree: 56, distance: 123.44 },
            { degree: 58, distance: 124.07 },
            { degree: 60, distance: 132.30 },
            { degree: 62, distance: 131.81 },
            { degree: 64, distance: 137.00 },
            { degree: 66, distance: 139.35 },
            { degree: 68, distance: 136.73 },
            { degree: 70, distance: 106.45 },
            { degree: 72, distance: 86.08 },
            { degree: 74, distance: 36.95 },
            { degree: 76, distance: 38.08 },
            { degree: 78, distance: 40.92 },
            { degree: 80, distance: 40.72 },
            { degree: 82, distance: 41.64 },
            { degree: 84, distance: 53.77 },
            { degree: 86, distance: 94.85 },
            { degree: 88, distance: 112.90 },
            { degree: 90, distance: 136.60 },
            { degree: 92, distance: 138.17 },
            { degree: 94, distance: 137.33 },
            { degree: 96, distance: 117.33 },
            { degree: 98, distance: 79.49 },
            { degree: 100, distance: 71.16 },
            { degree: 102, distance: 72.63 },
            { degree: 104, distance: 81.12 },
            { degree: 106, distance: 104.39 },
            { degree: 108, distance: 138.74 },
            { degree: 110, distance: 137.34 },
            { degree: 112, distance: 138.63 },
            { degree: 114, distance: 137.34 },
            { degree: 116, distance: 136.89 },
            { degree: 118, distance: 123.52 },
            { degree: 120, distance: 135.01 },
            { degree: 122, distance: 123.58 },
            { degree: 124, distance: 132.80 },
            { degree: 126, distance: 111.73 },
            { degree: 128, distance: 106.62 },
            { degree: 130, distance: 110.84 },
            { degree: 132, distance: 81.51 },
            { degree: 134, distance: 71.22 },
            { degree: 136, distance: 67.23 },
            { degree: 138, distance: 66.63 },
            { degree: 140, distance: 56.68 },
            { degree: 142, distance: 40.96 },
            { degree: 144, distance: 41.23 },
            { degree: 146, distance: 39.43 },
            { degree: 148, distance: 46.50 },
            { degree: 150, distance: 58.50 },
            { degree: 152, distance: 73.05 },
            { degree: 154, distance: 93.17 },
            { degree: 156, distance: 95.88 },
            { degree: 158, distance: 99.23 },
            { degree: 160, distance: 92.31 },
            { degree: 162, distance: 82.27 },
            { degree: 164, distance: 90.45 },
            { degree: 166, distance: 89.16 },
            { degree: 168, distance: 82.77 },
            { degree: 170, distance: 85.08 },
            { degree: 172, distance: 94.86 },
            { degree: 174, distance: 75.04 },
            { degree: 176, distance: 85.34 },
            { degree: 178, distance: 82.39 },
            { degree: 180, distance: 90.26 },
        ];

        let index = 0;
        scanInterval = setInterval(() => {
            if (index < scanData.length) {
                socket.write(`Degree: ${scanData[index].degree}  ADC Distance: ${scanData[index].distance.toFixed(2)}\n`);
                index++;
            } else {
                socket.write('Object at 78 degrees has a distance of 47.99cm and width of 10.05cm\n');
                socket.write('Object at 101 degrees has a distance of 140.33cm and width of 14.69cm\n');
                socket.write('Object at 142 degrees has a distance of 48.89cm and width of 17.07cm\n');
                socket.write('Object at 176 degrees has a distance of 81.53cm and width of 5.69cm\n');
                socket.write('END\n');
                clearInterval(scanInterval);
                scanning = false;
            }
        }, 30);
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
            
                // Special event every 100 cm for forward movement
                if (command === 'w' && value % 100 === 0) {
                    const event = specialEvents[specialEventIndex];
                    socket.write(`${event}\n`);
                    socket.write('Drove Back 10cm\n');
                    value -= 10;
                    specialEventIndex = (specialEventIndex + 1) % specialEvents.length;
            
                    // Stop movement after event
                    stopSimulation();
                }
            
            }, 30);
        } else if (command === 's') {
            startScan();
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
