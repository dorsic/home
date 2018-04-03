var elasticsearch = require('elasticsearch');

//var IsMessage = require('./IsMessage.js');
var es = null;

function ElasticsearchLogger(hostUrl) {
    es = new elasticsearch.Client({host: hostUrl});
};

ElasticsearchLogger.prototype.onOffToBool = function(onOffString) {
    if (onOffString == 'ON') return true;
    else return false;
}

ElasticsearchLogger.prototype.logMessage = function(message) {
  try {
      idx = "";
      message.double_value = null;
      message.string_value = null;
      message.bool_value = null;
      switch (message.type) {
        case 'rqtIs':
        case 'htnWas':
        case 'thtWas':
        case 'lgtWas':
        case 'dvcWas':
        case 'tmpIs': message.double_value = message.value; break; //idx = 'hddbl'; break;
        case 'swcIs': message.string_value = message.value.toLowerCase(); break; // idx = 'hdstr'; message.value = message.value.toLowerCase(); break;
        case 'htnIs':
        case 'thtIs':
        case 'dvcIs':
        case 'lgtIs':  message.bool_value = this.onOffToBool(message.value); break; // idx = 'hdbool'; message.value = this.onOffToBool(message.value);  break;
        case 'tmpAre': return;
      }
      delete message['value']; 
      es.index({
        index: "home",
        type: "event",
        method: 'POST',
        body: {
          time: message.time, //'new Date().toUTCString()',
          type: message.type,
          name: message.name.toLowerCase(),
          double_value: message.double_value,
          string_value: message.string_value,
          bool_value: message.bool_value
//          value: message.value
        }
      }, function (error, response) {
        if (error) {
          console.log(error);
        }
      });
  } catch (err) {
      console.log(err);
  }
}

module.exports = ElasticsearchLogger;
