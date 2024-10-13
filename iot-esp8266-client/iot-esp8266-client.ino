#include <ESP8266WiFi.h>    // load library esp wifi
#include <WebSocketsClient.h> // load library websocket client
#include <PZEM004Tv30.h> // load library sensor pzem

PZEM004Tv30 pzem(4, 5); // Software Serial pin 4 (RX) & 5 (TX)

const char* ssid = "Mus";  //ssid wifi (have internet)
const char* password = "mustofa11"; //password wifi

const char* websocket_server_host = "127.0.0.1";      //  IP atau domain dari server WebSocket dari nodejs
const uint16_t websocket_server_port = 10000;         //  port WebSocket server dari nodejs

WebSocketsClient webSocket;

void setup() {
    Serial.begin(115200); // open serial baud rate 115200
    WiFi.begin(ssid, password); //initialize wifi
    while (WiFi.status() != WL_CONNECTED) {  // load wifi to connect
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi"); // wifi connected

  // Start WebSocket server
  webSocket.begin(websocket_server_host, websocket_server_port, "/"); // initialize webcosket client

  // Tentukan event handler untuk WebSocket
  webSocket.onEvent(webSocketEvent);
}

void loop() {

    webSocket.loop();

    // Simulate sensor data
    float voltage = pzem.voltage();
    float current = pzem.current();
    //float voltage2 = pzem2.voltage();
    Serial.print(voltage);
    Serial.println(current);
    String jsonData = "{\"sensor_1\":" + String(voltage) + ",\"sensor_2\":" + String(current) + "}";
    // send data json, output {"sensor_1": voltage, "sensor_2": current}
    
    // Broadcast data to all connected clients
    webSocket.sendTXT(jsonData);
    delay(5000); // Kirim data setiap 2 detik
    // Send data to WebSocket server

}

// Event handler untuk WebSocket
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;
    case WStype_TEXT:
      Serial.printf("Received text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("Received binary data\n");
      break;
  }
}
