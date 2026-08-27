#include "dexcom.h"
#include "secrets.h"
#include <WiFi.h>

bool connectWiFi(const char* ssid, const char* password, unsigned long timeoutMs = 15000) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > timeoutMs) {
            return false;  // Timeout
        }
        delay(500);
    }

    return true;
}

Dexcom dexcom(SECRET_USER, SECRET_PASS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  Serial.println("Test");
  delay(1000);

  WiFi.mode(WIFI_STA);
  Serial.print("MAC ADDRESS:");
  Serial.println(WiFi.macAddress());

  Serial.println("Attempting WiFi connection");
  connectWiFi(WIFI_NAME, WIFI_PASS);

  if (WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi connection active!");
  } else {
    Serial.println("WiFi connection failure");
  }

  int tries = 3;
  while (tries > 0){
    Serial.println("Attempting login...");
    if (dexcom.login()) {
      Serial.println("Login success");
      tries=0;
    } else {
      Serial.println("Dexcom login failure");
      tries -= 1;
      delay(10000);
    }
  }

  //LED pin for testing
  pinMode(2, OUTPUT);
}

#define ALERT_NONE 0
#define ALERT_LOW 1
#define ALERT_HIGH 2

int alertStatus = ALERT_NONE;

void loop() {
  // put your main code here, to run repeatedly:
  while (dexcom.isLoggedIn()) {
    delay(100);
    DexcomGlucose reading = dexcom.getLatestGlucose();
    int reading_value = reading.value;
    Serial.print(reading.timestamp); Serial.print(" "); Serial.print(reading.trend); + Serial.print(" ") + Serial.println(reading.value);
    if (reading_value <= 80) {
      alertStatus = ALERT_LOW;
      digitalWrite(2,1);
      Serial.println("Active alert! (Low Glucose)");
    }
    else if (reading_value >= 280) {
      alertStatus = ALERT_HIGH;
      digitalWrite(2,1);
      Serial.println("Active alert! (High Glucose)");
    }
    else {
      alertStatus = ALERT_NONE;
      digitalWrite(2,0);
    }
    delay(10000);
    // digitalWrite(2,1);
  }
}
