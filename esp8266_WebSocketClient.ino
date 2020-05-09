/*
 * esp8266 WebSocketClient.ino
 *
 * 
 * 
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

//change these please !!!
const char *ssid =  "MIWIFI_2G_rsYk";   
const char *wifi_password =  "3q6pywysfkju"; 


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
      
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			webSocket.sendTXT("hi from y esp8266 (connecting)");//send msg to server when Connected
		  }
			break;
      
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get data text: %s\n", payload);

			webSocket.sendTXT(payload);//send message back to client (echo)
			break;
      
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] got binary data, length: %u\n", length);
			hexdump(payload, length);

			webSocket.sendBIN(payload, length);// send data back to client(echo)
			break;
      
    case WStype_PING:
       USE_SERIAL.printf("[WSc] get ping\n");// pong will be send automatically
       break;
            
    case WStype_PONG:
       //answer to a ping we send
       USE_SERIAL.printf("[WSc] get pong\n");
       break;

    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      USE_SERIAL.printf("[WSc] event %d, not implemented yet...", type);
      break;            
    }
}

void setup() {
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

  //WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

  //CHANGE THIS PLEASE
	// websocket server address, port and URL
	webSocket.begin("192.168.1.143", 80, "/");//my esp8266 board
  
  //webSocket.begin("echo.websocket.org", 80, "/");//internet free echo server

	webSocket.onEvent(webSocketEvent);// event handler

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);
  USE_SERIAL.println("my WEBsocket client is up!!!");
}

void loop() {
	webSocket.loop();
}
