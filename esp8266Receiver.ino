#include <SoftwareSerial.h>
SoftwareSerial s(D6, D5);
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include "ESP8266WiFi.h" 
#include "FirebaseArduino.h" 

SoftwareSerial serial_connection(D1, D2); 
TinyGPSPlus gps;

//int data;
////////////////////////////////////////////////////////////////////////////////
#define FIREBASE_HOST "project1-4486c.firebaseio.com" 
#define FIREBASE_AUTH "36hyIUmvtaQp6sY3GVcp6hgAske2fFyiEaFWlkLb" 

#define WIFI_SSID "Sg106" 
#define WIFI_PASSWORD "sou007Sg!06"
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
int count;
double lati, longi, height, speedMPH;
////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  s.begin(9600);
  serial_connection.begin(4800);
  setupWifi();
  while(!Serial) continue;
}

void setupWifi(){
  Serial.begin (9600);
  delay (1000);
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD); 

  while (WiFi.status()!= WL_CONNECTED) {
    Serial.print (".");
    delay (500);
  }
  Serial.println("Connected");
  Firebase.begin (FIREBASE_HOST, FIREBASE_AUTH); 
}

void loop() {
  getDataFromArduino();
  setupGPS();
  delay(1000);
}

void setupGPS() {
  while(serial_connection.available()) {
    gps.encode(serial_connection.read());
  }
  Serial.println("HI");
    Serial.println("Satelite count:");
    count = gps.satellites.value();
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    lati = (gps.location.lat(), 6); 
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    longi = (gps.location.lng(), 6);
    Serial.println(gps.location.lng(), 6);
    Serial.println("Altitude feet:");
    height = gps.altitude.feet();
    Serial.println(gps.altitude.feet());
    Serial.println("Speed MPH:");
    speedMPH = gps.speed.mph();
    Serial.println(gps.speed.mph());
    delay(500);

  Firebase.setString("All Values/SateLite Count", String(count));
  Firebase.setString("All Values/Latitude", String((gps.location.lat())));
  Firebase.setString("All Values/Longitude", String(longi));
  Firebase.setString("All Values/Altitude feet", String(height));
  Firebase.setString("All Values/Speed MPH", String(speedMPH));
}


void getDataFromArduino() {
  // put your main code here, to run repeatedly:
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if(root == JsonObject::invalid()) {
    return;
  }
  Serial.println("Json received and parsed");
  root.prettyPrintTo(Serial);

  Serial.print("H2 :");
  int data1 = root["data1"];
  Serial.print(data1);
  
  Serial.print("    LPG:");
  int data2 = root["data2"];
  Serial.print(data2);
  
  Serial.print("    CO:");
  int data3 = root["data3"];
  Serial.print(data3);
  
  Serial.print("    Alcohol:");
  int data4 = root["data4"];
  Serial.print(data4);
  
  Serial.print("    Propane:");
  int data5 = root["data5"];
  Serial.print(data5);

  Serial.print("    Temp:");
  float data6 = root["data6"];
  Serial.print(data6);
  Serial.print("   ");
  Serial.print((data6 * 9 / 5) + 32);

  Serial.print("    BPM:");
  int data7 = root["data7"];
  Serial.print(data7);
  
  Serial.println("");
  Serial.println("---------------------xxx-------------------------------------xxx----------------");

  // Firebase
  String firebaseH2 = String(data1) + String(" ppm");
  String firebaseLPG = String(data2) + String(" ppm");
  String firebaseCO = String(data3) + String(" ppm");
  String firebaseAlcohol = String(data4)+String(" ppm");
  String firebasePropane = String(data5)+String(" ppm");
  String firebaseTemp = String(data6)+String(" *C ")+String((data6 * 9 / 5) + 32)+String(" *F");
  String firebasePulse = String(data7)+String(" BPM");
  
  Firebase.setString("All Values/H2", firebaseH2);
  Firebase.setString("All Values/CO", firebaseCO);
  Firebase.setString("All Values/LPG", firebaseLPG);
  Firebase.setString("All Values/Alcohol", firebaseAlcohol);
  Firebase.setString("All Values/Propane", firebasePropane);
  Firebase.setString("All Values/Temp", firebaseTemp);
  Firebase.setString("All Values/BPM", firebasePulse);
  
  Firebase.pushString("H2", firebaseH2);
  Firebase.pushString("CO", firebaseCO);
  Firebase.pushString("LPG", firebaseLPG);
  Firebase.pushString("Alcohol", firebaseAlcohol);
  Firebase.pushString("Propane", firebasePropane);
  Firebase.pushString("Human Body Temp", firebaseTemp);
  Firebase.pushString("Human Body Pulse", firebasePulse);
  
}
