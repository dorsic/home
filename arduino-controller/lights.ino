

/*
switches
   40, 42, 44 - kupelna
   34 - pred spalnou
   32 - herna bez svetla   
   30 - herna so svetlom
   22 - vchod, krajny NO LED
   24 - vchod, stredny
   26 - vchod, krajny LED
   28 - kuch. linka
   30 - pred spalnou
   32 - spalna
   34 - herna LED
   36 - herna NO LED
   38 - kupelna krajny NO LED
   40 - kupelna stredny
   42 - kupelna krajny LED
   44 - spalnicka LED
   46 - spalnicka NO LED
   48 - spalna postel
   50 - zachod
   52 - kup. zrkadlo
   
lights
   23 - herna
   25 - herna zasuvka
   27 - spalnicka pri okne
   29 - spalnicka pri satniku
   31 - zachod
   33 - kuchyna policka
   35 - vchod
   37 - obyvacka
   39 - spalna
   41 - kupelna
   43 - kupelna zrkadlo
   45 - kumbal
   47 - policka obyvacka
   49 - rezerva
*/

#define SWITCH_CMD_NONE 0
#define SWITCH_CMD_CLICK 1
#define SWITCH_CMD_DOUBLECLICK 2
#define SWITCH_CMD_TRIPPLECLICK 3
#define SWITCH_CMD_LONGCLICK -1

#define LIGHT_OFF 1
#define LIGHT_ON 0

// defines index in the lights array
#define LHERNA 0
#define LHERNAZ 1
#define LSPALNICKAOKNO 2
#define LSPALNICKA 3
#define LZACHOD 4
#define LKUCHYNA 5
#define LVCHOD 6
#define LOBYVACKA 7
#define LSPALNA 8
#define LKUPELNA 9
#define LZRKADLO 10
#define LKUMBAL 11
#define LPOLICKA 12
#define LALL 8191    // 2^N_LIGHTS - 1

typedef struct {
  byte pin;
  byte state;
  byte newState;
} Light;


Light lights[] = { 
    // must be in same order as the L.. constant values above
    {23, LIGHT_ON, LIGHT_OFF},  // LHERNA
    {25, LIGHT_ON, LIGHT_OFF},  // LHERNAZ
    {27, LIGHT_ON, LIGHT_OFF},  // LSPALNICKAOKNO
    {29, LIGHT_ON, LIGHT_OFF}, // LSPALNICKA
    {31, LIGHT_ON, LIGHT_OFF}, // LZACHOD
    {33, LIGHT_ON, LIGHT_OFF},  // LKUCHYNA
    {35, LIGHT_ON, LIGHT_OFF},  // LVCHOD
    {37, LIGHT_ON, LIGHT_OFF}, // LOBYVACKA
    {39, LIGHT_ON, LIGHT_OFF}, // LSPALNA
    {41, LIGHT_ON, LIGHT_OFF},  // LKUPELNA
    {43, LIGHT_ON, LIGHT_OFF},  // LZRKADLO
    {45, LIGHT_ON, LIGHT_OFF}, // LKUMBAL
    {47, LIGHT_ON, LIGHT_OFF} // LPOLICKA
};

#define SVSTUPKRAJNOLED 0
#define SVSTUPSTRED 1
#define SVSTUPLED 2
#define SKUCHYNA 3
#define SPREDSPALNOU 4
#define SSPALNA 5
#define SHERNALED 6
#define SHERNANOLED 7
#define SKUPELNANOLED 8
#define SKUPELNASTRED 9
#define SKUPELNALED 10
#define SSPALNICKALED 11
#define SSPALNICKANOLED 12
#define SSPALNAPOSTEL 13
#define SZACHOD 14
#define SZRKADLO 15

typedef struct {
  byte pin;
  unsigned long lightsOnClick;
  unsigned long lightsOnDoubleClick;
  unsigned long lightsOnLongClick;
  ClickButton button;
  int command;
} LightSwitch;


LightSwitch lightSwitches[] = {
  { 22, 1<<LPOLICKA, 1<<LKUCHYNA, LALL, ClickButton(22, LOW, CLICKBTN_PULLUP) },  // SVSTUPNOLED
  { 24, 1<<LVCHOD, 1<<LKUCHYNA, LALL, ClickButton(24, LOW, CLICKBTN_PULLUP) },  // SVSTUPSTRED
  { 26, 1<<LOBYVACKA, 0, LALL, ClickButton(26, LOW, CLICKBTN_PULLUP) },  // SVSTUPLED
  { 28, 1<<LKUCHYNA, 0, LALL, ClickButton(28, LOW, CLICKBTN_PULLUP) }, // SKUCHYNA
  { 30, 1<<LOBYVACKA, 1<<LSPALNA, LALL, ClickButton(30, LOW, CLICKBTN_PULLUP) }, // SPREDSPALNOU
  { 32, 1<<LSPALNA, 0, LALL, ClickButton(32, LOW, CLICKBTN_PULLUP) }, // SSPALNA
  { 34, 1<<LHERNA, 0, LALL, ClickButton(34, LOW, CLICKBTN_PULLUP) },  // SHERNALED
  { 36, 1<<LPOLICKA, 1<<LHERNAZ, LALL, ClickButton(36, LOW, CLICKBTN_PULLUP) }, // SHERNANOLED
  { 38, 1<<LKUMBAL, 0, LALL, ClickButton(38, LOW, CLICKBTN_PULLUP) }, // SKUPELNANOLED
  { 40, 1<<LZACHOD, 0, LALL, ClickButton(40, LOW, CLICKBTN_PULLUP) }, // SKUPELNASTRED
  { 42, 1<<LKUPELNA, 1<<LZRKADLO, LALL, ClickButton(42, LOW, CLICKBTN_PULLUP) },  // SKUPELNALED
  { 44, 1<<LSPALNICKA, 1<<LSPALNA, LALL, ClickButton(44, LOW, CLICKBTN_PULLUP) }, //SSPALNICKALED
  { 46, 1<<LSPALNICKAOKNO, 1<<LSPALNA, LALL, ClickButton(46, LOW, CLICKBTN_PULLUP) }, // SSPALNICKANOLED
  { 48, 1<<LSPALNA, 1<<LSPALNICKA + 1<<LSPALNICKAOKNO, LALL, ClickButton(48, LOW, CLICKBTN_PULLUP) }, // SSPALNAPOSTEL
  { 50, 1<<LZACHOD, 1<<LZRKADLO, LALL, ClickButton(50, LOW, CLICKBTN_PULLUP) },   // SZACHOD
  { 52, 1<<LZRKADLO, 1<<LKUPELNA, LALL , ClickButton(52, LOW, CLICKBTN_PULLUP) }   // SZRKADLO
};


