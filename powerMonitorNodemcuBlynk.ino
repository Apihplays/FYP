#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#include <SoftwareSerial.h>
SoftwareSerial myS(13,15); //(D8,D7) ==rx,tx pin
BlynkTimer timer;

char auth[] = "use yours";
char ssid[] = "use yours";
char pass[] = "use yours";

int ledPin1 =  D4;
int ledState1 = LOW;

int ledPin2 =  LED_BUILTIN;      // the number of the LED pin
int ledState2 = LOW;             // ledState used to set the LED
int hour = 1;
float watts,volts,amps,totalEnergy,energyhour;

unsigned long period1 = 0;
unsigned long period2 = 5000; //5sec

unsigned long period3 = 0;
unsigned long period4 = 45000; //1 hour 

unsigned long previousMillis1 = 0;        // will store last time LED was updated
unsigned long previousMillis2 = 0;        // will store last time LED was updated

void setup() 
{
  Serial.begin(115200);
  myS.begin(9600);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);   
  Blynk.begin(auth, ssid, pass);
  Blynk.notify("Device started! Start to collecting data!");
}

void SerialEvent()
{
  if(myS.available()>0)
  {
       bool s = myS.find('*');
       if (s)
       {
        volts = myS.parseFloat(); //parse the value from myS to Nodemcu
        amps = myS.parseFloat();
        watts = myS.parseFloat();
       }
  }
  
  DataReceive();
} 

void DataReceive()
{   totalEnergy = ((watts*hour));
    unsigned long currentMillis = millis();
    if((ledState1 == HIGH) && (currentMillis - previousMillis1 >= period1))
    {
      ledState1 = LOW;
      previousMillis1 = currentMillis;  // Remember the time  
      Serial.print("Time: ");
      Serial.println((previousMillis1/1000) + String('s'));
      Serial.print("Volt: ");
      Serial.println(volts,4);    
      Serial.print("Current: ");
      Serial.println(amps,4);    
      Serial.print("Watt: ");
      Serial.println(watts,4);
      Serial.println(" ");
      digitalWrite(ledPin1, ledState1);  // Update the actual LED
    }
    else if((ledState1 == LOW) && (currentMillis - previousMillis1 >= period2)) //after timer reach 5 sec, value of sensor will be uploaded to Blynk server
    {
      ledState1 = HIGH;  // turn it on
      previousMillis1 = currentMillis;  // Remember the time
      digitalWrite(ledPin1, ledState1);    // Update the actual LED
      Blynk.virtualWrite(V5, volts);
      Blynk.virtualWrite(V6, amps);
      Blynk.virtualWrite(V7,watts);      
    }
     if((ledState2 == HIGH) && (currentMillis - previousMillis2 >= period3))
    {
    energyhour = (totalEnergy*1) + energyhour;
    ledState2 = LOW;  // Turn it off
    previousMillis2 = currentMillis;  // Remember the time
    digitalWrite(ledPin2, ledState2);  // Update the actual LED
    Serial.print("Jam: ");
    Serial.println(hour);
    Serial.print("energy: ");
    Serial.println(energyhour,4);
    Blynk.virtualWrite(V8,energyhour);
    Blynk.notify(("The amount of energy usage: ") + String(energyhour) + " Wh");
    }
     else if ((ledState2 == LOW) && (currentMillis - previousMillis2 >= period4)) //after timer reach 1 hour. notification will popup
    {
    ledState2 = HIGH;  // turn it on
    previousMillis2 = currentMillis;   // Remember the time
    digitalWrite(ledPin2, ledState2);    // Update the actual LED
    }
}

void loop() 
{
  SerialEvent();
  Blynk.run();
  timer.run();
}
