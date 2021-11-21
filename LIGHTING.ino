#include "EspMQTTClient.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


int criticalTemmp = 20;
unsigned long time4 = 0;


EspMQTTClient client(
  "Smartness",
  "acdb2022",
  "192.168.84.99",  // MQTT Broker server ip
  "bobur",   // Can be omitted if not needed
  "bobur",   // Can be omitted if not needed
  "wemos",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

#define ONE_WIRE_BUS D6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int temp1 = 0;

void setup()
{
  Serial.begin(115200);

  dht.begin();

  client.enableDebuggingMessages();
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  sensors.begin();

  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  client.publish("line1", "false");
  client.publish("line2", "false");
  client.publish("line3", "false");
  client.publish("cooler", "false");

}

void onConnectionEstablished()
{
  client.subscribe("line1", [](const String & payload) {
    Serial.println(payload);
    if (payload == "true") {
      digitalWrite(D5, LOW);
    }
    if (payload == "false") {
      digitalWrite(D5, HIGH);
    }
  });
  client.subscribe("line2", [](const String & payload) {
    Serial.println(payload);
    if (payload == "true") {
      digitalWrite(D6, HIGH);
    }
    if (payload == "false") {
      digitalWrite(D6, LOW);
    }
  });
  client.subscribe("line3", [](const String & payload) {
    Serial.println(payload);
    if (payload == "true") {
      digitalWrite(D7, HIGH);
    }
    if (payload == "false") {
      digitalWrite(D7, LOW);
    }
  });

  client.subscribe("ctemp", [](const String & payload) {
    Serial.println(payload);
    criticalTemmp = payload.toInt();
  });

  



  client.executeDelayed(2000, []() {
    String payload1 = String(temp1);
    Serial.println(payload1);
    client.publish("temp1", payload1);
  });

}
void loop()
{



  client.loop();
  if (time4 + 2000 < millis()) {
    time4 = millis();
    String payload1 = String(temp1);
    client.publish("temp1", payload1);

    temp1 = dht.readTemperature();

    if (temp1 > criticalTemmp) {
      client.publish("cooler", "true");
      digitalWrite(D8, HIGH);
    } else {
      client.publish("cooler", "false");
      digitalWrite(D8, LOW);
    }


  }
}
