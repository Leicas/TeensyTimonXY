import serial
import operator
import time
import math
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=.1)
ms = str(int(round(time.perf_counter()*1000.0)))
msf = 0
incr = 0
posx=0
posy=0
nb=0
addx=0
addy=0
sum = [0.,0.,0.,0.,0.,0.]
while True:
   data = arduino.readline()#the last bit gets rid of the new-line chars
   if data:
                msn = str(data.decode())
                ms = str(int(round(time.perf_counter()*1000.0*1000.0)))
                floats = [float(x) for x in msn.split()]
                #sum[:]=sum[:]+floats[:]
                if len(floats) == 6:
                   sum = list(map(operator.add, sum, floats))
                #print("%.2f" %sum[0] +" " +"%.2f" % sum[1] \
                #+ " %.2f" %sum[2] +" " +"%.2f" %sum[3] \
                #+ " %.2f" %sum[4] +" " +"%.2f" %sum[5])
                   if floats[0] != 0. or floats[1] !=0.:
                      addx-=-floats[0]
                      addy+=-floats[1]
                      nb+=1
                   if floats[2] !=0. or floats[3] !=0.:
                      addx+=-floats[2]*math.cos(0.5236)+floats[3]*math.sin(0.5236)
                      addy+=-floats[2]*math.sin(0.5236)-floats[3]*math.cos(0.5236)
                      nb+=1
                   if floats[4] !=0. or floats[5] !=0.:
                      addx+=-floats[4]*math.cos(2.618)+floats[5]*math.sin(2.618)
                      addy+=-floats[4]*math.sin(2.618)-floats[5]*math.cos(2.618)
                      nb+=1
                   if nb >= 1:
                      posx+=addx/nb
                      posy+=addy/nb
                      nb=0
                      addy=0
                      addx=0
                # print("%.2f" %posx + " " + "%.2f" %posy)
                incr+=1
                if incr == 100:
                   incr=0
                   print("%.2f" %posx + " " + "%.2f" %posy)
                   print("freq:" + str(100.0*1000.0*1000.0/(int(ms)-msf)))
                   msf = int(ms)
                #incr+=1
#   print("newboucle")
