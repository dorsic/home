#!/usr/bin/env node

var DataItem = require('./DataItem.js');

function DataStream() {
	this.dataItems = new Array();
};

DataStream.prototype.IsEmpty = function() {
    return (this.dataItems.length == 0);
}

DataStream.prototype.CurrentItem = function() {
    if (this.IsEmpty()) {
        return new DataItem(new Date(), NaN);
    }
    return this.dataItems[this.dataItems.length-1];
}

DataStream.prototype.Since = function() {
	if (this.IsEmpty()) return null;
	if (this.dataItems.length < 1) return null;

	var i = this.dataItems.length - 1;
	var currValue = this.dataItems[i].value;
	while ( i > 0 && currValue == this.dataItems[i-1].value) i--;
	if (i < 0) i = 0
	return this.dataItems[i].time;
}

DataStream.prototype.CurrentValue = function() {
    if (this.IsEmpty()) {
			  var i = new DataItem(new Date(), NaN);
				i["since"] = null;
        return i;
    }
		var i = this.dataItems[this.dataItems.length-1];
		i["since"] = this.Since();
    return i;
}

// duration of the datapoint, not the actual value when it is for example repeated
DataStream.prototype.BeforeLastValueDuration = function() {

    if (this.IsEmpty()) return null;
    if (this.dataItems.length < 2) return null;

    return new Date(this.dataItems[this.dataItems.length-1].time).getTime() -
	new Date(this.dataItems[this.dataItems.length-2].time).getTime();
}

// duration of tha value. how long the beforelast value was present/valid
DataStream.prototype.LastValueChangeDuration = function() {
	if (this.IsEmpty()) return null;
	if (this.dataItems.length < 2) return null;

	var i = this.dataItems.length - 1;
	var currValue = this.dataItems[i].value;
	while ( i > 0 && currValue == this.dataItems[i-1].value) i--;
	var j = i-1;
	if (j < 0) return null;
	currValue == this.dataItems[j].value
	while ( j > 0 && currValue == this.dataItems[j-1].value) j--;
	if (j < 0) return null;

	return new Date(this.dataItems[i].time).getTime() - new Date(this.dataItems[j].time).getTime();
}

DataStream.prototype.CurrentValueDuration = function() {
	if (this.IsEmpty()) return null;
	if (this.dataItems.length < 2) return null;

	var n = this.dataItems.length - 1;
	var i = n
	var currValue = this.dataItems[i].value;
	while ( i > 0 && currValue == this.dataItems[i-1].value) i--;
	if (i < 0) i = 0
	return new Date(this.dataItems[n].time).getTime() - new Date(this.dataItems[i].time).getTime();
}

DataStream.prototype.Values = function(fromDate, toDate) {
    var startIndex = 0;
    var endIndex = this.dataItems.length;
    for (var i = 0, len = this.dataItems.length; i < len; i++) {
	var moved = false;
	if (this.dataItems[i].time < fromDate ) { startIndex = i; moved = true; }
	if (this.dataItems[len-i-1].time > toDate) { endIndex = len-i-1; moved = true; }
	if (!moved) { break; }
    }
    return this.dataItems.slice(startIndex, endIndex+1);
}

DataStream.prototype.LastDayValues = function() {
    var n = new Date();
    return this.Values((new Date).setDate(n.getDate()-1), n);
}

DataStream.prototype.AddItem = function(dataItem) {
    this.dataItems[this.dataItems.length] = dataItem;
    while (this.dataItems.length > 3000) {
	this.dataItems.shift();
    }
}

DataStream.prototype.AddValue = function(value) {
    var di = new DataItem(new Date(), value);
    this.AddItem(di);
}

module.exports = DataStream;
