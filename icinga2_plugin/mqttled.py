#!/usr/bin/env python3

import paho.mqtt.publish as mqtt  # pip install --upgrade paho-mqtt
import os

HOSTNAME = '192.168.1.1'
PORT = 1883
QOS = 0
RETAIN = False
AUTH = None

TOPIC_STATE = 'rackmonitor/state'
TOPIC_OUTPUT = 'rackmonitor/output'
TOPIC_NAME = 'rackmonitor/name'
TOPIC_HOST = 'rackmonitor/host'
TOPIC_MESSAGE = 'rackmonitor/message'
TOPIC_R = 'rackmonitor/r'
TOPIC_G = 'rackmonitor/g'
TOPIC_B = 'rackmonitor/b'
TOPIC_COLOR = 'rackmonitor/color'

LED_LEVEL = 255

STATE = os.getenv('SERVICESTATE', '')
OUTPUT = os.getenv('SERVICEOUTPUT')
NAME = os.getenv('SERVICEDISPLAYNAME')
HOST = os.getenv('HOSTDISPLAYNAME')
if STATE is None or STATE == '':
    STATE = os.getenv('HOSTSTATE', '')
    OUTPUT = os.getenv('HOSTOUTPUT')
    if STATE is None or STATE == '':
        STATE = 'unknown'

if STATE == 'CRITICAL' or STATE == 'DOWN':
    R = LED_LEVEL
    G = 0
    B = 0
    COLOR = 'red'
if STATE == 'WARNING':
    R = LED_LEVEL
    G = LED_LEVEL
    B = 0
    COLOR = 'yellow'
if STATE == 'UNKNOWN':
    R = LED_LEVEL
    G = 0
    B = LED_LEVEL
    COLOR = 'purple'
if STATE == 'OK' or STATE == 'UP':
    R = 0
    G = LED_LEVEL
    B = 0
    COLOR = 'green'

# payload = None
# try:
#     payload = state
# except:
#     pass

MESSAGE = STATE + " " + NAME + " on " + HOST

mqtt.single(TOPIC_STATE, STATE, qos=QOS, retain=RETAIN, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_MESSAGE, MESSAGE, qos=QOS, retain=RETAIN, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_OUTPUT, OUTPUT, qos=QOS, retain=RETAIN, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_NAME, NAME, qos=QOS, retain=RETAIN, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_HOST, HOST, qos=QOS, retain=RETAIN, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_R, R, qos=QOS, retain=True, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_G, G, qos=QOS, retain=True, hostname=HOSTNAME, port=PORT, auth=AUTH)
mqtt.single(TOPIC_B, B, qos=QOS, retain=True, hostname=HOSTNAME, port=PORT, auth=AUTH)
