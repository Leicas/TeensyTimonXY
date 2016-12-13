import serial
import time
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=.1)
while True:
   ms = str(1.0) + '\n'
   arduino.write(ms.encode())
   time.sleep(1)
   ms = str(-0.5) + '\n'
   arduino.write(ms.encode())
   time.sleep(1)
   ms = str(1.5) + '\n'
   arduino.write(ms.encode())
   time.sleep(1)
   ms = str(-1.5) + '\n'
   arduino.write(ms.encode())
   time.sleep(1)
