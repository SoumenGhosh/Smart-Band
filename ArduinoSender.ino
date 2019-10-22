#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>

SoftwareSerial s(5, 6);
SoftwareSerial serial_connection(10, 11);

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
  
  getGasData();
  getTempData();
  getPulseData();

  delay(1000);
  
  sendDataToESP8266Mod();
  setupAlarmAndLed();
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

  if(s.available() > 0) {
    root.printTo(s);
  }
}

void getGasData() {
  MQ2.update(); 

  H2 =  MQ2.readSensor("H2"); 
  LPG =  MQ2.readSensor("LPG"); 
  CO =  MQ2.readSensor("CO"); 
  Alcohol =  MQ2.readSensor("Alcohol");
  Propane =  MQ2.readSensor("Propane");
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
