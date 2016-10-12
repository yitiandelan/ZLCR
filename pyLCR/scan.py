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
#ttyS0.write(b'zlcr -f 1.0\n')
time.sleep(1);

for i in np.arange(math.log10(100), math.log10(90000)+0.1, 0.1):
    f = math.pow(10,i)
    
    ttyS0.write(('zlcr -f {:e}\n'.format(f)).encode())
    if(f < 50):
        time.sleep(20)
    else:
        time.sleep(2)
    
    ttyS0.flushInput()
    ttyS0.readline()
    ttyS0.readline()
    str1 = bytes.decode(ttyS0.readline(), errors='replace')
    js.append(str1)
    print(str1)

ttyS0.close()
print('\nstop\n')
fn = open('data.json', 'w')
json.dump(js, fn)
fn.close()

x = []
p = []
y = []
Cs = []
Ls = []
Rs = []
D = []
Q = []

for i in js:
    try:
        tmp = json.loads(i)
        p.append(tmp['PHASE']*360/2/np.pi)
        y.append(tmp['MAG']*1000)
        x.append(tmp['FREQ'])

        Cs.append(abs(1/2/np.pi/x[-1]/y[-1]/sin(radians(p[-1]))))
        Ls.append(y[-1]*sin(radians(p[-1]))/(2*np.pi*x[-1]))
        Rs.append(y[-1]*cos(radians(p[-1])))
        Q.append(tan(abs(radians(p[-1]))))
        D.append(1/Q[-1])
    except:
        pass

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.plot(x, y)
ax1.set_yscale('log')
ax1.grid(True, 'both')
ax1.set_ylabel(u'log|z|')
ax1.set_xlabel(u'log f')

ax2 = ax1.twinx()
ax2.plot(x, p, 'r')
ax2.set_ylim(-100, 100)
ax2.set_yticks(range(-90,100,30))
ax2.set_ylabel(u'Phase')

#plt.plot(x,y)
plt.xscale('log')
plt.title(u'阻抗谱')
plt.grid(True, 'both')
plt.savefig('figure.png')
plt.show()
