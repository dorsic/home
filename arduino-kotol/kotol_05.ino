  #include "Streaming.h"
#include "SerialCommand.h"
#include <OneWire.h>
#include <DallasTemperature.h>

//#define DEBUG
#define PROJECT "kotol_05"

#define KOTOL_OUT_PIN 13
#define THERM_IN_PIN 6
#define OW_LIVINGROOM_PIN 10
#define OW_NORTHBALC_PIN 9
#define OW_KOTOL_PIN 8
#define SERIAL_ON true
#define ON 1
#define OFF 0

#ifdef DEBUG
  #define ANTI_CYCLE_INTERVAL 30000
  #define HEATING_OFF_TIMEOUT 10000
#else
  #define ANTI_CYCLE_INTERVAL 90000
  #define HEATING_OFF_TIMEOUT 30000
#endif

#define TBALKON 0
#define TOBYVACKA7 1

#define N_THERMOSTATS 2
#define HBYT7 0
#define HBYT8 1

int heatingState = OFF;
long heatingOffAt = -ANTI_CYCLE_INTERVAL;
long heatingOnAt = -HEATING_OFF_TIMEOUT;

SerialCommand sCmd;
OneWire owNorthBalc(OW_NORTHBALC_PIN);
OneWire owLivingRoom(OW_LIVINGROOM_PIN);
OneWire owKotol(OW_KOTOL_PIN);
DallasTemperature northBalcSensors(&owNorthBalc);
DallasTemperature livingRoomSensors(&owLivingRoom);
DallasTemperature kotolSensors(&owKotol);
DeviceAddress sensorAddresses[3];

typedef struct {
  byte idx;
  const char *name;
  byte state;
} Thermostat;

// thermostats[HBYT7] is in obyvacka7 - updated via Serail1
// thermostats[HBYT8] is by default regulated with temperature sensor predspalnou

Thermostat thermostats[N_THERMOSTATS] = { 
    {HBYT7, "byt7", OFF},
    {HBYT8, "byt8", OFF}
};


// --------------------   conversions   ---------------------------

byte textToOnOff(char *state)
{
  return (strcmp(state, "ON") == 0) ? ON : OFF;
}

const char *onOffToText(byte state)
{
  return (state == ON)? "ON": "OFF";
}

byte thermostatIndex(char *name)
{
  for (byte i = 0; i < N_THERMOSTATS; i++) {
    if (strcmp(name, thermostats[i].name) == 0) return i;
  }
  return 255;
}

// --------------------   serial commands   -----------------------------

void handleHeating()
{
  char *arg;
  byte state;
  
  arg = sCmd.next();
  if (arg != NULL) {
    state = textToOnOff(arg);
    toggleHeating(state);
   } else {
      Serial << "heating 'error processing heatign command - no argument (ON | OFF)'" << endl;
   }   
}

void handleHeatingState()
{
  Serial << "htnIs " << onOffToText(heatingState) << endl;
}

void handleThermostat()
{
  char *arg1;
  char *arg2;
  byte thtIdx;
  boolean err1 = false;
  
  arg1 = sCmd.next();  
  if (arg1 == NULL) err1 = true;
  else {
    thtIdx = thermostatIndex(arg1);
    if (thtIdx >= N_THERMOSTATS) err1 = true;
    else {
       arg2 = sCmd.next();
       thermostats[thtIdx].state = textToOnOff(arg2);
    }
  }
 if (err1) {
    if (SERIAL_ON) {
       Serial << "tht 'error processing tht command  - invalid first argument (thermostat name) or state ( ON | OFF)'" << endl;
       Serial << "available thermostat names: ";
       for (byte i = 0; i < N_THERMOSTATS; i++) { Serial << thermostats[i].name << " ," ; }
       Serial << endl;
    }
  }      
}

void handleThermostatState()
{
  char *arg;
  byte thtIdx;
  boolean err1 = false;
  
  arg = sCmd.next();  
  if (arg == NULL) err1 = true;
  else {
    thtIdx = thermostatIndex(arg);
    if (thtIdx >= N_THERMOSTATS) err1 = true;
    else {
       Serial << "thtIs " << arg << " " << onOffToText(thermostats[thtIdx].state) << endl;
    }
  }
  if (err1) {
       Serial << "tht? 'error processing tht? command  - invalid first argument (thermostat name)'" << endl;
       Serial << "available thermostat names: ";
       for (byte i = 0; i < N_THERMOSTATS; i++) { Serial << thermostats[i].name << " ," ; }
       Serial << endl;
  }     
}

