#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <TinyGPS++.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>

SoftwareSerial s(5, 6);
SoftwareSerial serial_connection(10, 11); // rx 10, tx 11
TinyGPSPlus gps;

MQUnifiedsensor MQ2(A0, 2);
int H2, LPG, CO, Alcohol, Propane;
float Temp;

////////////////////////////////////////////////
int BPM;
int pulseWire = 2;
int threshold = 550;
PulseSensorPlayground pulseSensor;
////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  s.begin(9600); 
  serial_connection.begin(9600);
  
  
  MQ2.inicializar();

  pulseSensor.analogInput(pulseWire);
  pulseSensor.setThreshold(threshold);

  pulseSensor.begin();
  pinMode(8, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
//  getGasData();
//  getTempData();
//  getPulseData();

  getGPS();
  delay(1000);
  
  sendDataToESP8266Mod();
//  sendGPSDataToESP();
  setupAlarmAndLed();
}

void getGPS() {
  while(serial_connection.available()) {
    gps.encode(serial_connection.read());
  }
    Serial.println("Satelite count:");
//    count = gps.satellites.value();
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
//    lati = (gps.location.lat(), 6); 
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
//    longi = (gps.location.lng(), 6);
    Serial.println(gps.location.lng(), 6);
    Serial.println("Altitude feet:");
//    height = gps.altitude.feet();
    Serial.println(gps.altitude.feet());
    Serial.println("Speed MPH:");
//    speedMPH = gps.speed.mph();
    Serial.println(gps.speed.mph());
}

void setupAlarmAndLed() {
  if(Temp > 40 || H2 > 100 || CO > 200 || LPG > 1000 ||
    Alcohol > 250 || Propane > 1000) {
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
}

void sendDataToESP8266Mod() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["data1"] = H2;
  root["data2"] = LPG;
  root["data3"] = CO;
  root["data4"] = Alcohol;
  root["data5"] = Propane;
  root["data6"] = Temp;
  root["data7"] = BPM;
//  root["data8"] = count;
//  root["data9"] = lati;
//  root["data10"] = longi;
//  root["data11"] = height;
//  root["data12"] = speedMPH;
//  Serial.println("HI");

  if(s.available() > 0) {
    root.printTo(s);
  }
}

//void sendGPSDataToESP() {
//  StaticJsonBuffer<1000> jsonBuffer;
//  JsonObject& root = jsonBuffer.createObject();
//
//  root["data8"] = count;
//  root["data9"] = lati;
//  root["data10"] = longi;
//  root["data11"] = height;
//  root["data12"] = speedMPH;
//
//  if(serial_connection.available() > 0) {
//    root.printTo(serial_connection);
//  }
//}

void getGasData() {
  MQ2.update(); 

  H2 =  MQ2.readSensor("H2"); 
  LPG =  MQ2.readSensor("LPG"); 
  CO =  MQ2.readSensor("CO"); 
  Alcohol =  MQ2.readSensor("Alcohol");
  Propane =  MQ2.readSensor("Propane");
//  Serial.println(H2);
//  Serial.println(CO);
//  Serial.println(LPG);
//  Serial.println(Alcohol);
//  Serial.println(Propane);
}

void getTempData() {
  Temp = analogRead(A1);
  Temp = Temp * .48828125;
  Serial.println(Temp);
}

void getPulseData() {
  int myBpm = pulseSensor.getBeatsPerMinute();
  Serial.println(myBpm);
  if(pulseSensor.sawStartOfBeat()) {
    BPM = myBpm;
  } else {
    BPM = -1;
  }
}
