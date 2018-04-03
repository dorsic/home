

void handleTemperatureState0()
{
  char *arg;
  byte tempIdx;
  boolean err1 = false;
  
  arg = sCmd.next();
  if (arg == NULL) err1 = true;
  else {
    tempIdx = atoi(arg);
    if (tempIdx >= N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS) err1 = true;
    else {
      Serial << "tmpIs " << tempSensors[tempIdx].idx << " " << tempSensors[tempIdx].temperature << endl;
    }
  }
 if (err1) {
    Serial << F("error processing temp command - invalid first argument (temp sensor id): ") << arg << endl;
    Serial << F("available temp sensor ids: ");
    for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) { Serial << tempSensors[i].idx << " ," ; }
    Serial << endl;
  }    
}

void handleTemperatureDebug()
{
  char *arg;
  byte tempIdx;
  boolean err1 = false;
  
  arg = sCmd.next();
  if (arg == NULL) err1 = true;
  else {
    tempIdx = atoi(arg);
    if (tempIdx >= N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS) err1 = true;
    else {
      Serial << "tmpdIs " 
        << tempSensors[tempIdx].idx << " "  << tempSensors[tempIdx].bus << " " 
        << tempSensors[tempIdx].tCorrection << " " << tempSensors[tempIdx].temperature << " " 
        << tempSensors[tempIdx].isReading << " " << tempSensors[tempIdx].lastRead << " "
        << tempSensors[tempIdx].needToAnswer
        << endl;         
    }
  }
 if (err1) {
    Serial << F("error processing temp command - invalid first argument (temp sensor id): ") << arg << endl;
    Serial << F("available temp sensor ids: ");
    for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) { Serial << tempSensors[i].idx << " ," ; }
    Serial << endl;
  }    
}

void handleTemperatureAllState()
{
  byte i;
  boolean err1 = false;

  Serial << "tmpAre";
    for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) Serial << " " << tempSensors[i].temperature;
  Serial << endl;
}


void handleTemperatureStateForce()
{
  char *arg;
  byte tempIdx;
  boolean err1 = false;
  
  arg = sCmd.next();
  if (arg == NULL) err1 = true;
  else {
    tempIdx = atoi(arg);
    if (tempIdx >= N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS) err1 = true;
    else {
      updateTemperature(tempIdx);
    }
  }
 if (err1) {
    Serial << F("error processing temp command - invalid first argument (temp sensor id): ") << arg << endl;
    Serial << F("available temp sensor ids: ");
    for (int i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) { Serial << tempSensors[i].idx << " ," ; }
    Serial << endl;
  }    
}

void handleTemperatureCorrection()
{
  char *arg1;
  char *arg2;
  byte tmpIdx;
  float rtmp;
  boolean err1 = false;
  
  arg1 = sCmd.next();  
  if (arg1 == NULL) err1 = true;
  else {
    tmpIdx = atoi(arg1);
    if (tmpIdx >= N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS) err1 = true;
    else {
       arg2 = sCmd.next();
       rtmp = atof(arg2);
       setTemperatureCorrection(tmpIdx, rtmp);
       Serial << "crtIs " << arg1 << " " << getTemperatureCorrection(tmpIdx) << endl; 
    }
  }
 if (err1) {
    if (SERIAL_ON) {
       Serial << F("crt 'error processing crt command  - invalid first argument (themperature sensor id) or second (temperature as float)': ") << arg1 << " " << arg2 << endl;
       Serial << F("available names: ");
       for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) { Serial << tempSensors[i].idx << " ," ; }
       Serial << endl;
    }
  }
}

void handleTemperatureCorrectionState()
{
  char *arg;
  byte tmpIdx;
  boolean err1 = false;
  
  arg = sCmd.next();  
  if (arg == NULL) err1 = true;
  else {
    tmpIdx = atoi(arg);
    if (tmpIdx >= N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS) err1 = true;
    else {
       Serial << "crtIs " << arg << " " << getTemperatureCorrection(tmpIdx) << endl; 
    }
  }
 if (err1) {
    if (SERIAL_ON) {
       Serial << F("crt? 'error processing ctt? command - invalid first argument (themperature sensor id)': ") << arg << endl;
       Serial << F("available ids: ");
       for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) { Serial << tempSensors[i].idx << " ," ; }
       Serial << endl;
    }
  }
}

void handleTemperatureCorrectionAllState()
{
  byte i;
  boolean err1 = false;

  Serial << "crtAre";
    for (byte i = 0; i < N_TEMP_SENSORS + N_TEMP_SLAVE_SENSORS; i++) Serial << " " << getTemperatureCorrection(i);
  Serial << endl;
}

void initSerial0TmpCommands()
{
  if (SERIAL_ON) {
    sCmd.addCommand("tmp?", handleTemperatureState0);
    sCmd.addCommand("tmpf?", handleTemperatureStateForce);
    sCmd.addCommand("tmpa?", handleTemperatureAllState);
    sCmd.addCommand("tmpd?", handleTemperatureDebug);
    sCmd.addCommand("crt", handleTemperatureCorrection);
    sCmd.addCommand("crt?", handleTemperatureCorrectionState);
    sCmd.addCommand("crta?", handleTemperatureCorrectionAllState);
  }
}


