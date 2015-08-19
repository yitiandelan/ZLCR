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
x = []
p = []
y = []
str1 = ''

ttyS0.flushInput()
ttyS0.flushOutput()

print('\nstart\n')
time.clock()
num = 2000
time1 = 0

ttyS0.write(b'zlcr -f 10000\n')

while not num == 0:
    num = num - 1
    b = ttyS0.readline()[:-1]
    str1 = bytes.decode(b, errors='replace')
    try:
        tmp = json.loads(str1)
        x.append(tmp['FREQ'])
        p.append(tmp['PHASE']*180/np.pi)
        y.append(tmp['MAG']*1000)
        if len(x) >= 100:
            tmp = y[-100:]
            err = (max(tmp) - min(tmp))/sum(tmp)*len(tmp)
            tmp = p[-100:]
            err1= (max(tmp) - min(tmp))/90
            print('{}\t{:.4f} %,{:.4f} %'.format(len(x),err*100,err1*100))
            if (err > 0.01) or (abs(err1) > 0.01):
                time1 = time.clock()
            elif num > 100:
                num = 100
            
        else:
            print(len(x))
    except:
        pass

ttyS0.close()

time2 = 0
time3 = 0
time4 = time.clock()
timetick = time4/len(x)

for i in range(0,len(y)):
    tmp = p[i:]
    rms = 90
    err = (max(tmp) - min(tmp))/rms
    if abs(err) > 0.01:
        time3 = i*timetick
        
    tmp = y[i:]
    rms = sum(y[-100:])/100
    err = (max(tmp) - min(tmp))/rms
    if err > 0.01:
        time2 = i*timetick
    
print('\nstop\n')
print('sample time:{}, samples len:{}'.format(time4,len(x)))
print('1% settime:{:.2f}, phase 1% settime:{:.2f}'.format(time2,time3))

plt.plot(y)
plt.show()
plt.plot(p)
plt.show()

f = open('y.txt','w')
f.write(str(y))
f.close()
f = open('p.txt','w')
f.write(str(p))
f.close()
