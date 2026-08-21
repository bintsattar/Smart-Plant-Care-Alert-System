#include "DHTesp.h"

const int DHT_PIN = 15;
const int LDR_PIN = 34;
const int SOIL_PIN = 35;

DHTesp dhtSensor;

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  Serial.println("--- Sensor Calibration Mode ---");
}

void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  int ldrVal = analogRead(LDR_PIN);
  int soilVal = analogRead(SOIL_PIN);

  Serial.print("Soil Raw: ");
  Serial.print(soilVal);
  Serial.print(" | Light Raw: ");
  Serial.print(ldrVal);
  Serial.print(" | Temp: ");
  Serial.print(data.temperature, 1);
  Serial.print("C | Humidity: ");
  Serial.print(data.humidity, 1);
  Serial.println("%");

  delay(1500);
}
