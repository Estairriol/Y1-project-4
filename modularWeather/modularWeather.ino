//om634 ac3499 kv395

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "UoB-IoT";    
const char* password = "mhczaw4z"; // password for this mac address
const char* baseLocUrl = "https://api.ip2location.io/?key=AC1CDE03F2C2BF7AA758D9C65EBD9A0D&ip=";
const char* weatherUrl1 = "https://api.openweathermap.org/data/2.5/weather?lat=";
const char* weatherUrl2 = "&lon=";
const char* weatherUrl3 = "&appid=76cc97551193b80ac3483e2faaadfac4";

String ip;
String locData;
double loc[2];
String weaData;


void connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  ip = WiFi.localIP().toString();
}

String getHTML(const char* url) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
      return payload;
    } else {
      Serial.println("Error on HTTP request");
      return "";
    }
    http.end();
  } else {
    Serial.println("Not connected to WiFi");
    return "";
  }
}

void setup() {
  Serial.begin(9600);
  connect();

  String fullLocUrl = String(baseLocUrl) + ip;
  locData = getHTML(fullLocUrl.c_str());
  JSONVar myObject = JSON.parse(locData);
  loc[0] = myObject["latitude"];
  loc[1] = myObject["longitude"];
  Serial.println(loc[0]);
}


void loop() {
  String fullweatherUrl = String(weatherUrl1) + loc[0] + String(weatherUrl2) + loc[1] + String(weatherUrl3);
  weaData = getHTML(fullweatherUrl.c_str());
  JSONVar myObject = JSON.parse(weaData);
  String weaType = myObject["weather"][0]["main"];
  Serial.println(weaType);

  delay(10000);
}
