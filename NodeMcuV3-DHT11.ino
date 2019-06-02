#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11 // or DHT11, DHT22
DHT dht(DHTPIN, DHTTYPE);

int led = 13;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  delay(500);
  Serial.println("Czujnik startuje");
  dht.begin();
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // for farenheit dht.readTemperature(true)
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C ");
}
