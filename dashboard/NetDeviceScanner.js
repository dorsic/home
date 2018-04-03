var exec = require('child_process').exec;
var DataStream = require('./DataStream.js');
var IsMessage = require('./IsMessage.js');

var SCAN_DELAY = 4000 // scan the network every 4s
var SCANS_IN_CYCLE = 15  // scan the network for 15 times
var CYCLE_TIME = 300000  // search for devices avery 5 minutes; cycle = SCANS_IN_CYCLE * scans
//var CYCLE_TIME = 60000  // testing purposes

function NetDeviceScanner(palisadyModel, elasticsearchLogger) {
  this.es = elasticsearchLogger;
  this.model = palisadyModel;
  // array of {"mac": <mac address>, "time": <last state change date>}
  this.foundDevices = [ ];
  this.activeDevices = [ ];
  this.counter = -1;
  this.scanTimer = null;
};

NetDeviceScanner.prototype.doScan = function(this_obj) {
  try {
//    snmpwalk -v 2c -O vq -c public 192.168.1.1 iso.3.6.1.2.1.3.1.1.2.19.1
// snmpwalk -v 2c -O vq -c public 192.168.1.1 iso.3.6.1.2.1.4.22.1.2.19
//    exec('snmpbulkget -v 2c -O vq -c public 192.168.1.1 iso.3.6.1.2.1.49 | tail -n +7 | grep "\\""'
    exec('snmpwalk -v 2c -O vq -c public 192.168.1.1 iso.3.6.1.2.1.4.22.1.2'
      , function(error, stdout, stderr) {
          var s = stdout.split("\n");
          for (var i in s) {
            if (s[i] != null && s[i].trim() !== '') {
              var mac = s[i].replace(/ /g, '').replace(/:/g, '').replace(/"/g, '').toLowerCase();
//              mac = mac.substring(1, mac.length-1).toLowerCase();
              if (this_obj.activeDevices.indexOf(mac) == -1) { this_obj.activeDevices.push(mac); }
            }
          }
        });
     this_obj.counter ++;

     if (this_obj.counter == SCANS_IN_CYCLE) {
       clearInterval(this_obj.scanTimer);
       this_obj.evaluateScans();
     }
   } catch (err) {
     console.log(err);
   }
}

NetDeviceScanner.prototype.evaluateScans = function() {

  // set all previously found but inactive devices to 0
  for (var mac in this.model.devices) {
    if (this.activeDevices.indexOf(mac) == -1) {
      var message = new IsMessage('dvcIs', mac, 'OFF', new Date());
      this.logToConsole(message);
      this.model.devices[mac].AddValue('OFF');
      if (this.es != null) { this.es.logMessage(message); }
    }
  }

  // log every activeDevice
  for (var i = 0; i < this.activeDevices.length; i++) {
    var message = new IsMessage('dvcIs', this.activeDevices[i], 'ON', new Date());
    this.logToConsole(message);

    if (!(this.activeDevices[i] in this.model.devices)) {
      this.model.devices[this.activeDevices[i]] = new DataStream();
    }
    this.model.devices[this.activeDevices[i]].AddValue('ON');
    if (this.es != null) { this.es.logMessage(message); }

  }

  for (var mac in this.model.devices) {
    var d = this.model.devices[mac].BeforeLastValueDuration();
    if (d !== null) {
            var k = ((this.activeDevices.indexOf(mac) > -1) ? 1 : -1);
            var val = k * (this.model.devices[mac].BeforeLastValueDuration())/60000.0;
            var message = new IsMessage('dvcWas', mac, val, new Date());
            this.logToConsole(message);
            if (this.es != null) { this.es.logMessage(message); }
    }
  }
}

NetDeviceScanner.prototype.scanNetwork = function(this_obj) {
  this_obj.activeDevices = [ ];
  this_obj.counter = 0;
  // every 4 second s within the first minute (15x4s = 60s)
  this_obj.scanTimer = setInterval(this_obj.doScan, SCAN_DELAY, this_obj);
}

NetDeviceScanner.prototype.logToConsole = function(message) {
  console.log((new Date()).toUTCString() + ': ' + message.type + ' ' + message.name + ' je ' + message.value + ' ' + message.time);
}

NetDeviceScanner.prototype.getDevices = function() {
//    return stringifyCurrentValues ({'temperatures': this.temperatures, 'lights': this.lights, 'switches':this.switches, 'requiredTemperatures': this.requiredTemperatures, 'thermostats': this.thermostats, 'heating': this.heating });
    return foundDevices();
}

NetDeviceScanner.prototype.startScanning = function() {
    console.log("Scanning network for devices...");
    this.scanNetwork(this);
    // every 5 minutes
    setInterval(this.scanNetwork, CYCLE_TIME, this);
}

module.exports = NetDeviceScanner;
