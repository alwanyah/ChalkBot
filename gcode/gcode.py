# from ..ChalkBot import *
import re

class gcode():
    def readCode(file):
        '''
        parsing gcode file and adding parameters to a dict

        input: gcode file
        output: dictionary
        
        dictionary keys:
            gcode,
            fcode,
            coordinates
            
        '''
        gdict = {}
        i = 0
        for line in file:
            line = line.strip()
            
            # parsing coordinates, gcode and fcode 
            # todo: adding curve
            gc = re.findall(r'[G].?\d+', line)
            fc = re.findall(r'[F].?\d+', line)
            coord = re.findall(r'[XY].?\d+.\d+', line)
            
            # adding data to dict
            if coord or gc or fc:
                gdict[i] = {'gcode': gc,'fcode': fc, 'coord':coord} 
                i+=1
                # print("{} - {}".format(coord[0], coord[1]))
                # print("{}".format(gc[0]))
                # print("{}".format(fc[0]))
                
        return gdict
    
    def printCode(dict):
        '''
        input: gcode dictionary
        output: chalkbot commands
        '''
        # if botClient.orientation == 0 and dict[0]['coord'][0]
        
        return

if __name__ == "__main__":
    file = open('example.gcode','r').readlines()
    output = gcode.readCode(file)
    print(output[0]['coord'][0])