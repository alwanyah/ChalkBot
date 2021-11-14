from gcodeparser import GcodeParser

import matplotlib.pyplot as plt
import numpy as np

# open gcode file and store contents as variable
with open('example.gcode', 'r') as f:
  gcode = f.read()

data = GcodeParser(gcode)

fig, ax = plt.subplots()

x = []
y = []
x0 = 0
y0 = 0
for c in data.lines:
    if 'Y' in c.params:
        c.params['Y'] = c.params['Y'] - 60
        #print(c.gcode_str)
        pass
    
    if c.command[0] == 'G' and c.command[1] == 1:
        #print(c)
        x0, y0 = c.params['X'], c.params['Y']
        x += [x0]
        y += [y0]
    
    if c.command[0] == 'G' and c.command[1] == 0 and len(x) > 0 and 'X' in c.params and 'Y' in c.params:
        ax.plot(x, y, '.')
        print(c)
        #ax.plot([x0,c.params['X']], [y0,c.params['Y']])
        ax.plot([c.params['X']], [c.params['Y']], '*')
        x = []
        y = []
       

plt.show()