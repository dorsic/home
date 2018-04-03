

/* TODO:
*/

/*
  
senzor na dvere chce 47k odpor
je to modrp-=biely kablik z kuchyna 1W

*/
//#define DEBUG

#include <SoftwareSerial.h>
#include "ClickButton.h"
#include "Streaming.h"
#include <SerialCommand.h>
#include "Serial1Command.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "commons.h"

#define PROJECT "hr_07"

SerialCommand sCmd;
Serial1Command sCmd1;

// ---------------------------   MAINS  ----------------------------

void setup()
{
  delay(4000);  

  setup_serial();
  setup_serial1();
  
  setup_lights();
  setup_temperatures();
  setup_powerMeter();
  setup_thermostat();  
  
  initSerial0LightCommands();
  initSerial0TmpCommands();
  initSerial0ThtCommands();
  initSerialCommands();  
  
  initSerial1Commands();  
}

void loop()
{
  updateSerial();
  updateSerial1();
  updateLightSwitches();
  updateLights();
  updateTemperatures();
  updateThermostat();
}
