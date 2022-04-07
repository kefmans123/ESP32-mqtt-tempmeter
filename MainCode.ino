#define DHTPIN 18
#define DHTTYPE DHT11

#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "iPhone van Tom";
const char* password = "school2019";
const int ledPin = 19;

const char* mqtt_server = "172.20.10.12";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
  pinMode (ledPin, OUTPUT);

  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setCallback(callback);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
        client.publish("ESP32/online", "Online");
        client.subscribe("ESP32/change_led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  if (String(topic) == "ESP32/change_led") {
    Serial.print("Changing output to ");
    if(messageTemp == "true"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "false"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void loop() {
  if (Serial.available()) {
}
//  Serial.println(now());
  if (!client.connected()) {
    reconnect();
  } 
  client.loop();
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
     float t = dht.readTemperature();
     if (isnan(t)){
      Serial.println("The sensor isn't working. Please check the sensor");
      client.publish("ESP32/temp", "0");
     }

    client.publish("ESP32/temp", String(t).c_str());
  }
}
