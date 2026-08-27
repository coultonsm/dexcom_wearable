#include "Dexcom.h"

// Dexcom Application ID for the United States
#define APPLICATION_ID "d89443d2-327c-4a6f-89e5-496bbb0317db"

/*
 * Google Trust Services - WE1 Intermediate Certificate
 */
static const char GOOGLE_WE1_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw
MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp
Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+
Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP
LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr
BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU
kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F
avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku
Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv
ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx
AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V
sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN
7uJW
-----END CERTIFICATE-----
)EOF";

Dexcom::Dexcom(const char* username, const char* password)
    : _username(username), _password(password) {

    // Secure TLS using embedded CA certificate
    _client.setCACert(GOOGLE_WE1_CA);
}

bool Dexcom::authenticate() {
    String url = String(_baseUrl) + "/General/AuthenticatePublisherAccount";

    _https.begin(_client, url);
    _https.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["accountName"] = _username;
    doc["password"] = _password;
    doc["applicationId"] = APPLICATION_ID;

    String body;
    serializeJson(doc, body);

    int httpCode = _https.POST(body);

    Serial.print("Auth HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode <= 0) {
        _https.end();
        return false;
    }

    // ✅ READ ONCE
    String response = _https.getString();
    Serial.println(response);

    if (httpCode == 200) {
        _sessionId = response;
        _sessionId.replace("\"", "");
        Serial.print("Session ID: ");
        Serial.println(_sessionId);
        _https.end();
        return true;
    }

    _https.end();
    return false;
}

bool Dexcom::loginAccount() {
    String url = String(_baseUrl) + "/General/LoginPublisherAccountById";

    _https.begin(_client, url);
    _https.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["accountId"] = _sessionId;
    doc["password"] = _password;
    doc["applicationId"] = APPLICATION_ID;

    String body;
    serializeJson(doc, body);

    int httpCode = _https.POST(body);

    Serial.print("Login HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode <= 0) {
        _https.end();
        return false;
    }

    // ✅ READ ONCE
    String response = _https.getString();
    Serial.println(response);

    if (httpCode == 200) {
        _accountId = response;
        _accountId.replace("\"", "");
        _https.end();
        return true;
    }

    _https.end();
    return false;
}

bool Dexcom::login() {
    if (!authenticate()) return false;
    return loginAccount();
}

bool Dexcom::isLoggedIn() {
    return _accountId.length() > 0;
}

DexcomGlucose Dexcom::getLatestGlucose() {
    DexcomGlucose reading;
    reading.value = -1;
    reading.trend = "UNKNOWN";
    reading.timestamp = "";

    if (!isLoggedIn()) {
        return reading;
    }

    String url = String(_baseUrl) +
        "/Publisher/ReadPublisherLatestGlucoseValues"
        "?sessionId=" + _accountId +
        "&minutes=10&maxCount=1";

    _https.begin(_client, url);

    int httpCode = _https.GET();

    if (httpCode == 200) {
        String payload = _https.getString();

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            reading.value = doc[0]["Value"] | -1;
            reading.trend = doc[0]["Trend"] | "UNKNOWN";
            reading.timestamp = doc[0]["WT"] | "";
        }
    }

    _https.end();
    return reading;
}