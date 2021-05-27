import serial
from time import sleep
import paho.mqtt.client as mqtt

ORG = "a4zasv"
DEVICE_TYPE = "Pi3B"
TOKEN = "0EwsnyKK6xxjiSL-l2"
DEVICE_ID = "667711"

server = ORG + ".messaging.internetofthings.ibmcloud.com"
pubTopic1 = "iot-2/evt/status1/fmt/json"
pubTopic2 = "iot-2/evt/status2/fmt/json"
authMethod = "use-token-auth"
token=TOKEN
clientId = "d:" + ORG + ":" + DEVICE_TYPE + ":" +DEVICE_ID

mqttc = mqtt.Client(client_id=clientId)
mqttc.username_pw_set(authMethod, token)
mqttc.connect(server, 1883, 60)

sht31=serial.Serial('/dev/ttyACM0',9600)
while(1):
    readText = sht31.readline()
    temp=readText[1:5]
    hum=readText[6:10]
    temp_str = "{\"temperatura\":" + str(temp) + "}"
    hum_str = "{\"wilgotnosc\":" + str(hum) + "}"
    mqttc.publish(pubTopic1, temp_str)
    mqttc.publish(pubTopic2, hum_str)
    print(temp + " " + hum)

mqttc.loop_forever()