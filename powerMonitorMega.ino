#include "EmonLib.h"         
#define SAMPLE_COUNT 5
#define VOLT_CAL 255.4    //voltage calibration
#define CURRENT_CAL1 62.5 //sensor 1 calibration 62.2
EnergyMonitor emon1;    
          
long milisec = millis(); // calculate time in milliseconds
long masa = milisec/1000; // convert milliseconds to seconds

//arrays to hold the sample data
float volts1[SAMPLE_COUNT];
float amps1[SAMPLE_COUNT];
float watts1[SAMPLE_COUNT];
//define sensor pin number
const int currentPin = 0;
const int voltagePin = 1;

//counter to keep track of the current sample location
int counter = 0;

void setup()
{  
  Serial.begin(115200);
  Serial1.begin(9600);
  emon1.voltage(voltagePin, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(currentPin, CURRENT_CAL1);       // Current: input pin, calibration.
}

void loop()
{
  /*reset the var that keeps track of the number of samples taken 
   *(loop back around to 0 on the array for our running total) */
  if(counter >= SAMPLE_COUNT)
  {
    counter = 0;
  }

  //calculate the most recent readings
  emon1.calcVI(20,5000);               

  //save the voltage, current, watts to the array for later averaging
  amps1[counter] = emon1.Irms - 0.02; //minus some 
  volts1[counter] = emon1.Vrms;
  watts1[counter] = emon1.Vrms * emon1.Irms;
  counter++;
  
  //setup the vars to be averaged
  float wattAvg1 = 0;
  float voltAvg1 = 0;
  float ampAvg1 = 0;
  
  //add em up for averaging
  for(int i = 0; i < SAMPLE_COUNT; i++)
  {
    wattAvg1 += watts1[i];
    voltAvg1 += volts1[i];
    ampAvg1 += amps1[i];
  }

  //get the final average by dividing by the # of samples
   wattAvg1 /= SAMPLE_COUNT;
   ampAvg1 /= SAMPLE_COUNT;
   voltAvg1 /= SAMPLE_COUNT;
   
  //calculate the total amps and watts
  double totalAmp = ampAvg1;
  double totalWatt = wattAvg1;

  //send the power info to the ESP12E module through Serial
  sendPowerInfo (voltAvg1, totalAmp, totalWatt);
}

//send the power info to the ESP12E module through Serial1 (comma separated and starting with *)
void sendPowerInfo(float Volts, float Amps, float Watts)
{
  Serial1.print("*");
  Serial1.print(Volts);
  Serial1.print(",");
  Serial1.print(Amps);
  Serial1.print(",");
  Serial1.println(Watts);
  delay(1000);
}
