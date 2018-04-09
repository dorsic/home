#pip install requests
#pip install python-pushsafer
#pip install apscheduler
#pip install dateutil

#log in to https://www.pushsafer.com/ via fmefi@centrum.sk/dsdsj5ma or just fieeki and set up the events

import sys
import requests
import logging
from datetime import datetime, timedelta
from dateutil import parser
from dateutil.tz import tzutc
from pushsafer import init, Client

from apscheduler.schedulers.blocking import BlockingScheduler

TEMP_TOO_LOW = 19.5
TEMP_TOO_HIGH = 23.0
HEATING_LONG_ON = 6  # hours
REQUIRED_TEMPERATURE_SETTING_THRESHHOLD = 500
DASHBOARD_URL = 'http://obd.today:3333'

# Message Types
MT_TEST_MESSAGE = 0
MT_DASHBOARD_DOWN = 1			# send max. every 12h
MT_HEATING_LONG_ON = 2			# send max. every 6h
MT_TEMP_TOO_HIGH = 3			# send max. every 12h
MT_TEMP_TOO_LOW = 4				# send max. every 12h
MT_REQUIRED_TEMP_CHANGED = 5	# send max. every 5min 
MT_TEMP_OUTSIDE = 6				# send at 0700 and 0730 AM
MT_ALL_LIGHTS_OFF = 7			# send at 0900 AM in case of event
MT_ISMCS_DOWN = 8
MT_ISMCSGIS_DOWN = 9

# Pushsafer configuration
# secure ID HbsZo1UgOxna4oRPfn7x
# iPhone7 deviceId 9209
# groupID

IPHONE7 = 9209

# init pushsafer API

init("HbsZo1UgOxna4oRPfn7x")
pushsafer = Client("")
# sentMessages stores key-value pairs where key is the message type and value is the time a message of that type was sent
# this collection is used to limit sent messages
sentMessages = {}

def getHeatingOnHours(status):
	if (status['heating']['value'] == 'OFF'): 
		return 0
	if (status['heating']['value'] == None):
		return 0

	since = parser.parse(status['heating']['since'])
	now = datetime.now(tzutc())
	return (now-since).seconds/3600

def isHeatingLongOn(status):
	return True if (getHeatingOnHours(status) >= HEATING_LONG_ON) else False

def getSensorTemperature(status, sensor):
	return float(status['temperatures'][sensor]['value'])

def getRequiredTemperature(status, sensor):
	return float(status['requiredTemperatures'][sensor]['value'])

def getOutsideTemperature(status):
	return getSensorTemperature(status, 'balkon')

def sensorTemperatureTooHigh(status):
	for sensor in ['obyvacka7', 'predspalnou', 'kuchyna', 'kupelna', 'spalnicka']:
		if getSensorTemperature(status, sensor) > TEMP_TOO_HIGH:
			return sensor
	return None

def sensorTemperatureTooLow(status):
	for sensor in ['obyvacka7', 'predspalnou', 'kuchyna', 'kupelna']:
		if getSensorTemperature(status, sensor) < TEMP_TOO_LOW:
			return sensor
	return None

def requiredTemperatureChanged(status):
	now = datetime.now(tzutc())
	for sensor in status['requiredTemperatures']:
		since = status['requiredTemperatures'][sensor]['since']
		if since != None:
			since = parser.parse(since)
			diff = (now-since).seconds
			if 0 < diff and diff < REQUIRED_TEMPERATURE_SETTING_THRESHHOLD:
				print("Required temperature chaged before " + str(diff) + " seconds.")
				return sensor
	return None

def lightsOn(status):
	cnt = 0
	for light in status['lights']:
		if status['lights'][light]['value'] == 'ON':
			cnt = cnt + 1
	return cnt


#def isAppartmentRequestedTempUpdated(status):
#	return -1

# return true if the notification should be send
# returns false if a notification of that type has already been send in given time span
def sendOrLimitNotification(messageType):
	lastMessageTime = sentMessages[messageType] if messageType in sentMessages.keys() else datetime(2000, 1, 1)
	now = datetime.now()
	timeDiff = (now-lastMessageTime).seconds
	if (messageType == MT_DASHBOARD_DOWN and timeDiff <= 12*3600):
		return False
	if (messageType == MT_HEATING_LONG_ON and timeDiff <= 6*3600):
		return False
	if (messageType == MT_TEMP_TOO_HIGH and timeDiff <= 12*3600):
		return False
	if (messageType == MT_TEMP_TOO_LOW and timeDiff <= 12*3600):
		return False
	if (messageType == MT_REQUIRED_TEMP_CHANGED and timeDiff <= 7*60):
		return False
	if (messageType == MT_ISMCS_DOWN and timeDiff <= 2*3600):
		return False
	if (messageType == MT_ISMCSGIS_DOWN and timeDiff <= 3*3600):
		return False

	return True


def sendNotification(messageType, message, title, device, icon):
	try:
		print ("Request for sending notification (" + title + "): " + message)

		if (sendOrLimitNotification(messageType)):
			print ("Sending notification (" + title + "): " + message)
			pushsafer.send_message(message, title, device, icon, None, None, "http://obd.today:3333", "Open Dashboard", "0", "", "", "")
			print ("Notification sent.")
			sentMessages[messageType] = datetime.now()
		else:
			print ("Nofitication ignored due to limits.")
	except:
		print ("Unable to sent notification. " + str(sys.exc_info()[0]))