void handleTemperatureState() {
  char *arg = sCmd.next();
  if (arg != NULL)
  {
     if (strcmp(arg, "balkon") == 0) {
        northBalcSensors.requestTemperatures();
        float tmp = northBalcSensors.getTempCByIndex(0);
        Serial << "tmpIs balkon " <<  tmp << endl;
        return;
     }
     if (strcmp(arg, "obyvacka7") == 0) {
        livingRoomSensors.requestTemperatures();
        float tmp = livingRoomSensors.getTempCByIndex(0);
        Serial << "tmpIs obyvacka7 " <<  tmp << endl;
        return;       
     }
     if (strcmp(arg, "kotol") == 0) {
        kotolSensors.requestTemperatures();
        float tmp = kotolSensors.getTempCByIndex(0);
        Serial << "tmpIs kotol " <<  tmp << endl;
        return;       
     }     
  }
}

void handleDebug() {
  Serial << "debugIs heatingState> " << heatingState << endl;
  for (byte i = 0; i < N_THERMOSTATS; i++) Serial << "debugIs thermostats[" << i << "](" << thermostats[i].name << ") is " << onOffToText(thermostats[i].state) << endl;
  Serial << "thermostat inputPin is > " << digitalRead(THERM_IN_PIN) << endl;
  Serial << "debugIs heatingOnAt, heatingOffAt, current> " << heatingOnAt << ", " << heatingOffAt << ", " << millis() << endl;
}

void unrecognized(const char *command) {
  Serial.println("What?");
}

void initSerialCommands()
{
  sCmd.addCommand("htn", handleHeating);
  sCmd.addCommand("htn?", handleHeatingState);
  sCmd.addCommand("tht", handleThermostat);
  sCmd.addCommand("tht?", handleThermostatState);  
  sCmd.addCommand("tmp?", handleTemperatureState);
  sCmd.addCommand("debug?", handleDebug);
  sCmd.setDefaultHandler(unrecognized);
}

/////////////////////////////////////

void setup()
{
  pinMode(THERM_IN_PIN, INPUT_PULLUP);
  pinMode(KOTOL_OUT_PIN, OUTPUT);  
  if (SERIAL_ON) {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }

    #ifdef DEBUG
      Serial << PROJECT << " started in DEBUG." << endl;
    #else 
      Serial << PROJECT << " started." << endl;    
    #endif
  }
  
  northBalcSensors.begin();
  northBalcSensors.getAddress(sensorAddresses[0], 0);
  northBalcSensors.setResolution(sensorAddresses[0], 12);
  livingRoomSensors.begin();
  livingRoomSensors.getAddress(sensorAddresses[1], 0);
  livingRoomSensors.setResolution(sensorAddresses[1], 12);
  kotolSensors.begin();
  kotolSensors.getAddress(sensorAddresses[2], 0);
  kotolSensors.setResolution(sensorAddresses[2], 12);
  
  if (SERIAL_ON) {
    initSerialCommands();
    Serial << "Serial commands initialized." << endl;
  }

}

void toggleHeating(int state)
{
  heatingState = state;
  digitalWrite(KOTOL_OUT_PIN, heatingState);
  if (state == ON) heatingOnAt = millis();
  if (state == OFF) heatingOffAt = millis();
  if (SERIAL_ON) handleHeatingState();
}

void updateThermostat()
{
  int thermoState = digitalRead(THERM_IN_PIN);
  if (thermoState == LOW) { thermoState = HIGH; }
  else { thermoState = LOW; }
  if (thermostats[HBYT7].state != thermoState) {
    thermostats[HBYT7].state = thermoState;
    if (SERIAL_ON) Serial << "thtIs byt7 " << onOffToText(thermostats[HBYT7].state) << endl;
  }
}

void updateHeating()
{
  boolean someON = false;
  boolean allOFF = true;
  
  for (byte i = 0; i < N_THERMOSTATS; i++)
  {
      someON |= (thermostats[i].state == ON);
      allOFF &= (thermostats[i].state == OFF);
  }
  
  if (someON) {
     if (heatingState == OFF && (millis() - heatingOffAt >= ANTI_CYCLE_INTERVAL)) {
        toggleHeating(ON);
     } 
  }
  
  if (allOFF) {
     if (heatingState == ON && (millis() - heatingOnAt > HEATING_OFF_TIMEOUT)) {
        toggleHeating(OFF);
     }
  }  
}

void loop()
{
  if (SERIAL_ON && Serial.available() > 0) {
    sCmd.readSerial();
  } else {
  }

  updateThermostat();
  updateHeating();
}
