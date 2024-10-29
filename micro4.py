# -*- coding: utf-8 -*-
"""
Created on Sun Oct 27 16:39:55 2024

@author: kevin
"""

import serial
import time
import json
import paho.mqtt.client as mqtt

# Configuración de conexión con ThingsBoard
THINGSBOARD_HOST = 'iot.eie.ucr.ac.cr'
THINGSBOARD_PORT = 1883
ACCESS_TOKEN = 'zb7gbr9dvw2mf01du7qb'  # Token de autenticación de ThingsBoard
TOPIC = 'v1/devices/me/telemetry'  # Topic específico de ThingsBoard

# Configuración del cliente MQTT
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado exitosamente a ThingsBoard")
    else:
        print("Error de conexión, código rc={}".format(rc))

client.on_connect = on_connect
client.username_pw_set(ACCESS_TOKEN)  # Autenticación con ThingsBoard
client.connect(THINGSBOARD_HOST, THINGSBOARD_PORT, 60)
client.loop_start()

# Configuración del puerto serial
serial_port = 'COM7'  # Cambia según tu configuración (e.g., COM3 para Windows)
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate, timeout=1)

def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        print("Datos recibidos:", line)
        return line
    return None

def send_to_thingsboard(battery_level, x, y, z, usb_connected):
    # Estructura de datos en JSON para enviar a ThingsBoard
    telemetry_data = {
        'battery_level': battery_level,
        'gyroscope_x': x,
        'gyroscope_y': y,
        'gyroscope_z': z,
        'usb_connected': usb_connected
    }
    # Envía los datos al topic en ThingsBoard
    client.publish(TOPIC, json.dumps(telemetry_data))
    print("Datos enviados a ThingsBoard:", telemetry_data)

# Bucle principal de lectura y envío
try:
    while True:
        data = read_serial_data()
        if data:
            # Parsear datos recibidos; asumiendo que llegan en formato CSV: "battery,x,y,z,usb_status"
            try:
                battery, x, y, z, usb_status = data.split(',')
                battery_level = int(battery)
                x_axis = int(x)
                y_axis = int(y)
                z_axis = int(z)
                usb_connected = bool(int(usb_status))
                
                # Enviar datos a ThingsBoard
                send_to_thingsboard(battery_level, x_axis, y_axis, z_axis, usb_connected)
                time.sleep(0.1)  # Ajusta según la frecuencia de envío deseada
            except ValueError as e:
                print("Error al parsear los datos:", e)
         
except KeyboardInterrupt:
    print("Interrumpido por el usuario")
finally:
    ser.close()
    client.loop_stop()
    client.disconnect()
