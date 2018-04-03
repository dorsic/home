  
#define TEMP_THRESHOLD 0.1  // 0.1 degC
#define THT_READING_INTEVAL 60000

// thermostatState[0] is in obyvacka7 - updated via Serail1

byte thermState = OFF;
unsigned long lastThtReading = 0;

void setup_thermostat() {
  for (byte i = 0; i < N_THERMOSTATS; i++) {
     if (thermostat[i].reqT != RQT_UNDEFINED) {
        Serial << "rqtIs " << i << " " << getRequiredTemperature(i) << endl;
     }
  }
  if (SERIAL1_ON) Serial1 << "tht? " << thermostat[THTINT].name << endl;
}

void setRequiredTemperature(byte index, float temp)
{
    thermostat[index].reqT = (short)(temp * 1000.0);
}

float getRequiredTemperature(byte index)
{
    return ((float)thermostat[index].reqT) / 1000.0;
}


void updateThermostat() {
  float curTemperature;
  byte isSomeOn = OFF;
  boolean needToHeatUp = false;
  boolean needToColdDown = false;
  
  for (byte i = 0; i < N_THERMOSTATS-N_THT_KOTOL; i++)
  {
     if (thermostat[i].reqT != RQT_UNDEFINED) {
       curTemperature = getTemperature(thermostat[i].tempSensor);
       if (8.0 < curTemperature && curTemperature < 32.0) { 
         needToHeatUp |= (curTemperature < (getRequiredTemperature(i)-TEMP_THRESHOLD));
         needToColdDown |= (curTemperature > (getRequiredTemperature(i)+TEMP_THRESHOLD));
         if ((thermostat[i].state == OFF || thermostat[i].state == BYTE_UNDEFINED) && needToHeatUp) {
            toggleThermostat(i, ON);
          } else {
            if (thermostat[i].state == ON && needToColdDown && !needToHeatUp) toggleThermostat(i, OFF);
          }
       }       
     } //else thermostat[i].state = OFF;
     if (thermostat[i].state == ON) isSomeOn = ON;
  }

  if (thermostat[THTEXT].state != isSomeOn) {
    thermostat[THTEXT].state = isSomeOn;
    if (SERIAL1_ON) Serial1 << "tht " << thermostat[THTEXT].name << " " << onOffToText(isSomeOn) << endl;
  }

  if ((millis() - lastThtReading >= THT_READING_INTEVAL))
  {
    if (SERIAL1_ON) Serial1 << "tht? " << thermostat[THTINT].name << endl;
    lastThtReading = millis();
  }
}

void toggleThermostat(byte index, byte value) {
  thermostat[index].state = value;
  if (SERIAL_ON) Serial << "thtIs " << index << " " << onOffToText(value) << endl;
}

byte thermostatIndex(char *name)
{
  for (byte i = 0; i < N_THERMOSTATS; i++)
  {
      if (strcmp(name, thermostat[i].name) == 0) return i;
  }
    
  return 255;
}

