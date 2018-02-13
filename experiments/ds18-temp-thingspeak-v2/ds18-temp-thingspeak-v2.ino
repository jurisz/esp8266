#include <OneWire.h>
#include <DallasTemperature.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <MyWifiSecurity.h>

// JU | TP_LINK
char* WIFI_SSID = WIFI_TP_LINK_SSID;
char* WIFI_PASS = WIFI_TP_LINK_PASS;

char* thingSpeakUrl = "api.thingspeak.com";
char* apiKey = THINGSPEAK_TEST_KEY;
unsigned long channelID = THINGSPEAK_TEST_CHANNEL;

const int DEEP_SLEEP_SECONDS = 2 * 60;

const int FIELD_1 = 8;

#define ONE_WIRE_BUS 0 // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient client; 

int connectToWiFi(){
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 10) {
        WiFi.begin( WIFI_SSID, WIFI_PASS );
        counter++;
        Serial.print(".");
        delay(2000);
    }
    
    Serial.print("Wifi status: ");
    Serial.println(WiFi.status());
    
    ThingSpeak.begin( client );

    return 1;
}

float readTemperature() {
    sensors.begin();
    sensors.requestTemperatures(); 
    float temp = sensors.getTempCByIndex(0);
    Serial.println("Temperature: " + String(temp));
    return temp;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  delay(1000);

  if (connectToWiFi() > 0)  {
    float temp = readTemperature();
    if (temp > -50 && temp < 55 ) {
      sendTemperatureToThingSpeak(temp);  
    }
  }
 
  Serial.println("Done. Going for deep sleep...");
  ESP.deepSleep(DEEP_SLEEP_SECONDS * 1000000);
}


void sendTemperatureToThingSpeak(float temp) {
  writeTSData(channelID, FIELD_1, temp);
}

// Use this function if you want to write a single field
void writeTSData( long TSChannel, unsigned int TSField, float data ){
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, apiKey); // Write the data to the channel
  if ( writeSuccess ){
    Serial.println( String(data) + " written to Thingspeak." );
  } else {
    Serial.println( "Error for writing to teamspeak: " + String(data) );
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
