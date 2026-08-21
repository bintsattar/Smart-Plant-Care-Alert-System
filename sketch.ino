#include "DHTesp.h"

// Hardware Pins
const int DHT_PIN = 15;
const int LDR_PIN = 34;
const int SOIL_PIN = 35;

// Observed Sensor Limits (From Calibration Run)
// Soil: 0 (Dry) to 4095 (Wet)
// Light: 4063 (Dark) to 32 (Bright)
// Temp: -40C to 80C
const int SOIL_DRY_THRESHOLD = 1500;   
const int LIGHT_DARK_THRESHOLD = 3000; 
const float TEMP_HEAT_THRESHOLD = 35.0; 

DHTesp dhtSensor;

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  Serial.println("--- ESP32 Smart Plant Monitor Active ---");
}

void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  int ldrVal = analogRead(LDR_PIN);
  int soilVal = analogRead(SOIL_PIN);

  String status = "HEALTHY";

  // Evaluated custom condition first
  if (soilVal < SOIL_DRY_THRESHOLD && data.temperature > TEMP_HEAT_THRESHOLD) {
    status = "HEAT_WAVE_DROUGHT_RISK"; 
  } else if (soilVal < SOIL_DRY_THRESHOLD) {
    status = "NEEDS_WATER";
  } else if (ldrVal > LIGHT_DARK_THRESHOLD) {
    status = "NEEDS_LIGHT";
  } else if (data.temperature > TEMP_HEAT_THRESHOLD) {
    status = "TOO_HOT";
  }

  Serial.print("Soil: "); Serial.print(soilVal);
  Serial.print(" | Light: "); Serial.print(ldrVal);
  Serial.print(" | Temp: "); Serial.print(data.temperature, 1); Serial.print("C");
  Serial.print(" | Humidity: "); Serial.print(data.humidity, 1); Serial.print("%");
  Serial.print(" | STATUS: "); Serial.println(status);

  delay(2000);
}
