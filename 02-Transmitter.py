import serial
import json
import time
import requests
from pprint import pprint
from time import sleep

try:
	ser=serial.Serial('COM4',9600)
	ser1=serial.Serial('COM6',9600)
except Exception as e:
	print(e)
finalPayload=''
packetRead=False
foundUID=False
api_link='https://rfid-98749.firebaseio.com/.json'
#set data into 'firebase'
data={"card1":'0006493766',"fans":0}
response= requests.put(api_link,json=data)
response= requests.get(api_link)
pprint(response.json())
#call UID from 'firebase'
ids=response.json()["card1"]
pprint(ids)
cmd=0

while True:
	#set speed to be zero when the fan(motor) is off	
	if ser1.inWaiting()>0:
		data={"card1":'0006493766',"fans":0}
		response= requests.put(api_link,json=data)
		cmd=0
	#read every digit of UID	
	while ser.inWaiting()>0:
		packetRead=True
		payload=ser.read()
		payload=payload.decode()
		finalPayload+=payload
	if packetRead:
		packetRead=False
		#to justify the UID of the card
		if finalPayload.find(ids)!=-1:
			foundUID=True
		
		if foundUID:
			print("YAY!!A FAMILIAR RFID TAG")
			cmd+=1
			#to control the motor speed
			if cmd>3:
				cmd=1
			#save the motor speed into 'firebase'
			data={"card1":'0006493766',"fans":cmd}
			response= requests.put(api_link,json=data)
			foundUID=False
		else:
			print("GO AWAY!!YOU’RE NOT WELCOME HERE!")
			cmd=4
			foundUID=False	
		print(cmd)
		#send the result(speed) to arduino
		ser1.write(str(cmd).encode())
