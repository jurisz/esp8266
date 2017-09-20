
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

const char GREEN_HOUSE_CODE[] = "c1";

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
	r.temp = dht.readTemperature();
	return r;
}

char* formatFloat(float number, char* buf) {
     dtostrf(number, 6, 1, buf);
     return buf;
}

void connectAndPostData(SensorData data) {

	WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
	
	USE_SERIAL.println("Connecting to WIFI");
    int counter = 0;
    while (WiFiMulti.run() != WL_CONNECTED && counter < 5) {
        delay(1000);
        counter++;
        USE_SERIAL.print(".");
    }

    if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        USE_SERIAL.println("http begin connect");
        http.begin("http://192.168.20.1:3000/sensor/save/c1");

        USE_SERIAL.println("Http posting");
        // start connection and send HTTP header
        http.addHeader("Accept", "*/*");
        http.addHeader("Content-Type", "application/json");
        
        float temp = (float) random(500)/10.0;
        char tempStr[7];
        char humidStr[7];
        char jsonBuffer[255];
        sprintf(jsonBuffer, "[{\"name\": \"temp\", \"value\": %s}, {\"name\": \"humidity\", \"value\": %s}]",
         formatFloat(data.temp, tempStr), formatFloat(data.humidity, humidStr));

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
            USE_SERIAL.printf("Http POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    } else {
        USE_SERIAL.printf("Can't connect to Wifi");
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
   
   delay(SENSOR_WARM_UP_SECONDS * 1000);
   
   SensorData data = readSensor();
   connectAndPostData(data);

   USE_SERIAL.println("Measurements done and sent. Going for deep sleep...");
   ESP.deepSleep(DEEP_SLEEP_SECONDS * 1000000);
}

void loop() {
    //empty
}
