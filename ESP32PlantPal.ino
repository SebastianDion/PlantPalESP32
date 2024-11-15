#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "thomski"
#define WIFI_PASSWORD "thomas8112004"

#define FIREBASE_HOST "https://plantpal-ff2b6-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "1vIgdhsJMQ3Q3KA5BVpI8XOgTEuqXBkIteHIWWPo"

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

const int moistureSensorPin = 34; 
const int uvSensorPin = 33;        
const int relayPin = 15;            

int moistureThreshold = 2000; 
int uvThreshold = 1100;       

int soilMoistureValue = 0;
int uvSensorValue = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  soilMoistureValue = analogRead(moistureSensorPin);
  uvSensorValue = analogRead(uvSensorPin);

  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);
  Serial.print("UV Sensor: ");
  Serial.println(uvSensorValue);

  if (soilMoistureValue > moistureThreshold && uvSensorValue < uvThreshold) {
    digitalWrite(relayPin, LOW);
    Serial.println("Pump ACTIVE");
    delay(7000);
    digitalWrite(relayPin, HIGH);
  } 
  else {
    digitalWrite(relayPin, HIGH);
    Serial.println("Pump OFF");
  }

  if (Firebase.setInt(firebaseData, "/sensorData/soilMoisture", soilMoistureValue)) {
    Serial.println("Soil Moisture sent to Firebase");
  } else {
    Serial.print("Failed to send data: ");
    Serial.println(firebaseData.errorReason());
  }


  if (Firebase.setInt(firebaseData, "/sensorData/uvSensor", uvSensorValue)) {
    Serial.println("UV Sensor sent to Firebase");
  } else {
    Serial.print("Failed to send data: ");
    Serial.println(firebaseData.errorReason());
  }

  delay(2000);
}