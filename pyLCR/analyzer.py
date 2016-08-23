import serial
import time
import json
import matplotlib.pyplot as plt
import numpy as np
import math
from pylab import *

mpl.rcParams['font.sans-serif'] = ['Microsoft YaHei Mono']
mpl.rcParams['axes.unicode_minus'] = False 

ttyS0 = serial.Serial('COM8', 115200)
ttyS0.timeout = 1

js = []
str1 = ''

ttyS0.flushInput()
ttyS0.flushOutput()

print('\nstart\n')
#time.sleep(5)

#ttyS0.write(b'zlcr -f 1000\n')
for i in range(0, 250):
        b = ttyS0.readline()[:-1]
        str1 = bytes.decode(b, errors='replace')
        js.append(str1)
        print(len(js))

ttyS0.close()
print('\nstop\n')

x =[]
p = []
y = []

for i in js:
    try:
        tmp = json.loads(i)
        x.append(tmp['FREQ'])
        p.append(tmp['PHASE']*180/np.pi)
        y.append(tmp['MAG']*1000)
    except:
        pass

plt.plot(y)
plt.show()
plt.plot(p)
plt.show()
