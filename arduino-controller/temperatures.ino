    


OneWire oneWire[5] = { OneWire(OW_BUS_0_PIN), OneWire(OW_BUS_1_PIN), OneWire(OW_BUS_2_PIN), OneWire(OW_BUS_3_PIN), OneWire(OW_BUS_4_PIN) };
DallasTemperature tSensors[5] = { DallasTemperature(&oneWire[0]), DallasTemperature(&oneWire[1]), DallasTemperature(&oneWire[2]), DallasTemperature(&oneWire[3]), DallasTemperature(&oneWire[4])};

unsigned long lastReadings = -30000;
boolean isTempReading = false;

const char *tempSensorName(byte index)
{
    if (index == TBALKON) return "balkon";
    if (index == TOBYVACKA7) return "obyvacka7";
    if (index == TKOTOL) return "kotol";

    return "";
}

byte tempSensorIndex(char *name)
{
  if (strcmp(name, "balkon") == 0) return TBALKON;
  if (strcmp(name, "obyvacka7") == 0) return TOBYVACKA7;
  if (strcmp(name, "kotol") == 0) return TKOTOL;
  
  return 255;
}

void setup_temperatures() {
  for (int i = 0; i < 5; i++) {
    tSensors[i].begin();
    tSensors[i].setWaitForConversion(false); 
  }

  for (int i = 0; i < N_TEMP_SENSORS; i++) {
     tSensors[tempSensors[i].bus].setResolution(tempSensors[i].address, 11);
     tempSensors[i].needToAnswer = false;
  }
  
  for (int i = 0; i < N_TEMP_SLAVE_SENSORS; i++) tempSensors[i+N_TEMP_SENSORS].needToAnswer = false;
  
}


float getTemperature(byte index)
{
   return tempSensors[index].temperature; 
}

float getTemperatureCorrection(byte index)
{
    return ((float)tempSensors[index].tCorrection) / 10000.0;
}

void setTemperatureCorrection(byte index, float value)
{
    if (value > 3.2) value = 3.2;
    if (value < -3.2) value = -3.2;
    tempSensors[index].tCorrection = (short)(value * 10000.0);
}

void sendTemperature(byte index)
{
  if (SERIAL_ON)
      Serial << "tmpIs " << index << " " << tempSensors[index].temperature << endl;
}

void setTemperature(byte index, float value)
{
//    Serial << F("setting temperature ") << index << " to " << value << endl;
    tempSensors[index].temperature = value + ((float)tempSensors[index].tCorrection) / 10000.0;
    tempSensors[index].isReading = false;
    tempSensors[index].lastRead = millis();
    if (tempSensors[index].needToAnswer) { sendTemperature(index); tempSensors[index].needToAnswer = false; }
}

void updateTemperature(byte index)
{
    byte bus;
//    Serial << "updating temperature " << index << " that is (reading) " << tempSensors[index].isReading << endl; 
    if (tempSensors[index].isReading == true) return;
    
    if (index < N_TEMP_SENSORS) {
      bus = tempSensors[index].bus;
      tSensors[tempSensors[index].bus].requestTemperatures();
    } else {
      if (SERIAL1_ON) { 
        Serial1 << "tmp? " << tempSensorName(index+N_TEMP_SENSORS) << endl; 
      }
    }
//    tempSensors[index].isReading = true;
    tempSensors[index].lastRead = millis();  
    tempSensors[index].needToAnswer = true;
}

void updateTemperatures()
{
   if (!isTempReading && (millis() - lastReadings >= TEMP_READING_INTEVAL))
   {
      isTempReading = true;
      for (int i = 0; i < N_TEMP_SENSORS; i++) { tempSensors[i].isReading = true; tempSensors[i].lastRead = millis(); }
      for (int i = 0; i < 5; i++) { tSensors[i].requestTemperatures(); }
      
      if (SERIAL1_ON) { 
        for (int i = N_TEMP_SENSORS; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) {
          if (!tempSensors[i].isReading) { 
            Serial1 << "tmp? " << tempSensorName(i) << endl; 
            tempSensors[i].isReading = true; 
            tempSensors[i].lastRead = millis(); 
          }
        }
      } 
      lastReadings = millis();
   }

   for (int i = 0; i < N_TEMP_SENSORS; i++)
   {
      if (tempSensors[i].isReading && (millis() - tempSensors[i].lastRead >= 700)) {
         setTemperature(i, tSensors[tempSensors[i].bus].getTempC(tempSensors[i].address)); 
      }
   }
   
   if (isTempReading && (millis() - lastReadings >= 700))
   {
      isTempReading = false;
      lastReadings = millis(); 
   }
   
}




