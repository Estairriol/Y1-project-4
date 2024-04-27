//om634 ac3499 kv395

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "UoB-IoT";    
const char* password = "rt9crsw5"; // password for this mac address
const char* baseLocUrl = "https://api.ip2location.io/?key=AC1CDE03F2C2BF7AA758D9C65EBD9A0D&ip=";
const char* weatherUrl1 = "https://api.openweathermap.org/data/2.5/weather?lat=";
const char* weatherUrl2 = "&lon=";
const char* weatherUrl3 = "&appid=76cc97551193b80ac3483e2faaadfac4";

String ip;
String locData;
double loc[2];
String weaData;
String weaType;
String dayNight;
//1 = blue sun
//2 = white cloud
//3 = blue rain
//4 = green wind 
//5 = white moon
//6 = blue snow
int sun = 1;
int cloud = 2;
int rain = 3;
int wind = 4;
int moon = 5;
int snow = 6;
int switchPin = 7;


unsigned long prevBlink = 0;
const long interval = 1000*10;


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

void doWeather(){
  String fullweatherUrl = String(weatherUrl1) + loc[0] + String(weatherUrl2) + loc[1] + String(weatherUrl3);
  weaData = getHTML(fullweatherUrl.c_str());
  JSONVar myObject = JSON.parse(weaData);
  String weaIcon = myObject["weather"][0]["icon"];
  Serial.println(weaIcon);
  weaType = weaIcon.substring(0,2);
  dayNight = weaIcon.substring(2,3);

  digitalWrite(sun, LOW);
  digitalWrite(cloud, LOW);
  digitalWrite(rain, LOW);
  digitalWrite(wind, LOW);
  digitalWrite(moon, LOW);
  digitalWrite(snow, LOW);

  if (weaType == "01") {
    Serial.println("sun");
    digitalWrite(sun, HIGH);
  } else if (weaType == "02") {
    Serial.println("sun");
    Serial.println("cloud");
    digitalWrite(sun, HIGH);
    digitalWrite(cloud, HIGH);
  } else if (weaType == "03" || weaType == "04") {
    Serial.println("cloud");
    digitalWrite(cloud, HIGH);
  } else if (weaType == "09") {
    Serial.println("cloud");
    Serial.println("rain");
    digitalWrite(cloud, HIGH);
    digitalWrite(rain, HIGH);
  } else if (weaType == "10" || weaType == "11") {
    Serial.println("sun");
    Serial.println("cloud");
    Serial.println("rain");
    digitalWrite(sun, HIGH);
    digitalWrite(cloud, HIGH);
    digitalWrite(rain, HIGH);
  } else if (weaType == "13" || weaType == "50") {
    Serial.println("snow");
    digitalWrite(snow, HIGH);
    digitalWrite(cloud, HIGH);
  } else {
    // Handle the default case here
  }

  if (dayNight == "d") {
    Serial.println("Day");
    digitalWrite(sun, HIGH);
  } else if (dayNight == "n") {
    Serial.println("Night");
    digitalWrite(moon, HIGH);
  } else {
    // Handle the default case here
  }
}

void setup() {
  Serial.begin(9600);
  connect();

  pinMode(sun, OUTPUT);
  pinMode(cloud, OUTPUT);
  pinMode(rain, OUTPUT);
  pinMode(wind, OUTPUT);
  pinMode(moon, OUTPUT);
  pinMode(snow, OUTPUT);
  pinMode(switchPin, INPUT);


  String fullLocUrl = String(baseLocUrl) + ip;
  locData = getHTML(fullLocUrl.c_str());
  JSONVar myObject = JSON.parse(locData);
  loc[0] = myObject["latitude"];
  loc[1] = myObject["longitude"];
  Serial.println(loc[0]);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    doWeather();
  }
}
