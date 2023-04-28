#!/usr/bin/env python3

import paho.mqtt.publish as mqtt  # pip install --upgrade paho-mqtt
import os

hostname = '192.168.1.1'
port = 1883
qos=0
retain=False
auth=None

topic_state = 'rackmonitor/state'
topic_output = 'rackmonitor/output'
topic_name = 'rackmonitor/name'
topic_host = 'rackmonitor/host'
topic_message = 'rackmonitor/message'
topic_r = 'rackmonitor/r'
topic_g = 'rackmonitor/g'
topic_b = 'rackmonitor/b'

led_level = 255

state = os.getenv('SERVICESTATE', '')
output = os.getenv('SERVICEOUTPUT')
name = os.getenv('SERVICEDISPLAYNAME')
host = os.getenv('HOSTDISPLAYNAME')
if state is None or state == '':
    state = os.getenv('HOSTSTATE', '')
    output = os.getenv('HOSTOUTPUT')
    if state is None or state == '':
        state = 'unknown'

if state == 'CRITICAL' or state == 'DOWN':
    r = led_level
    g = 0
    b = 0
if state == 'WARNING':
    r = led_level
    g = led_level
    b = 0
if state == 'UNKNOWN':
    r = led_level
    g = 0
    b = led_level
if state == 'OK' or state == 'UP':
    r = 0
    g = led_level
    b = 0

# payload = None
# try:
#     payload = state
# except:
#     pass

message = state + " " + name + " on " + host

mqtt.single(topic_state, state, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_message, message, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_output, output, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_name, name, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_host, host, qos=qos, retain=retain, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_r, r, qos=qos, retain=True, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_g, g, qos=qos, retain=True, hostname=hostname, port=port, auth=auth)
mqtt.single(topic_b, b, qos=qos, retain=True, hostname=hostname, port=port, auth=auth)