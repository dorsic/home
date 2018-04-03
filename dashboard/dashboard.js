#!/usr/bin/env node

var events = require("events");
var express = require('express');
var bodyParser = require('body-parser');
var appPort = 1234;
var app = express();
var http = require('http');
var server = http.createServer(app);
var socket = require('socket.io');
var io = socket.listen(app.listen(appPort));

var SerialInterpreter = require('./SerialInterpreter.js');
var PalisadyModel = require('./PalisadyModel.js');
var IsMessage = require('./IsMessage.js');
var NamesMap = require('./NamesMap.js');
var ElasticsearchLogger = require('./ElasticsearchLogger.js');
var NetDeviceScanner = require('./NetDeviceScanner.js');

var namesMap = new NamesMap();
var stdin = process.openStdin();
//var serial = new SerialInterpreter('/dev/tty.usbserial-AH01GKWI');
//var serial = new SerialInterpreter('/dev/tty.usbmodemfa131');
var serial = new SerialInterpreter('/dev/ttyACM0');
var model = new PalisadyModel();
var esl = new ElasticsearchLogger('http://192.168.1.67:9200/');

var netScanner = new NetDeviceScanner(model, esl);
netScanner.startScanning();

app.set('views', __dirname + '/views');
//app.set('view engine', "jade");
//app.engine('jade', require('jade').__express);
app.use(express.static(__dirname + '/public'));
app.use(bodyParser.json());

app.get('/status', function(req, res) {
	try {
    	res.send(model.CurrentStatus());
    } catch (err) {
    	console.log('Error sending status. ', err);
    }
});

app.get('/currentStatus', function(req, res) {
	try {
	    res.send(model.CurrentStatus());
    } catch (err) {
    	console.log('Error sending current status. ', err);
    }
});

app.get('/lights/:name/:value', function(req, res) {
	try {
	    console.log('received request for light ' + req.params.name + ' ' + req.params.value);
	    serial.sendCommand('lgt ' + namesMap.lightIndex(req.params.name) + ' ' + req.params.value);
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error sending ligths. ', err);
    }    
});

app.get('/requiredTemperatures/:name/:value', function(req, res) {
	try {
	    console.log('received request for required temperature ' + req.params.name + ' ' + req.params.value);
	    serial.sendCommand('rqt ' + namesMap.thermostatIndex(req.params.name) + ' ' + req.params.value);
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error sending required temperature. ', err);
    }
});

app.get('/temperature/:name/:value', function(req, res) {
	try {
	    console.log('received request to log temperature ' + req.params.name + ' ' + req.params.value);
	    var message = new IsMessage('tmpIs', req.params.name, req.params.value, new Date());
	    esl.logMessage(message);
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error sending temperature. ', err);
    }
});

app.get('/logElastic/:type/:name/:value', function(req, res) {
	try {
	    console.log('received request to log temperature ' + req.params.name + ' ' + req.params.value);
	    var message = new IsMessage(req.params.type, req.params.name, req.params.value, new Date());
	    esl.logMessage(message);
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error logging to elastic. ', err);
    }
});

app.get('/thermostats/:name/:value', function(req, res) {
	try {
	    console.log('received request for thermostat ' + req.params.name + ' ' + req.params.value);
	    serial.sendCommand('tht ' + namesMap.thermostatIndex(req.params.name) + ' ' + req.params.value);
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error sending thermostats. ', err);
    }
});

app.get('/heating', function(req, res) {
	try {
    	res.send(model.heating.CurrentValue());
    } catch (err) {
    	console.log('Error sending heating. ', err);
    }
});

app.get('/heating/:value', function(req, res) {
	try {
	    console.log('received request for heating ' + req.params.value);
	    serial.sendCommand('htn ' + req.params.value + '\n');
	    res.send('{"message": "command executed"}');
    } catch (err) {
    	console.log('Error sending heating value. ', err);
    }    
});

app.get('/:type/:name/currentStatus', function(req, res) {
	try {
    	res.send(model[req.params.type][req.params.name].CurrentValue());
    } catch (err) {
    	console.log('Error sending generic current status. ', err);
    }    
});

app.get('/:type/:name/lastValueChangeDuration', function(req, res) {
	try {
	    var result = model[req.params.type][req.params.name].LastValueChangeDuration();
	    console.log(req.params.type + '/' + req.params.name + '/lastValueChangeDuration: ' + result);
	    res.send(result);
    } catch (err) {
    	console.log('Error sending last value change duration. ', err);
    }	    
});

app.get('/:type/:name/currentValueDuration', function(req, res) {
	try {
	    var value = model[req.params.type][req.params.name].CurrentValueDuration();
	    var result = '{"' + req.params.type + '": {"' + req.params.name + '": { "currentValueDuration": ' + value + '}}}';
	    console.log(req.params.type + '/' + req.params.name + '/currentValueDuration: ' + result);
	    res.send(result);
    } catch (err) {
    	console.log('Error sending current value duration. ', err);
    }	    
});

app.get('/:type/:name', function(req, res) {
	try {
    	res.send(model[req.params.type][req.params.name].CurrentValue());
    } catch (err) {
    	console.log('Error sending generic type name. ', err);
    }    	
});
app.get('/', function(req, res) {
	try {
	    res.render('page.jade', {title: 'Home Dashboard'});
//    res.send(__dirname + '/views/index.html');
    } catch (err) {
    	console.log('Error sending app.', err);
    }
});

