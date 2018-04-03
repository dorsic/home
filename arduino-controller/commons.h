

#define SERIAL_ON true


#ifdef DEBUG
  #define SERIAL1_ON false // used for communication with KOTOL
#else
  #define SERIAL1_ON true // used for communication with KOTOL
#endif

#define OFF 0
#define ON 1

#define RQT_UNDEFINED -32655
#define BYTE_UNDEFINED 255

#define DOOR_PIN A1
#define POWER_METER_PIN 7

// --------------------   lights   ---------------------------

#define N_LIGHTS 13
#define N_LIGHT_SWITCHES 16

// --------------------   temperatures, HEATING & THERMOSTATS   ---------------------------

#define OW_BUS_0_PIN 12
#define OW_BUS_1_PIN 11
#define OW_BUS_2_PIN 10
#define OW_BUS_3_PIN 9
#define OW_BUS_4_PIN 8

#define REQUIRED_TEMPERATURE 20700  // 20.7 degC

typedef struct {
  byte idx;
  uint8_t address[8];
  byte bus;
  short tCorrection; // short data type used to save memory. Use tcorrection/10000 to get the correct float value (thus max. correction is +-3.2 degC ).
//  short reqT; // required temperature * 1000 - verified by the thermostat
  float temperature;  // last measured value
  bool isReading; // if the temperature is currently reading - is in the state of updating the Dallas registries
  unsigned long lastRead;  // time when the temperature was last read
//  byte thermostatState;
  bool needToAnswer; // if force-reading the temperature this flags, which sensor do I need to reply via serail port
} DallasTempSensor;

#ifdef DEBUG
  #define N_TEMP_SENSORS 1
  #define N_TEMP_SLAVE_SENSORS 1
  #define TEMP_READING_INTEVAL 3000
  #define TVCHOD 0
  #define TPREDSPALNOU 1
  #define TOBYVACKA7 2
  #define TBALKON 3
  #define TKOTOL 4
  DallasTempSensor tempSensors[] = {
    // index, address, bus, correction, requiredTemp, temperature, isReading, lastRead, needToAnswer
    { TVCHOD, {0x28, 0x94, 0xA9, 0x01, 0x05, 0x00, 0x00, 0x09}, 4, -29569, 21100, 0.0, 0, 0 }
  };
#else
  #define N_TEMP_SENSORS 11
  #define N_TEMP_SLAVE_SENSORS 3
  #define TEMP_READING_INTEVAL 15000
  #define TVCHOD 0
  #define TKUCHYNA 1
  #define TPREDSPALNOU 2
  #define TSPALNAZ 3
  #define TKUPELNA 4
  #define TSPALNICKA 5
  #define THIFI 6  
  #define TRKUPELNAIN 7
  #define TRKUPELNAOUT 8
  #define TRSPALNAIN 9
  #define TRSPALNAOUT 10
  // nasledovne musia mat indexy >= N_TEMP_SENSORS, lebo requesty sa preposielaju na Serial1
  #define TBALKON 11
  #define TOBYVACKA7 12
  #define TKOTOL 13
  DallasTempSensor tempSensors[] = {
    // index, address, bus, correction, requiredTemp, temperature, isReading, lastRead, thermostatState, needToAnswer
    { TVCHOD, {0x28, 0x02, 0xA5, 0x01, 0x05, 0x00, 0x00, 0x2B}, 0, 0, 0.0, 0, 0 },
    { TKUCHYNA, {0x28, 0x56, 0x6C, 0x00, 0x05, 0x00, 0x00, 0x87}, 0, 0, 0.0, 0, 0 },
    { TPREDSPALNOU, {0x28, 0x75, 0x2E, 0x02, 0x05, 0x00, 0x00, 0x05}, 2, 0, 0.0, 0, 0 },
    { TSPALNAZ, {0x28, 0x9D, 0x09, 0x02, 0x05, 0x00, 0x00, 0x24}, 2, 0, 0.0, 0, 0 },
    { TKUPELNA, {0x28, 0x3E, 0x6C, 0x00, 0x05, 0x00, 0x00, 0xE5}, 3, 0, 0.0, 0, 0 },
    { TSPALNICKA, {0x28, 0xC3, 0x33, 0x01, 0x05, 0x00, 0x00, 0xA8}, 4, 0, 0.0, 0, 0 },
    { THIFI, {0x28, 0x00, 0x00, 0x05, 0x0B, 0x0B, 0xCD, 0xB1}, 0, 0, 0.0, 0 ,0 },
    { TRKUPELNAIN, {0x28, 0xD4, 0x9B, 0xB6, 0x05, 0x00, 0x00, 0x6C}, 0, 1761, 0.0, 0, 0 },
    { TRKUPELNAOUT, {0x28, 0x0E, 0x58, 0x01, 0x05, 0x00, 0x00, 0x1C}, 0, 1675, 0.0, 0, 0 },
    { TRSPALNAIN, {0x28, 0x66, 0x78, 0xB6, 0x05, 0x00, 0x00, 0x91}, 2, 189, 0.0, 0, 0 },
    { TRSPALNAOUT, {0x28, 0x00, 0x98, 0xB6, 0x05, 0x00, 0x00, 0x3A}, 2, 299, 0.0, 0, 0 },
    { TBALKON, {0x28, 0x02, 0xA5, 0x01, 0x05, 0x00, 0x00, 0x2B}, BYTE_UNDEFINED, 0, 0.0, 0, 0 },
    { TOBYVACKA7, {0x28, 0x02, 0xA5, 0x01, 0x05, 0x00, 0x00, 0x2B}, BYTE_UNDEFINED, 0, 0.0, 0, 0 },
    { TKOTOL, {0x28, 0x02, 0xA5, 0x01, 0x05, 0x00, 0x00, 0x2B}, BYTE_UNDEFINED, 0, 0.0, 0, 0 }
  };
#endif

#define THTEXT 8
#define THTINT 9 
#define N_THT_KOTOL 2
#define N_THERMOSTATS 10
typedef struct {
  const char *name;
  short reqT; // required temperature * 1000 - verified by the thermostat
  byte tempSensor;
  byte state;
} Thermostat;

Thermostat thermostat[] = {
  { "", RQT_UNDEFINED, TVCHOD, BYTE_UNDEFINED },
  { "", RQT_UNDEFINED, TKUCHYNA, BYTE_UNDEFINED },
  { "", REQUIRED_TEMPERATURE, TPREDSPALNOU, BYTE_UNDEFINED },
  { "", RQT_UNDEFINED, TSPALNAZ, BYTE_UNDEFINED },
  { "", RQT_UNDEFINED, TKUPELNA, BYTE_UNDEFINED },
  { "", RQT_UNDEFINED, TSPALNICKA, BYTE_UNDEFINED },
  { "", RQT_UNDEFINED, THIFI, BYTE_UNDEFINED },
  { "", REQUIRED_TEMPERATURE, TOBYVACKA7, BYTE_UNDEFINED },
  { "byt8", RQT_UNDEFINED, BYTE_UNDEFINED, BYTE_UNDEFINED },
  { "byt7", RQT_UNDEFINED, BYTE_UNDEFINED, BYTE_UNDEFINED }
};

