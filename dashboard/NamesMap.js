
var TempNamesMap = {
    0: "vchod",
    1: "kuchyna",
    2: "predspalnou",
    3: "spalnaz",
    4: "kupelna",
    5: "spalnicka",
    6: "hifi",
    7: "rkupelnain",
    8: "rkupelnaout",
    9: "rspalnain",
    10: "rspalnaout",
    11: "balkon",
    12: "obyvacka7",
    13: "kotol",
    14: "int",
    15: "ext"
};

var TempNamesMapRev = {
    "vchod": 0,
    "kuchyna": 1,
    "predspalnou": 2,
    "spalnaz": 3,
    "kupelna": 4 ,
    "spalnicka": 5,
    "hifi": 6,
    "rkupelnain": 7,
    "rkupelnaout": 8,
    "rspalnain": 9,
    "rspalnaout": 10,
    "balkon": 11,
    "obyvacka7": 12,
    "kotol": 13,
    "int": 14,
    "ext": 15
};

var ThermostatNamesMap = {
    0: "vchod",
    1: "kuchyna",
    2: "predspalnou",
    3: "spalnaz",
    4: "kupelna",
    5: "spalnicka",
    6: "hifi",
    7: "obyvacka7",
    8: "int",
    9: "ext"
};

var ThermostatNamesMapRev = {
    "vchod": 0,
    "kuchyna": 1,
    "predspalnou": 2,
    "spalnaz": 3,
    "kupelna": 4 ,
    "spalnicka": 5,
    "hifi": 6,
    "obyvacka7": 7,
    "int": 8,
    "ext": 9
};

var LightNamesMap = {
  0: "herna",
  1: "hernaz",
  2: "spalnickaokno",
  3: "spalnicka",
  4: "zachod",
  5: "kuchyna",
  6: "vchod",
  7: "obyvacka",
  8: "spalna",
  9: "kupelna",
  10: "zrkadlo",
  11: "kumbal",
  12: "policka"
};

var LightNamesMapRev = {
  "herna": 0,
  "hernaz": 1,
  "spalnickaokno": 2,
  "spalnicka": 3,
  "zachod": 4,
  "kuchyna": 5,
  "vchod": 6,
  "obyvacka": 7,
  "spalna": 8,
  "kupelna": 9,
  "zrkadlo": 10,
  "kumbal" : 11,
  "policka": 12
};

var SwitchNamesMap = {
  0: "vstupKrajNoLed",
  1: "vstupStred",
  2: "vstupLed",
  3: "kuchyna",
  4: "predSpalnou",
  5: "spalna",
  6: "hernaLed",
  7: "hernaNoLed",
  8: "kupelnaNoLed",
  9: "kupelnaStred",
  10: "kupelnaLed",
  11: "spalnickaLed",
  12: "spalnickaNoLed",
  13: "spalnaPostel",
  14: "zachod",
  15: "zrkadlo"
};

var SwitchNamesMapRev = {
  "vstupKrajNoLed": 0,
  "vstupStred": 1,
  "vstupLed": 2,
  "kuchyna": 3,
  "predSpalnou": 4,
  "spalna": 5,
  "hernaLed": 6,
  "hernaNoLed": 7,
  "kupelnaNoLed": 8,
  "kupelnaStred": 9,
  "kupelnaLed": 10,
  "spalnickaLed": 11,
  "spalnickaNoLed": 12,
  "spalnaPostel": 13,
  "zachod": 14,
  "zrkadlo": 15
};

function NamesMap() { };

NamesMap.prototype.tempName = function (index) {
    if (typeof(index) == "string") return TempNamesMap[parseInt(index)];
    return TempNamesMap[index];
};

NamesMap.prototype.tempIndex = function (name) {
    return TempNamesMapRev[name];
};

NamesMap.prototype.thermostatName = function (index) {
    if (typeof(index) == "string") return ThermostatNamesMap[parseInt(index)];
    return ThermostatNamesMap[index];
};

NamesMap.prototype.thermostatIndex = function (name) {
    return ThermostatNamesMapRev[name];
};

NamesMap.prototype.lightName = function (index) {
    if (typeof(index) == "string") return LightNamesMap[parseInt(index)];
    return LightNamesMap[index];
};

NamesMap.prototype.lightIndex = function (name) {
    return LightNamesMapRev[name];
};

NamesMap.prototype.switchName = function (index) {
    if (typeof(index) == "string") return SwitchNamesMap[parseInt(index)];
    return SwitchNamesMap[index];
};

NamesMap.prototype.switchIndex = function (name) {
    return switchNamesMapRev[name];
};

module.exports = NamesMap;
