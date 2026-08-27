#ifndef DEXCOM_H
#define DEXCOM_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct DexcomGlucose {
    int value;
    String trend;
    String timestamp;
};

class Dexcom {
public:
    Dexcom(const char* username, const char* password);

    bool login();
    bool isLoggedIn();
    DexcomGlucose getLatestGlucose();

private:
    const char* _username;
    const char* _password;

    String _sessionId;
    String _accountId;

    const char* _baseUrl =
      "https://share2.dexcom.com/ShareWebServices/Services";

    WiFiClientSecure _client;
    HTTPClient _https;

    bool authenticate();
    bool loginAccount();
};

#endif