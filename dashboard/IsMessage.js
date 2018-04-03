#!/usr/bin/env node

function IsMessage(type, name, value, time) {
	this.type = type;
	this.name = name;
	this.value = value;
	this.time = time;
};

module.exports = IsMessage;