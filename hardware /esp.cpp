#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include "DHT.h"

// ===== CONFIG =====
#define WIFI_SSID  "PEARL ROSARY SCHOOL"
#define WIFI_PASS  "01234567"

#define DHTPIN 4
#define DHTTYPE DHT11

String firebaseHost = "https://aeroview-7a470-default-rtdb.firebaseio.com/";

DHT dht(DHTPIN, DHTTYPE);

// ===== Variables for averaging =====
float tempReadings[30];
float humReadings[30];
int indexCounter = 0;

unsigned long lastRead = 0;

// ================= SETUP ===================
void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Sync NTP time for date
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
}

// ================= LOOP ===================
void loop() {
  if (millis() - lastRead >= 2000) {
    lastRead = millis();
    readSensor();
  }
}

// ================ SENSOR READ =================
void readSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("DHT ERROR!");
    return;
  }

  tempReadings[indexCounter] = t;
  humReadings[indexCounter] = h;
  indexCounter++;

  Serial.printf("Reading %d -> T: %.1f H: %.1f\n", indexCounter, t, h);

  if (indexCounter >= 30) {
    float avgT = average(tempReadings, 30);
    float avgH = average(humReadings, 30);

    Serial.println("---------");
    Serial.printf("Avg Temp: %.2f\n", avgT);
    Serial.printf("Avg Hum : %.2f\n", avgH);
    Serial.println("Uploading...");
    Serial.println("---------");

    uploadData(avgT, avgH);

    indexCounter = 0;
  }
}

float average(float arr[], int size) {
  float sum = 0;
  for (int i = 0; i < size; i++) sum += arr[i];
  return sum / size;
}

// ================ DATE STRING =================
String getDateString() {
  time_t now = time(nullptr);
  while (now < 100000) {   // wait until synced
    delay(100);
    now = time(nullptr);
  }

  struct tm *t = localtime(&now);

  char dateStr[15];
  sprintf(dateStr, "%04d-%02d-%02d", 
          t->tm_year + 1900, 
          t->tm_mon + 1, 
          t->tm_mday);

  return String(dateStr);
}

// ================ UPLOAD DATA =================
void uploadData(float temp, float hum) {

  String datePath = getDateString();
  int nextIndex = getNextIndex(datePath);
  if (nextIndex <= 0) return;

  String path = "DATA/" + datePath + "/" + String(nextIndex) + ".json";
  String url = firebaseHost + path;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"hum\":" + String(hum);
  json += "}";

  int code = http.PUT(json);
  Serial.printf("Upload code: %d\n", code);

  if (code > 0) Serial.println(http.getString());
  http.end();
}

// ================ GET NEXT INDEX =================
int getNextIndex(String datePath) {

  String url = firebaseHost + "DATA/" + datePath + ".json";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, url);

  int code = http.GET();
  Serial.printf("Index GET code: %d\n", code);

  if (code != 200) {
    http.end();
    return 1;
  }

  String payload = http.getString();
  http.end();

  if (payload == "null") return 1;

  int maxIndex = 0;
  String key = "";

  for (int i = 0; i < payload.length(); i++) {

    if (isdigit(payload[i])) {
      key += payload[i];
    } else {
      if (key.length() > 0) {
        int num = key.toInt();
        if (num > maxIndex) maxIndex = num;
        key = "";
      }
    }
  }

  return maxIndex + 1;
}