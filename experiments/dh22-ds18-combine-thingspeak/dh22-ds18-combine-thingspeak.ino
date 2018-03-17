#include <ESP8266WiFi.h>
#include <MyWifiSecurity.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// JU | TP_LINK
char* WIFI_SSID = WIFI_TP_LINK_SSID;
char* WIFI_PASS = WIFI_TP_LINK_PASS;

char* thingSpeakUrl = "api.thingspeak.com";
char* apiKey = THINGSPEAK_TEST_KEY;
unsigned long channelID = THINGSPEAK_TEST_CHANNEL;

const int DEEP_SLEEP_SECONDS = 15 * 60;

const int DHT_TEMP_FIELD = 3;
const int DHT_HUMID_FIELD = 4;
const int DS18_FIELD = 5;

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS 0  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

WiFiClient client; 

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

float readTemperature() {
    DS18B20.begin();
    DS18B20.requestTemperatures(); 
    float temp = DS18B20.getTempCByIndex(0);
    Serial.println("Ds18 temperature: " + String(temp));
    return temp;
}

int connectToWiFi(){
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 10) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        counter++;
        Serial.print(".");
        delay(2000);
    }
    
    Serial.print("Wifi status: ");
    Serial.println(WiFi.status());
    
    ThingSpeak.begin( client );

    /*
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println( "Connected to wifi" );
      ThingSpeak.begin( client );
      //return 1;
    } else {
      Serial.println( "No wifi" );
      //return -1;
    }
    */
    return 1;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  delay(1000);

  if (connectToWiFi() > 0)  {
    bool dataExists = false;
    
    float temp = readTemperature();
    if (temp > -50 && temp < 55 ) {
      sendDs18TemperatureToThingSpeak(temp);
      dataExists = true;
    }
    
    SensorData data = readSensor();
    if (isnan(data.humidity) || isnan(data.temp)) {
      Serial.println("No sensor data");
    } else {
      sendDht22TemperatureToThingSpeak(data); 
      dataExists = true;
    }

    //dataExists =true;
    //ThingSpeak.setField(1, 10.0f);

    if (dataExists) {
      int  writeSuccess = ThingSpeak.writeFields(channelID, apiKey); 
      if ( writeSuccess ){
        Serial.println( "Data written to Thingspeak." );
      } else {
        Serial.println( "Error writing to teamspeak code: " + writeSuccess );
      }
    }
  }
 
  Serial.println("Done. Going for deep sleep!");
  ESP.deepSleep(DEEP_SLEEP_SECONDS * 1000000);
}

void sendDht22TemperatureToThingSpeak(SensorData data) {
  ThingSpeak.setField(DHT_TEMP_FIELD, data.temp);
  Serial.println( "Dht22 temp: " + String(data.temp) + " set to field 3" );
  ThingSpeak.setField(DHT_HUMID_FIELD, data.humidity);
}

void sendDs18TemperatureToThingSpeak(float temp) {
  ThingSpeak.setField(DS18_FIELD, temp);
  Serial.println( "Ds18 temp: " + String(temp) + " set to field 5" );
}

void loop() {
  // put your main code here, to run repeatedly:

}
