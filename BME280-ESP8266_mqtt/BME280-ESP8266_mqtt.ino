#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

// Updates BME280 readings every 30 seconds
const long interval = 30000;

// Name of the topic
const char* topicName = "outside_1";

/* WIFI: Put your SSID & Password*/
char* ssid = "YOUR SSID";  // Enter SSID here
char* password = "YOUR PASSWORD";  //Enter Password here

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.47"; // mqtt IP Address
const char* mqtt_login = "USERNAME"; // mqtt username
const char* mqtt_password = "PASSWORD"; // mqtt password

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Fail");
    Serial.print(WiFi.status()); Serial.print(' ');
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  
  Serial.println("Setup OK");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_login, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  bme.begin(0x76);
  
  temperature = bme.readTemperature();
  
  // Convert the value to a char array
  //char tempString[8];
  //dtostrf(temperature, 1, 2, tempString);
  //Serial.print("Temperature: ");
  //Serial.println(tempString);
  //client.publish("outside_1/temperature", String(temperature).c_str(), true);

  humidity = bme.readHumidity();
  
  // Convert the value to a char array
  //char humString[8];
  //dtostrf(humidity, 1, 2, humString);
  //Serial.print("Humidity: ");
  //Serial.println(humString);
  //client.publish("outside_1/humidity", String(humidity).c_str(), true);

  pressure = bme.readPressure() / 100.0F;
  
  // Convert the value to a char array
  //char pressureString[8];
  //dtostrf(pressure, 1, 2, pressureString);
  //Serial.print("Pressure: ");
  //Serial.println(pressureString);
  //client.publish("outside_1/pressure", String(pressure).c_str(), true);

  StaticJsonDocument<256> JSONbuffer;
  JSONbuffer["temperature"] = String(temperature, 1);
  JSONbuffer["humidity"] = String(humidity, 1);
  JSONbuffer["pressure"] = String(pressure, 1);

  char buffer[256];
  serializeJson(JSONbuffer, buffer);

  if (client.publish(topicName, buffer)) {
    Serial.println("Publish ok");
  }
  else {
    Serial.println("Publish failed");
  }

  delay(interval);
}
