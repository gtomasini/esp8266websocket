/*
 * esp8266 WebSocketServer.ino
 *
 * just an echo server
 * 
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;

//change these please
const char *ssid =  "MIWIFI_2G_rsYk";   
const char *wifi_password =  "3q6pywysfkju"; 

WebSocketsServer webSocket = WebSocketsServer(80);

#define USE_SERIAL Serial

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
				
				        // send message to client
				        webSocket.sendTXT(num, "Hi from WebSocket Server(ESP8266)");
            }
            break;
            
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] got data Text: %s\n", num, payload);
            webSocket.sendTXT(num, payload);// send message to client
            //send data to all connected clients
            //webSocket.broadcastTXT("message here");
            break;
            
        case WStype_BIN:
            USE_SERIAL.printf("[%u] got binary data, length: %u\n", num, length);
            hexdump(payload, length);
            //send some message to client
            //webSocket.sendBIN(num, payload, length);
            break;

         default:
           USE_SERIAL.printf("[WSc] event %d, not implemented yet...", type);
           break;
    }
}

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(9600);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(ssid, wifi_password);
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    USE_SERIAL.println("my WEBsocket server is up!!!");
}

void loop() {
    webSocket.loop();
}
