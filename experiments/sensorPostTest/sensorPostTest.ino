#include <MyWifiSecurity.h>

/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

char* formatFloat(float number, char* buf) {
//     char tmpStr[6];
     dtostrf(number, 6, 1, buf);
     return buf;
}

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.println("http begin connect");
        http.begin("http://192.168.20.1:3000/sensor/save/gh-sm1");

        USE_SERIAL.println("Http posting");
        // start connection and send HTTP header
        http.addHeader("Accept", "*/*");
        http.addHeader("Content-Type", "application/json");
        
        float temp = (float) random(500)/10.0;
        char tempStr[7];
        char jsonBuffer[255];
        sprintf(jsonBuffer, "[{\"name\": \"vv\", \"value\": %d}, {\"name\": \"temp1\", \"value\": %s}]", 3, formatFloat(temp, tempStr));

//        USE_SERIAL.print("json payload: ");
//        USE_SERIAL.print(jsonBuffer);
//        USE_SERIAL.print("\n");
        
        int httpCode = http.POST(jsonBuffer);

        // httpCode will be negative on error
        if(httpCode > 0) {
            USE_SERIAL.printf("Post result code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(30000);
}



