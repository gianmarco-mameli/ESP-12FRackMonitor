#!/usr/bin/env python3

import paho.mqtt.publish as mqtt  # pip install --upgrade paho-mqtt
import os

hostname = '192.168.1.1'
port = 1883
qos=0
retain=False
auth=None

topic = 'rackmonitor/state'
topic_r = 'rackmonitor/r'
topic_g = 'rackmonitor/g'
topic_b = 'rackmonitor/b'


state = os.getenv('SERVICESTATE', '').lower()
if state is None or state == '':
    state = os.getenv('HOSTSTATE', '').lower()
    if state is None or state == '':
        state = 'unknown'

if state == 'critical' or state == 'down':
    r = 50
    g = 0
    b = 0
if state == 'warning':
    r = 50
    g = 50
    b = 0
if state == 'unknown':
    r = 50
    g = 0
    b = 50
if state == 'ok' or state == 'up':
    r = 0
    g = 50
    b = 0

payload = None
try:
    payload = state
except:
    pass

mqtt.single(topic, state, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_r, r, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_g, g, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_b, b, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)