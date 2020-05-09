/*
 * ESP32 WebSocketClient for upwork
 *
 *
 */

#include <Arduino.h>

#include <WiFi.h>
//#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>

//WiFiMulti WiFiMulti;
WiFiClient wclient;
WebSocketsClient webSocket;

const char *ssid =  "MIWIFI_2G_rsYk";   
const char *wifi_password =  "3q6pywysfkju"; 

#define USE_SERIAL Serial

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0)
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  static int counter=0;
  char message[32];
  
  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
      
    case WStype_CONNECTED:
      USE_SERIAL.printf("[WSc] connected to url: %s\n", payload);
      webSocket.sendTXT("Hi from my ESP32");//send message to server when Connected
      break;
      
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] got txt data from server: \"%s\"\n", payload);
      // send message to server
      snprintf(message, 32, "MESSAGE_FROM_MYESP32_#%d", counter++);
      //webSocket.sendTXT("hello world (from my esp32)!!!");
      USE_SERIAL.printf("[WSc] send back to server txt data: \"%s\"\n\n", message);
      webSocket.sendTXT(message);
      break;
      
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] got binary data, length: %u\n", length);
      hexdump(payload, length);
      delay(10000);
      webSocket.sendBIN(payload, length);//send data to server
      break;

    case WStype_PING:
       USE_SERIAL.printf("[WSc] got ping\n");// pong will be send automatically
       break;
            
    case WStype_PONG:
       USE_SERIAL.printf("[WSc] got pong\n");// answer to a ping we send
       break;
             
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      USE_SERIAL.printf("[WSc] event %d, not implemented yet...", type);
      break;
  }
}

void setup_wifi() {
  USE_SERIAL.print("\nconnecting to ");
  USE_SERIAL.println(ssid);

  WiFi.begin(ssid, wifi_password); // Connect to network
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    USE_SERIAL.print(".");
  }

  //Serial.println();
  USE_SERIAL.print("\nWiFi connected with IP addr: ");
  USE_SERIAL.println(WiFi.localIP());
}

void setup() {
  USE_SERIAL.begin(9600);
  //Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  setup_wifi();

  //websocket server, CHANGE THIS PLEASE
  //webSocket.begin("echo.websocket.org", 80, "/");//echo server
  webSocket.begin("192.168.1.145", 80, "/");//esp8266 ip SERVER
  
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
