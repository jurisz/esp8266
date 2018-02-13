#include <ESP8266WiFi.h>
#include <MyWifiSecurity.h>
#include "ThingSpeak.h"
#include "DHT.h"

// JU | TP_LINK
char* WIFI_SSID = WIFI_TP_LINK_SSID;
char* WIFI_PASS = WIFI_TP_LINK_PASS;

char* thingSpeakUrl = "api.thingspeak.com";
char* apiKey = THINGSPEAK_TEST_KEY;
unsigned long channelID = THINGSPEAK_TEST_CHANNEL;

const FIELD_TEMP = 1;
const FIELD_HUMID=2;

const int DEEP_SLEEP_SECONDS = 15 * 60;

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

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
    SensorData data = readSensor();
    if (isnan(data.humidity) || isnan(data.temp)) {
      Serial.println("No sensor data");
    } else {
      sendTemperatureToThingSpeak(data); 
    }
  }
 
  Serial.println("Done. Going for deep sleep...");
  ESP.deepSleep(DEEP_SLEEP_SECONDS * 1000000);
}

void sendTemperatureToThingSpeak(SensorData data) {
  ThingSpeak.setField(FIELD_TEMP, data.temp);
  ThingSpeak.setField(FIELD_HUMID, data.humidity);
  int  writeSuccess = ThingSpeak.writeFields(channelID, apiKey); 
    if ( writeSuccess ){
    Serial.println( "Temp: " + String(data.temp) + " written to Thingspeak." );
  } else {
    Serial.println( "Error for writing to teamspeak code: " + writeSuccess );
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
