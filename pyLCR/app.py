import serial
import time
import json
import matplotlib.pyplot as plt
import numpy as np

ttyS0 = serial.Serial('COM8', 115200)
ttyS0.timeout = 1

js = [0] *100
str1 = ''

#fig = plt.figure()
#fig.show()

ttyS0.flushInput()
ttyS0.flushOutput()

print('\nstart\n')
ttyS0.write(b'zlcr -f 10000.0\n')

while ttyS0.isOpen():
    try:
        if(ttyS0.inWaiting() > 1000):
            ttyS0.read_all()
            str1 = ''
        elif(ttyS0.inWaiting() > 2):
            b = ttyS0.readline()[:-1]
            str1 = bytes.decode(b, errors='replace')
            
            if(str1[0] == '{' and str1[-1] == '}' and str1.isprintable()):
                js.pop(0)
                js.append(json.loads(str1))
                str1 = '{:.2f} Hz, {:.6f} kohm, {:.4f} °'.format(js[-1]['FREQ'], js[-1]['MAG'], js[-1]['PHASE'])
                print(str1)
        else:
            time.sleep(0.01)
    except Exception as e:
        print(e)
        ttyS0.close()
        
print('\nstop\n')

#plt.hist(mag,100,normed=1,facecolor='g',alpha=0.75)
#plt.grid(True)
#fig.canvas.draw()
