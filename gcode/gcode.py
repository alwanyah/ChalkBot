import re

class gcode():
    def readCode(file):
        # gcode = file.readlines()
        gdict = {}
        i = 0
        for line in file:
            line = line.strip()
            
            gc = re.findall(r'[G].?\d+', line)
            fc = re.findall(r'[F].?\d+', line)
            coord = re.findall(r'[XY].?\d+.\d+', line)
            
            if coord or gc or fc:
                gdict[i] = {'gcode': gc,'fcode': fc, 'coord':coord} 
                i+=1
                # print("{} - {}".format(coord[0], coord[1]))
                # print("{}".format(gc[0]))
                # print("{}".format(fc[0]))
                
        return gdict

if __name__ == "__main__":
    file = open('example.gcode','r').readlines()
    output = gcode.readCode(file)
    print(output)