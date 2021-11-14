from gcodeparser import GcodeParser

# open gcode file and store contents as variable
with open('example.gcode', 'r') as f:
  gcode = f.read()

data = GcodeParser(gcode)

for c in data.lines:
    
    if 'Y' in c.params:
        c.params['Y'] = c.params['Y'] - 60
        print(c.gcode_str)
        
with open('output.gcode', 'w') as f:
  for c in data.lines:
    f.write(c.gcode_str)
    f.write('\n')

    