app.post('/logElastic', function(req, res) {
    if ((typeof(req.body) !== 'undefined') || (req.body !== null)) {
      if ((typeof(req.body.time) === 'undefined') || (req.body.time === null)) req.body.time = new Date();
      var message = new IsMessage(req.body.type, req.body.name, req.body.value, req.body.time);
      esl.logMessage(message);
      res.send('{"message": "data received"}');
    } else {
      res.send('{"message": "empty body"}');
    }
});

app.post('/', function(req, res){
  if ((typeof(req.body) !== 'undefined') || (req.body !== null)) {
    if ((typeof(req.body.time) === 'undefined') || (req.body.time === null)) req.body.time = new Date();
    var message = new IsMessage(req.body.type, req.body.name, req.body.value, req.body.time);
    switch (message.type) {
      case 'rqtIs': handleRqtIs(message);
    }
    res.send('{"message": "data received"}');
  } else {
    res.send('{"message": "empty body"}');
  }
});

function logToConsole(message) {
    console.log((new Date()).toUTCString() + ': ' + message.type + " " + " " + message.name + " " + message.value + ' je ' + message.time);
}

function negateOnOFF(value) {
  if (value == 'ON') return 'OFF';
  else return 'ON';
}


function onOffToBool(onOffString) {
    if (onOffString == 'ON') return 1;
    else return 0;
}

function handleRqtIs(message)
{
	try {
	    logToConsole(message);
	    model.requiredTemperatures[message.name].AddValue(message.value);
	    io.sockets.emit(message.type, message);
	    esl.logMessage(message);
	} catch (err) {
    	console.log(err);
    }
}

try {
    serial.on('open', function(data) {
//        serial.sendCommand("ON\n");
        console.log('serial port opened. "' + data + '"');
        serial.sendCommand("rqt 7 21.1");
        serial.sendCommand("rqt 2 20.3");
        serial.sendCommand("crt 7 0.0");
        serial.sendCommand("crt 8 0.0");
        serial.sendCommand("crt 9 0.0");
        serial.sendCommand("crt 10 0.0");
        serial.sendCommand("tmpa?");
    });

    serial.on('lgtIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.lights[data.name].AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);

	        data.type = 'lgtWas';
	        k = ((data.value == 1) ? -1.0 : 1.0);
	        data.value = k * model.lights[data.name].BeforeLastValueDuration()/60000.0;
	        esl.logMessage(data);
		} catch (err) {
	    	console.log(err);
	    }
    });

    serial.on('swcIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.switches[data.name].AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);
		} catch (err) {
	    	console.log(err);
	    }
    });

    serial.on('tmpIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.temperatures[data.name].AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);
		} catch (err) {
	    	console.log(err);
	    }
    });

    serial.on('tmpAre', function(data) {
    	try {
	       for (i = 0; i < data.value.length; i++) {
	         d = new IsMessage('tmpIs', namesMap.tempName(i), data.value[i], data.time);
	         model.temperatures[d.name].AddValue(d.value);
	         io.sockets.emit(d.type, d);
	         esl.logMessage(d);
	       }
		} catch (err) {
	    	console.log(err);
	    }
    });

    serial.on('htnIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.heating.AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);

	        data.type = 'htnWas';
	        k = ((data.value == 1) ? -1 : 1);
	        data.value = k * (model.heating.BeforeLastValueDuration())/60000.0;
	        esl.logMessage(data);
      	} catch (err) {
	    	console.log(err);
	    }
    });

    serial.on('thtIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.thermostats[data.name].AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);

	        data.type = 'thtWas';
	        k = ((data.value == 1) ? -1 : 1);
	        data.value = k * (model.thermostats[data.name].BeforeLastValueDuration())/60000.0;
	        esl.logMessage(data);
		} catch (err) {
	    	console.log(err);
	    }	        
    });

    serial.on('rqtIs', function(data) {
    	try {
	//        logToConsole(data);
	        model.requiredTemperatures[data.name].AddValue(data.value);
	        io.sockets.emit(data.type, data);
	        esl.logMessage(data);
		} catch (err) {
	    	console.log(err);
	    }        
    });

    serial.on('dorIs', function(data) {
        logToConsole(data);
    });

    serial.on('pwrIs', function(data) {
        logToConsole(data);
    });

    serial.on('ready', function() {
        console.log("Serial communication ready.");
        serial.sendCommand("rqt 7 21.1");
        serial.sendCommand("rqt 2 20.3");
        serial.sendCommand("crt 7 0.0");
        serial.sendCommand("crt 8 0.0");
        serial.sendCommand("crt 9 0.0");
        serial.sendCommand("crt 10 0.0");
        serial.sendCommand("tmpa?");        
    });

    serial.open();

    setInterval(function() {
        serial.sendCommand("tmpa?");
    }, 300000);

} catch (err) {
    console.log(err);
}

stdin.on('data', function(d) {
	try {
	    if (dx.toString() == '?\n') {
	    } else {
	        serial.sendCommand(d.toString());
	    }
	} catch (err) {
		console.log("Error reading serial port.", err);
	}
});

io.sockets.on('connection', function(soc) {
    console.log('new connection to socket.');
});
