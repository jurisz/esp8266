
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <MyWifiSecurity.h>

#define USE_SERIAL Serial
#define DHTPIN 1     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti WiFiMulti;

// Time to deep sleep (in seconds): 15 * 60
const int DEEP_SLEEP_SECONDS = 60;
const int SENSOR_WARM_UP_SECONDS = 5;

struct SensorData {
  float humidity;
  float temp;
};

SensorData readSensor() {
	SensorData r;
	r.humidity = dht.readHumidity();
	r.temp = dht.readTemperature()
	return r;
}

void connectAndPostData(SensorData data) {

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
  
        //TODO
  
        http.end();
    } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
}

void setup() {

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

   for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
   }

   //TODO switch on sensor power GPIO-2	
   
   delay(SENSOR_WARM_UP_SECONDS * 1000)
   
   SensorData data = readSensor();
   connectAndPostData(data);

   USE_SERIAL.println("Measurements done and sent. Going for deep sleep...");
   ESP.deepSleep(sleepTimeS * 1000000);
}
