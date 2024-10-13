const express = require('express'); // framework back-end javascript
const mysql = require('mysql'); // access mysql
const expressWs = require('express-ws');  //server ws
const WebSocket = require('ws'); //client ws
require('dotenv').config();

const app = express();
expressWs(app);
// Ganti dengan IP dan port ESP8266
const ws = new WebSocket(process.env.WS_URL);
const port = 10000;

// Setup MySQL connection
const db = mysql.createConnection({
    host: process.env.DB_HOST,
    user: process.env.DB_USERNAME,
    password: process.env.DB_PASSWORD,
    database: process.env.DB_DATABASE,
});

db.connect((err) => {
    if (err) throw err;
    console.log('Connected to MySQL Database');
});

ws.on('open', () => {
    console.log('Connected to WebSocket server');
});

ws.on('message', (message) => {
    console.log('Received data:', message);
    // Parse data
    const sensorData = JSON.parse(message);
    console.log('Parsed data:', sensorData);

    if (sensorData.sensor_1) {
        const query = 'INSERT INTO sensor_data (sensor_1, sensor_2) VALUES (?, ?)';
        db.query(query, [sensorData.sensor_1, sensorData.sensor_2], (err, result) => {
            if (err) throw err;
            console.log('Data inserted into MySQL:', result.insertId);
        });
    } else {
        console.log("NaN data");
    }

});

ws.on('close', () => {
    console.log('Disconnected from WebSocket server');
});

ws.on('error', (error) => {
    console.error('WebSocket error:', error);
});


// WebSocket route to stream data to clients
app.ws('/realtime-data', (ws, req) => {
    console.log('New client connected for real-time data');

    const interval = setInterval(() => {
        db.query('SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT 1', (err, results) => {
            if (err) throw err;
            ws.send(JSON.stringify(results));
        });
    }, 5000);
    
    ws.on('close', () => {
        console.log('Client disconnected');
        clearInterval(interval);
    });
});

app.use(express.static('public'));

app.listen(port, () => {
    console.log(`Node.js server is running on http://localhost:${port}`);
});
// app.ws('/', (ws, req) => {
//     console.log('New client connected for real-time data');

//     ws.on('message', (msg) => {
//         console.log(`Received: ${msg}`);
//         // Parse data
//         const sensorData = JSON.parse(msg);
//         console.log('Parsed data:', sensorData);

//         if (sensorData.sensor_1 && sensorData.sensor_2) {
//             const query = 'INSERT INTO sensor_data (sensor_1, sensor_2) VALUES (?, ?)';
//             db.query(query, [sensorData.sensor_1, sensorData.sensor_2], (err, result) => {
//                 if (err) throw err;
//                 console.log('Data inserted into MySQL:', result.insertId);
//             });
//         } else {
//             console.log("NaN data");
//         }
//     });

//     ws.on('close', () => {
//         console.log('Client disconnected');
//     });
// });
