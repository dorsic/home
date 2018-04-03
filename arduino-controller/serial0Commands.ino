

void setup_serial()
{
  if (SERIAL_ON) {
    Serial.begin(9600);
    while (!Serial) { 
      ; // wait for serial port to connect. Needed for Leonardo only 
    }
    #ifdef DEBUG
      Serial << PROJECT << " started DEBUG." << endl;
    #else 
      Serial << PROJECT << " started." << endl;    
    #endif
  }
}

// --------------------   conversions   ---------------------------

byte textToOnOff(char *state)
{
  return (strcmp(state, "ON") == 0) ? ON : OFF;
}

const char *onOffToText(byte state)
{
  return (state == ON)? "ON": "OFF";
}

const char *onOffUndefinedText(byte state)
{
  return (state == ON)? "ON": (state == OFF)? "OFF": "UNDEFINED";
}

// --------------------   serial commands   -----------------------------

void handleHeating0()
{
  char *arg = sCmd.next();
  if (arg != NULL) {
    Serial1 << "htn " << arg << endl;
   } else {
      Serial << F("heating 'error processing heatign command - no argument (ON | OFF)': ") << arg << endl;
   }   
}

void handleHeatingState0()
{
  Serial << "htnIs " << onOffToText(heatingState) <<  endl;
}

void handleHeatingStateForce()
{
  Serial1 << "htn?" << endl;
}

// --------------------   serial commands   -----------------------------



void handleDebug() {
  if (SERIAL1_ON) Serial1 << F("debug?") << endl;  
}

void unrecognized0() {
  Serial.println(F("What?"));
}

void initSerialCommands()
{
  if (SERIAL_ON) {

    
    sCmd.addCommand("htn", handleHeating0);
    sCmd.addCommand("htn?", handleHeatingState0);
    sCmd.addCommand("htnf?", handleHeatingStateForce);
    sCmd.addCommand("debug?", handleDebug);
    
    sCmd.addDefaultHandler(unrecognized0);
    Serial << F("Serial commands initialized.") << endl;
  }
}

void updateSerial()
{
  if (SERIAL_ON && Serial.available() > 0) {
    sCmd.readSerial();
  }  
}


