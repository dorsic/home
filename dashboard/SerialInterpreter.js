#!/usr/bin/env node


TSENSORS = {
    TVCHOD : 'vchod',
    TKUCHYNA : 'kuchyna'
}

var IsMessage = require("./IsMessage.js");
var NamesMap = require("./NamesMap.js");

var util = require("util");
var events = require("events");
var SerialPort = require('serialport');
var namesMap = new NamesMap();

function SerialInterpreter(port) {
    events.EventEmitter.call(this);
    this.portName = port;
    this.sp = null;
}

util.inherits(SerialInterpreter, events.EventEmitter);

//SerialInterpreter.prototype.handle = function( request, response ) { };


SerialInterpreter.prototype.open = function() {
    this.sp = new SerialPort(this.portName, {
	baudRate: 9600, // this is synced to what was set for the Arduino Code
	dataBits: 8, // this is the default for Arduino serial communication
	parity: 'none', // this is the default for Arduino serial communication
	stopBits: 1, // this is the default for Arduino serial communication
	flowControl: false, // this is the default for Arduino serial communication
	parser: SerialPort.parsers.readline("\n")
    });

//    console.log("serial port opening.");
    var _this = this;

    this.sp.on('data', function (data) { // call back when data is received
      	if (data != null) {
      	    try {
          		var date = new Date()
          		console.log(date.toUTCString() + ': Serial received: ' + data.toString());
          		var msgPortions = data.trim().split(' ');
          		var msg = null;
          		switch (msgPortions[0].toString()) {
          		    case "ready":
                          _this.emit('ready', null);
                          break;
          		    case "htnIs":
                          msg = new IsMessage(msgPortions[0], '', msgPortions[1], date);
                          _this.emit(msgPortions[0], msg);
                          break;
                  case "tmpIs":
                          msg = new IsMessage(msgPortions[0], namesMap.tempName(msgPortions[1]), msgPortions[2], date);
                          _this.emit(msgPortions[0], msg);
                          break;
                  case "thtIs":
                  case "rqtIs":
                          msg = new IsMessage(msgPortions[0], namesMap.thermostatName(msgPortions[1]), msgPortions[2], date);
                          _this.emit(msgPortions[0], msg);
                          break;
                  case "lgtIs":
                          msg = new IsMessage(msgPortions[0], namesMap.lightName(msgPortions[1]), msgPortions[2], date);
                          _this.emit(msgPortions[0], msg);
                          break;
                  case "swcIs":
                          msg = new IsMessage(msgPortions[0], namesMap.switchName(msgPortions[1]), msgPortions[2], date);
                          _this.emit(msgPortions[0], msg);
                          break;
                  case "tmpAre":
                          msg = new IsMessage(msgPortions[0], '', msgPortions.slice(1), date);
                          _this.emit(msgPortions[0], msg);
                          break;
          		    default:
          			         msg = new IsMessage(msgPortions[0], msgPortions[1], msgPortions[2], date);
          			        _this.emit(msgPortions[0], msg);
          		}
      	    } catch (err) {
      		      console.log('err from SerialInterpreter> ' + err.toString());
      	    }
      	}
    });

    this.sp.on('error', function (data) {
        try {
      	   if (data != null) console.log(data.toString());
         } catch (err) {
            console.log('err from SerialInterpreter error> ' + err.toString());
         }
    });

    this.emit('open', this.portName);

}


SerialInterpreter.prototype.sendCommand = function(data) {
    try {
      console.log("Sending data to serial port > " + data + '\n');
      this.sp.write(data+"\r", null);
    } catch (err) {
      console.log("Error sending serial command.");
    }
}

module.exports = SerialInterpreter;
