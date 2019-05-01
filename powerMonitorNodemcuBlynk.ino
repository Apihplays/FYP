#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
BlynkTimer timer;
#include <SoftwareSerial.h>
SoftwareSerial myS(13,15); //(D8,D7) / rx/tx

char auth[] = "-";
char ssid[] = "-";
char pass[] = "-";

long milisec = millis(); // calculate time in milliseconds
long masa = milisec/1000; // convert milliseconds to seconds

  double watts;
  double volts;
  double amps;
  double totalEnergy;
  
void setup() 
{
  Serial.begin(115200);
  myS.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Blynk.notify("Device started! Start to collecting data!");
  //timer.setInterval(1000L, DataReceive);
}

void SerialEvent()
{
  if(myS.available()>0)
  {
       bool s = myS.find('*');
       if (s)
       {
        volts = myS.parseFloat();
        amps = myS.parseFloat();
        watts= myS.parseFloat();
       }
  }
  DataReceive();
} 

void DataReceive()
{    
    //totalEnergy =totalEnergy + (watts*masa)/3600; //reading Watt-hour
    totalEnergy = totalEnergy +(watts*masa)/(1000*3600);//reading kWh
    
    Serial.print("Volt: ");
    Serial.println(volts,3);
    Blynk.virtualWrite(V5, volts);
    
    Serial.print("Current: ");
    Serial.println(amps,3);
    Blynk.virtualWrite(V6, amps);
    
    Serial.print("Watt: ");
    Serial.println(watts,3);
    Blynk.virtualWrite(V7,watts);
    
    Serial.print("Energy: ");
    Serial.println(totalEnergy,3);
    Blynk.virtualWrite(V8,totalEnergy);
    Serial.println(" ");
    delay(1000);
}

void loop() 
{
  SerialEvent();
  Blynk.run();
  timer.run();
}