def getStatus():
	print (str(datetime.now()) + " > Getting actual status...")
	try:
		r = requests.get(DASHBOARD_URL + "/status")
		if (r.status_code) != 200:
			sendNotification(MT_DASHBOARD_DOWN, "Dashboard returns HTTP code " + str(r.status_code) + ".", "Dashboard Non-responding", IPHONE7, 132)
			return None
		return r.json()
	except :
		sendNotification(MT_DASHBOARD_DOWN, "Dashboard API not responding. Server down.", "Dashboard Not Responding", IPHONE7, 132)
		return None

def execute():
	status = getStatus()
	try:
		if (isHeatingLongOn(status)):
			sendNotification(MT_HEATING_LONG_ON, "Heating is already " + str(getHeatingOnHours(status)) + " hours ON.", "Heating Long ON", IPHONE7, 91)
	except :
		print ("Exception Heating Long On.")

	try:
		sensor = sensorTemperatureTooHigh(status)
		if (sensor != None):
			sendNotification(MT_TEMP_TOO_HIGH, "Temperature " + str(getSensorTemperature(status, sensor)) + " in " + sensor + " is too high.", \
				"Temperature Too High", IPHONE7, 62)
	except:
		print ("Exception Sensor Temperature Too High.")

	try:
		sensor = sensorTemperatureTooLow(status)
		if (sensor != None):
			sendNotification(MT_TEMP_TOO_LOW, "Temperature " + str(getSensorTemperature(status, sensor)) + " in " + sensor + " is too low.", \
				"Temperature Too Low", IPHONE7, 63)
	except:
		print ("Exception Sensor Temperature Too Low.")

	try:	
		sensor = requiredTemperatureChanged(status)
		if (sensor != None):
			sendNotification(MT_REQUIRED_TEMP_CHANGED, "New temperature " + str(getRequiredTemperature(status, sensor)) + " set in " + sensor + ".", \
				"Required Temperature Changed", IPHONE7, 91)
	except:
		print ("Exception Required Temperature Changed.")

	try:
		print("Checking Portal IS MCS")
		r = requests.get("http://ismcs.cdb.sk/portal")
		if (r.status_code) != 200:
			sendNotification(MT_ISMCS_DOWN, "Portal IS MCS returns HTTP code " + str(r.status_code) + ".", \
				"IS MCS Non-responding", IPHONE7, 132)

		print("Checking GIS IS MCS")
		r = requests.get("https://ismcs.cdb.sk/portal/Services/EsriProxyService/proxy.ashx?https://gis02.ismcs.sk:6443/arcgis/rest/services?f=json")
		if (r.status_code) != 200:
			 sendNotification(MT_ISMCS_DOWN, "GIS portal IS MCS returns HTTP code " + str(r.status_code) + ".", \
                                "GIS IS MCS Non-responding", IPHONE7, 132)

		print("Checking GIS WMS")
		r = requests.get("https://ismcs.cdb.sk/portal/services/EsriProxyService/proxy.ashx?https://kataster.skgeodesy.sk/eskn/services/NR/kn_wms_orto/MapServer/WmsServer?SERVICE=WMS&REQUEST=GetMap&FORMAT=image%2Fpng&TRANSPARENT=TRUE&STYLES=&VERSION=1.3.0&LAYERS=0%2C1%2C2%2C3%2C5%2C6%2C7%2C8%2C9%2C10&WIDTH=1209&HEIGHT=429&CRS=EPSG%3A25834&BBOX=342957.8094572855%2C5411395.600499534%2C344557.2189061044%2C5411963.132884598")
		if (r.status_code) != 200:
			r = requests.get("https://kataster.skgeodesy.sk/eskn/services/NR/kn_wms_orto/MapServer/WmsServer?SERVICE=WMS&REQUEST=GetMap&FORMAT=image%2Fpng&TRANSPARENT=TRUE&STYLES=&VERSION=1.3.0&LAYERS=0%2C1%2C2%2C3%2C5%2C6%2C7%2C8%2C9%2C10&WIDTH=1209&HEIGHT=429&CRS=EPSG%3A25834&BBOX=342957.8094572855%2C5411395.600499534%2C344557.2189061044%2C5411963.132884598")
			if (r.status_code) == 200:
				 sendNotification(MT_ISMCS_DOWN, "WMS on Portal IS MCS returns HTTP code " + str(r.status_code) + ".", \
	                                "WMS IS MCS Non-responding", IPHONE7, 132)
	except:
		print ("Exception portal ismcs")


def executeOutsideTemperature():
	print(str(datetime.now()) + " > Getting outside temperature...")
	status = getStatus()
	sendNotification(MT_TEMP_OUTSIDE, " is " + str(getOutsideTemperature(status)) + ".", "Outside Temperature", IPHONE7, 60)

def executeLightsOff():
	print(str(datetime.now()) + " > Checking all lights OFF...")
	status = getStatus()
	cnt = lightsOn(status)
	if (cnt > 0):
		sendNotification(MT_ALL_LIGHTS_OFF, str(cnt) + " light are still ON.", "All Lights OFF", IPHONE7, 93)

#sendNotification(MT_TEST_MESSAGE, "Heating is already " + str(8) + " hours ON.", "Heating Long ON", IPHONE7, 91)

scheduler = BlockingScheduler()
scheduler.add_job(execute, 'interval', minutes=1)
scheduler.add_job(executeOutsideTemperature, trigger='cron', hour='7', minute='0')
scheduler.add_job(executeOutsideTemperature, trigger='cron', hour='7', minute='30')
scheduler.add_job(executeLightsOff, trigger='cron', hour='9', minute='0', second='0')
scheduler.start()
