#!/usr/bin/env node

var DataItem = require('./DataItem.js');
var DataStream = require('./DataStream.js');



function PalisadyModel() {
    // vchod, kuchyna, hifi, predspalnou, spalnaz, kupelna, spalnicka, obyvacka7, balkon
    this.temperatures = {
       'vchod': new DataStream(),
			 'kuchyna': new DataStream(),
       'predspalnou': new DataStream(),
			 'spalnaz': new DataStream(),
			 'kupelna': new DataStream(),
			 'spalnicka': new DataStream(),
       'hifi': new DataStream(),
       'rkupelnain': new DataStream(),
       'rkupelnaout': new DataStream(),
       'rspalnain': new DataStream(),
       'rspalnaout': new DataStream(),
       'balkon': new DataStream(),
       'obyvacka7': new DataStream(),
       'kotol': new DataStream()
			 };
    // herna, hernaz, spalnickaokno, spalnicka, zachod, kuchyna, vchod, obyvacka, spalna, kupelna, zrkado, kumbal, policka
    this.lights = {
       'herna': new DataStream(),
		   'hernaz': new DataStream(),
		   'spalnickaokno': new DataStream(),
		   'spalnicka': new DataStream(),
		   'zachod': new DataStream(),
		   'kuchyna': new DataStream(),
		   'vchod': new DataStream(),
		   'obyvacka': new DataStream(),
		   'spalna': new DataStream(),
		   'kupelna': new DataStream(),
		   'zrkadlo': new DataStream(),
		   'kumbal': new DataStream(),
		   'policka': new DataStream()
		   };

    // vstupKrajNoLed, vstupStred, vstupLed, kuchyna, predSpalnou, spalna, hernaLed, hernaNoLed, kumbal, kupelnaStred, kupelna, spalnickaLed, spalnickaNoLed, spalnaPostel, zachod, zrkadlo
    this.switches = {
         'vstupKrajNoLed': new DataStream(),
		     'vstupStred': new DataStream(),
		     'vstupLed': new DataStream(),
		     'kuchyna': new DataStream(),
		     'predSpalnou': new DataStream(),
		     'spalna': new DataStream(),
		     'hernaLed': new DataStream(),
		     'hernaNoLed': new DataStream(),
		     'kupelnaNoLed': new DataStream(),
		     'kupelnaStred': new DataStream(),
		     'kupelnaLed': new DataStream(),
		     'spalnickaLed': new DataStream(),
		     'spalnickaNoLed': new DataStream(),
		     'spalnaPostel': new DataStream(),
		     'zachod': new DataStream(),
		     'zrkadlo': new DataStream()
		     };

    this.requiredTemperatures = {
      'vchod': new DataStream(),
      'kuchyna': new DataStream(),
      'predspalnou': new DataStream(),
      'spalnaz': new DataStream(),
      'kupelna': new DataStream(),
      'spalnicka': new DataStream(),
      'hifi': new DataStream(),
      'obyvacka7': new DataStream()
    };

    this.thermostats = {
      'vchod': new DataStream(),
      'kuchyna': new DataStream(),
      'predspalnou': new DataStream(),
      'spalnaz': new DataStream(),
      'kupelna': new DataStream(),
      'spalnicka': new DataStream(),
      'hifi': new DataStream(),
      'obyvacka7': new DataStream(),
      'int': new DataStream(),
      'ext': new DataStream()
      };

    this.heating = new DataStream();

    this.devices = { };
};

/*
function stringifyCurrentValues(object) {
    if (object instanceof DataStream) {
	return JSON.stringify(object.CurrentValue());
    }
    var str = '{';
    for (prop in object)
    {
	str += '"' + prop.toString() + '":' + stringifyCurrentValues(object[prop]);
	str += ',';
    }
    if (str.length == 1) return JSON.stringify(object);
    return str.slice(0, -1) + '}';
}
*/

function stringifyWithDataStream(object, dataStreamValueFunction) {
    if (object instanceof DataStream) {
	if (typeof object[dataStreamValueFunction] === 'function') {
	    return JSON.stringify(object[dataStreamValueFunction]());
	}
    }
    var str = '{';
    for (prop in object)
    {
	str += '"' + prop.toString() + '":' + stringifyWithDataStream(object[prop], dataStreamValueFunction);
	str += ',';
    }
    if (str.length == 1) return JSON.stringify(object);
    return str.slice(0, -1) + '}';
}

PalisadyModel.prototype.CurrentStatus = function() {
//    return stringifyCurrentValues ({'temperatures': this.temperatures, 'lights': this.lights, 'switches':this.switches, 'requiredTemperatures': this.requiredTemperatures, 'thermostats': this.thermostats, 'heating': this.heating });
    return stringifyWithDataStream({'temperatures': this.temperatures, 'lights': this.lights, 'switches':this.switches, 'requiredTemperatures': this.requiredTemperatures, 'thermostats': this.thermostats, 'heating': this.heating, 'devices': this.devices }, "CurrentValue");
}

PalisadyModel.prototype.LastDayStatus = function() {
    return stringifyWithDataStream({'temperatures': this.temperatures, 'lights': this.lights, 'switches':this.switches, 'requiredTemperatures': this.requiredTemperatures, 'thermostats': this.thermostats, 'heating': this.heating, 'devices': this.devices }, "LastDayValues");
}

module.exports = PalisadyModel;
