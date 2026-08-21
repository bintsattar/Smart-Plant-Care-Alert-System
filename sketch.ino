#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

// Hardware Pins
const int DHT_PIN = 15;
const int LDR_PIN = 34;
const int SOIL_PIN = 35;

// Calibration Thresholds
const int SOIL_DRY_THRESHOLD = 1500;   
const int LIGHT_DARK_THRESHOLD = 3000; 
const float TEMP_HEAT_THRESHOLD = 35.0; 
const float TEMP_FREEZE_THRESHOLD = 10.0; // Added Freezing Threshold

// Simulated WiFi Credentials (Wokwi standard)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Public Test MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32PlantClient-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  int ldrVal = analogRead(LDR_PIN);
  int soilVal = analogRead(SOIL_PIN);

  String status = "HEALTHY";
  if (soilVal < SOIL_DRY_THRESHOLD && data.temperature > TEMP_HEAT_THRESHOLD) {
    status = "HEAT_WAVE_DROUGHT_RISK"; 
  } else if (data.temperature < TEMP_FREEZE_THRESHOLD) {
    status = "FREEZING"; // Triggers if temperature drops below 10.0 °C
  } else if (soilVal < SOIL_DRY_THRESHOLD) {
    status = "NEEDS_WATER";
  } else if (ldrVal > LIGHT_DARK_THRESHOLD) {
    status = "NEEDS_LIGHT";
  } else if (data.temperature > TEMP_HEAT_THRESHOLD) {
    status = "TOO_HOT";
  }

  // Publish telemetry and state to MQTT topics
  client.publish("plant/monitor/soil", String(soilVal).c_str());
  client.publish("plant/monitor/light", String(ldrVal).c_str());
  client.publish("plant/monitor/temp", String(data.temperature, 1).c_str());
  client.publish("plant/monitor/status", status.c_str());

  Serial.print("Published payload status: ");
  Serial.println(status);

  delay(3000);
}
