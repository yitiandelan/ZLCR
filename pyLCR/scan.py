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
ttyS0.write(b'zlcr -f 100.0\n')
time.sleep(5);

for i in np.arange(math.log10(200), math.log10(92000), 0.05):
    ttyS0.write(('zlcr -f ' + str(math.pow(10,i)) + '\n').encode())
    time.sleep(1)
    ttyS0.flushInput()
    ttyS0.readline()
    b = ttyS0.readline()
    str1 = bytes.decode(b, errors='replace')
    js.append(str1)
    print(str1)

ttyS0.close()
print('\nstop\n')

x =[]
p = []
y = []

for i in js:
    try:
        tmp = json.loads(i)
        p.append(tmp['PHASE']*360/2/np.pi)
        y.append(tmp['MAG']*1000)
        x.append(tmp['FREQ'])
    except:
        pass

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.plot(x, y)
ax1.set_yscale('log')
ax1.grid(True, 'both')
#ax1.set_ylim(1e-5, 1e5)
ax1.set_ylabel(u'log|z|')
ax1.set_xlabel(u'log f')

ax2 = ax1.twinx()
ax2.plot(x, p, 'r')
ax2.set_ylim(-100, 100)
ax2.set_ylabel(u'Phase')

#plt.plot(x,y)
plt.xscale('log')
#plt.yscale('log')
#plt.xlim(1e1, 1e5)
#plt.ylim(1e-3, 1e5)
plt.title(u'阻抗谱')
#plt.xlabel(u'log f')
plt.grid(True, 'both')
plt.savefig('figure.png')
plt.show()