void setup_lights()
{
  for (byte i=0; i<N_LIGHTS; i++) {
    pinMode(lights[i].pin,OUTPUT);  
  }
  
  for (byte i=0; i<N_LIGHT_SWITCHES; i++) {
    lightSwitches[i].button.debounceTime   = 20;   // Debounce timer in ms
    lightSwitches[i].button.multiclickTime = 250;  // Time limit for multi clicks
    lightSwitches[i].button.longClickTime  = 1000; // Time until long clicks register
  }
}



// --------------------   lights   -------------------------------------------

void switchLight(byte lightIdx, byte state) {
  
  if (lights[lightIdx].state != state) { 
    lights[lightIdx].newState = state;
  }
}

void toggleLight(byte lightIdx)
{
  lights[lightIdx].newState = (lights[lightIdx].state + 1) % 2;
#ifdef DEBUG
  if (SERIAL_ON) Serial << "Toggled light " << lightIdx << endl;
#endif
}

void allLightsOff() {
  for (byte i = 0; i < N_LIGHTS; i++) { 
    switchLight(i, LIGHT_OFF);
  }  
}

void updateLights(){
  for (byte i = 0; i < N_LIGHTS; i++) {
    if (lights[i].newState != lights[i].state) {
      digitalWrite(lights[i].pin, lights[i].newState);
      lights[i].state = lights[i].newState;
      if (SERIAL_ON) sendLightState(i, lights[i].state);
    }
  }  
}



// ---------------------   light switches   ----------------------------------

void lightSwitchClicked(byte lightSwitchIdx) {
  for (byte i = 0; i < N_LIGHTS; i++) {
    if ((lightSwitches[lightSwitchIdx].lightsOnClick & (1 << i)) == (1 << i)) {
        toggleLight(i);
    }
  }
}

void lightSwitchDoubleClicked(byte lightSwitchIdx) {
  for (byte i = 0; i < N_LIGHTS; i++) {
    if ((lightSwitches[lightSwitchIdx].lightsOnDoubleClick & (1 << i)) == (1 << i)) {
        toggleLight(i);
    }
  }
}

void lightSwitchLongClicked(byte lightSwitchIdx) {
  for (byte i = 0; i < N_LIGHTS; i++) {
    if ((lightSwitches[lightSwitchIdx].lightsOnLongClick & (1 << i)) == (1 << i)) {
        switchLight(i, LIGHT_OFF);
    }
  }
}

void updateLightSwitches() {
  // Go through each button and set the corresponding LED function
  for (byte i=0; i<N_LIGHT_SWITCHES; i++)
  {
    // Update state of all buttons
    lightSwitches[i].button.Update();
  
    // Save click codes in switchCommands, as clicks counts are reset at next Update()
    if (lightSwitches[i].button.clicks != SWITCH_CMD_NONE) { 
      lightSwitches[i].command = lightSwitches[i].button.clicks; 
    }
    
    // Simply toggle LED on single clicks
    // (Cant use switchCommands like the others here,
    //  as it would toggle on and off all the time)
    if (lightSwitches[i].button.clicks == SWITCH_CMD_CLICK) {
      if (SERIAL_ON) sendLightSwitchState(i, SWITCH_CMD_CLICK);
      lightSwitchClicked(i);
      lightSwitches[i].command = SWITCH_CMD_NONE;
    }

    if (lightSwitches[i].command == SWITCH_CMD_DOUBLECLICK) {
      if (SERIAL_ON) sendLightSwitchState(i, SWITCH_CMD_DOUBLECLICK);
      lightSwitchDoubleClicked(i);
      lightSwitches[i].command = SWITCH_CMD_NONE;
    }

    if (lightSwitches[i].command == SWITCH_CMD_LONGCLICK) {
      if (SERIAL_ON) sendLightSwitchState(i, SWITCH_CMD_LONGCLICK);
      lightSwitchLongClicked(i);
      lightSwitches[i].command = SWITCH_CMD_NONE;
    }  
  }  
}


