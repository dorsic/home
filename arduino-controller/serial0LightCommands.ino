
// --------------------   conversions   ---------------------------

const char *lightOnOffToText(byte state)
{
  return (state == LIGHT_ON)? "ON": "OFF";
}

byte textToLightOnOff(char *state)
{
  return (strcmp(state, "ON") == 0) ? LIGHT_ON : LIGHT_OFF;
}

// --------------------   serial messages   -----------------------------

void sendLightSwitchState(byte lightSwitchIndex, int state) {
  const char *st;
  switch (state) {
    case SWITCH_CMD_CLICK:
      st = "CLICK"; break;
    case SWITCH_CMD_DOUBLECLICK:
      st = "DOUBLECLICK"; break;
    case SWITCH_CMD_LONGCLICK:
      st = "LONGCLICK"; break;
    default:
      st = "UNRECOGNIZED";
  }
    
  Serial << "swcIs " << lightSwitchIndex << " " << st << endl; 
}

void sendLightState(byte lightIndex, byte state) {
  Serial << "lgtIs " << lightIndex << " " << lightOnOffToText(state) << endl;
}


// --------------------   serial commands   -----------------------------

void handleLight()
{
  char *arg;
  byte lightIdx;
  byte state;
  boolean err1 = false;
  
  arg = sCmd.next();
  if (arg == NULL) err1 = true;
  else {
    lightIdx = atoi(arg);
    if (lightIdx >= N_LIGHTS) err1 = true;
    else {
      arg = sCmd.next();
      if (arg != NULL) {
        state = textToLightOnOff(arg);
        switchLight(lightIdx, state);
      } else {
          Serial << F("error processing light command - no second argument (ON | OFF)") << endl;
      }
    }
   
  }
 if (err1) {
    Serial << F("error processing light command - invalid first argument (light id)") << endl;
    Serial << F("available light ids: ");
    for (int i = 0; i < N_LIGHTS; i++) { Serial << i << " ," ; }
    Serial << endl;
  }
}

void handleLightsOff() {
  allLightsOff();
}

void handleLightState()
{
  char *arg;
  byte lightIdx;
  boolean err1 = false;
  
  arg = sCmd.next();
  if (arg == NULL) err1 = true;
  else {
    lightIdx = atoi(arg);
    if (lightIdx >= N_LIGHTS) err1 = true;
    else {
      Serial << "lgtIs " << lightIdx << " " << lightOnOffToText(lights[lightIdx].state) << endl;
    }   
  }
  
 if (err1) {
    Serial << F("error processing light command - no first argument (light id)") << endl;
    Serial << F("available light ids: ");
    for (int i = 0; i < N_LIGHTS; i++) { Serial << i << " ," ; }
    Serial << endl;
  }  
}

void initSerial0LightCommands()
{
  if (SERIAL_ON) {
     sCmd.addCommand("lgt", handleLight);
     sCmd.addCommand("lgtx", handleLightsOff);
     sCmd.addCommand("lgt?", handleLightState);
     Serial << F("Serial lights commands initialized.") << endl;
  }
}

