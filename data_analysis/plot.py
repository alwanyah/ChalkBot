#first script to plot collected gms data

import numpy as np
from matplotlib import pyplot as plt


data = np.loadtxt("gps/test2.txt")
data1 = np.loadtxt("gps/test3.txt")
data2 = np.loadtxt("gps/test4.txt")

v = data[0,:] - data[-1,:]
print(np.hypot(v[0], v[1]))

plt.plot(data[:,1], data[:,0])
plt.plot(data1[:,1], data1[:,0])
plt.plot(data2[:,1], data2[:,0])

plt.gca().set_aspect('equal')
plt.show()