#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#include <SoftwareSerial.h>
SoftwareSerial myS(13,15); // (D8,D7) = rx/tx pin on nodemcu
BlynkTimer timer;

char auth[] = "insert your auth code";
char ssid[] = "your wifi";
char pass[] = "your wifi password";

//long milisec = millis(); // calculate time in milliseconds
long masa = millis()/1000; // convert milliseconds to seconds

float watts;
float volts;
float amps;
float totalEnergy;
float energyhour;
  
void setup() 
{
  Serial.begin(115200);
  myS.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Blynk.notify("Device started! Start to collecting data!");
  timer.setInterval(5000, SerialEvent); // 5 saat sekali
  timer.setInterval(300000, notifyUsage); //900k = 15min notify energy usage
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
        watts = myS.parseFloat();
        totalEnergy = (watts*masa)/(3600);
       }
  }
  DataReceive();
  yield();
} 

void DataReceive()
{   
    
    //totalEnergy[count] = (watts*masa)/3600; //reading Watt-hour
    //totalEnergy = (watts*masaskrg)/(1000*3600);//reading kWh
    energyhour = energyhour + totalEnergy;
    Serial.print("Uptime: ");
    Serial.println(masa++);
    
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
    Serial.println(energyhour,3);
    Blynk.virtualWrite(V8,energyhour);
    Serial.println(" ");
    yield();
}

void notifyUsage()
{
  Blynk.notify(("The amount of energy usage: ") + String(energyhour) + " Wh");  
}

void loop() 
{
  //SerialEvent();
  Blynk.run();
  timer.run();
}
