

void setup_serial1()
{
  if (SERIAL1_ON) {  
    Serial1.begin(9600);
    while (!Serial1) { 
      ; // wait for serial port to connect. Needed for Leonardo only 
    }
    if (SERIAL_ON) Serial << "Serial1 opened." << endl;  
  }
}

// --------------------   serial commands   -----------------------------

void handleTemperatureAnswer1()
{
  char *arg;
  byte tempIdx;
  boolean err1 = false;
  
  arg = sCmd1.next();  
  if (arg == NULL) err1 = true;
  else {
    tempIdx = tempSensorIndex(arg);
    if (N_TEMP_SENSORS <= tempIdx && tempIdx < N_TEMP_SENSORS+N_TEMP_SLAVE_SENSORS) {
       arg = sCmd1.next();
       setTemperature(tempIdx, atof(arg));
    } else err1 = true;
  }
  if (err1) {
    if (SERIAL_ON) {
       Serial << F("tmp? (1) 'error processing tmp? command  - invalid first argument (temp sensor id)': ") << arg << endl;
       Serial << F("available temp sensor ids:");
       for (byte i = N_TEMP_SENSORS; i < N_TEMP_SENSORS+N_TEMP_SLAVE_SENSORS; i++) { Serial << " " << tempSensorName(i); }
       Serial << endl;
    }
  }   
}

void handleHeatingAnswer1()
{
  char *arg;
  byte newState;
  
  arg = sCmd1.next();
  if (arg != NULL) {
    newState = textToOnOff(arg);
    heatingState = newState;
    if (SERIAL_ON) {
        Serial << "htnIs " << arg << endl;
    }
  } else {
    if (SERIAL_ON) {
      Serial << F("htnIs (1) 'error processing heating command - no argument (ON | OFF)': ") << arg << endl;
    }
  }
}

void handleThermostatAnswer1()
{
  char *arg1;
  char *arg2;
  byte thtIdx;
  boolean err1 = false;
  
  arg1 = sCmd1.next();  
  if (arg1 == NULL) err1 = true;
  else {
    thtIdx = thermostatIndex(arg1);
    if (thtIdx == 255) err1 = true;
    else {
       arg2 = sCmd1.next();
       thermostat[thtIdx].state = textToOnOff(arg2);
       if (SERIAL_ON) Serial << "thtIs " << thtIdx << " " << arg2 << endl; 
    }
  }
 if (err1) {
    if (SERIAL_ON) {
       Serial << F("thtIs (1) 'error processing thtIs command  - invalid first argument (thermostat id) or state ( ON | OFF)': ") << arg1 << " " << arg2 << endl;
       Serial << F("available thermostat ids:");
       for (byte i = N_THERMOSTATS-N_THT_KOTOL; i < N_THERMOSTATS; i++) { Serial << " " << thermostat[i].name; }
       Serial << endl;
    }
  }     
}

void handleDebugAnswer1()
{
  char *arg;
  
  arg = sCmd1.next();
  Serial << "debugIs> ";
  while (arg != NULL)
  {
    Serial << arg << " ";
    arg = sCmd1.next();    
  }
  Serial << endl;
}

void unrecognized1() {
  Serial1.println("What?");
}

void initSerial1Commands()
{
   if (SERIAL1_ON) {
      sCmd1.addCommand("tmpIs", handleTemperatureAnswer1);
      sCmd1.addCommand("htnIs", handleHeatingAnswer1);
      sCmd1.addCommand("thtIs", handleThermostatAnswer1);  
      sCmd1.addCommand("debugIs", handleDebugAnswer1);  
      
      sCmd1.addDefaultHandler(unrecognized1);
      if (SERIAL_ON) { Serial << F("Serial1 commands initialized.") << endl; } 
   }
}


void updateSerial1()
{
  if (SERIAL1_ON && Serial1.available() > 0) {
//    readSerial1();
    sCmd1.readSerial();
  }  
}

///////////////////////////////////////

