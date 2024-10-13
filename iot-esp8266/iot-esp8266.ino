#include <ESP8266WiFi.h>    // load library esp wifi
#include <WebSocketsServer.h> // load library websocket server
#include <PZEM004Tv30.h> // load library sensor pzem

PZEM004Tv30 pzem(4, 5); // Software Serial pin 4 (RX) & 5 (TX)

const char* ssid = "Mus";  //ssid wifi (have internet)
const char* password = "mustofa11"; //password wifi

// Create WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(10000); // create websocket server with port 10000


void setup() {
    Serial.begin(115200); // open serial baud rate 115200
    WiFi.begin(ssid, password); //initialize wifi
    while (WiFi.status() != WL_CONNECTED) {  // load wifi to connect
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi"); // wifi connected

  // Start WebSocket server
  webSocket.begin(); // initialize websocket
  webSocket.onEvent(webSocketEvent); //

  Serial.print("WebSocket server started at ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":10000");
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
    webSocket.broadcastTXT(jsonData);
    delay(5000); // Kirim data setiap 2 detik
    // Send data to WebSocket server

}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("Client connected from %s\n", webSocket.remoteIP(num).toString().c_str());
      break;
    case WStype_DISCONNECTED:
      Serial.printf("Client disconnected: %d\n", num);
      break;
    case WStype_TEXT:
      Serial.printf("Received text: %s\n", payload);
      break;
    default:
      break;
  }
}
