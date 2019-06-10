#include "firebase.h"
#include "WifiConnect.h"
#include "NTPcode.h"

#ifndef DHT_H
#include "DHT.h"
#endif

#define DHTPIN 2
#define DHTTYPE DHT11 // or DHT11, DHT22
DHT dht(DHTPIN, DHTTYPE);


int led = 13;
bool isWiFiOn;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  delay(500);
  isWiFiOn = WifiConnect();
}

void loop() {
  if (isWiFiOn) {
  Serial.println("Sensor on");
    dht.begin();
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // for farenheit dht.readTemperature(true)
    if (!isnan(h) && !isnan(t)) {
      unsigned long timeStamp = getTimeFromNTP();
      if (timeStamp > 0) {
        Serial.print(timeStamp);
        Serial.print("    ");
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print("%  Temperature: ");
        Serial.print(t);
        Serial.println("°C ");
        if(pushToBase(String("{\"hum\":") + String(h) + ", \"temp\":" + String(t) + ", \"time\":" + String(timeStamp) + "}")) {
          Serial.println("I go slip for 1h"); // https://kalkulator.pro/ca/hour-to-microsecond.html
          ESP.deepSleep(3600000000);
          delay(10000);
          return;
        } else {
          Serial.println("We cannot send data to Firebase :(");
        }
      }
    }
  }
  Serial.println("We try again for 10s");
  delay(10000);
}
