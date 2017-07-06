/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 * For DeepSleep pull up (connect)  D0 <-> RST
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;


const char* SSID = "Android";
const char* WIFI_PASS = "xxxxxx";

// Time to deep sleep (in seconds):
const int sleepTimeS = 30;

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

   for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    connectToWifi();

	  USE_SERIAL.println("Going to deep sleep");
    ESP.deepSleep(sleepTimeS * 1000000);
    delay(100);
}

void connectToWifi() {

  WiFiMulti.addAP(SSID, WIFI_PASS);

  USE_SERIAL.println("Connecting to WIFI");
  int counter = 0;
  while (WiFiMulti.run() != WL_CONNECTED && counter < 5) {
    delay(1000);
    counter++;
    USE_SERIAL.print(".");
  }

  if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://freegeoip.net/json/"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        delay(5000);
  } else {
      USE_SERIAL.println("Not connected to WIFI");
  }
 
}

void loop() {
    //delay(5000);
}

