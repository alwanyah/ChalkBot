from __future__ import print_function
import sys
import pantograph
import random
import math
import itertools
from datetime import datetime
import CommandServer
#
#import threading
#import traceback
#import json



class Robot(object):
    def __init__(self):
        self.x = 0
        self.y = 0
        self.xvel = 0
        self.yvel = 0
        self.theta = 0
        self.rvel = 0
        self.print = 0
        #self.t = datetime.now()
        self.lastUpdate = 0
        self.lastCommand = "None"
        self.lastCommandTime = 0
        self.server = CommandServer()
    

    #driveRequest has following structure: [v_pwm, r_pwm, p_pwm, duration]
    def drive(self, driveRequest):
        self.xvel = (math.cos(self.theta) - math.sin(self.theta)) * driveRequest[0]
        self.yvel = (math.sin(self.theta) + math.cos(self.theta)) * driveRequest[0]
        self.rvel = driveRequest[1]
    
    def setVelocitySmooth(self, v, r):
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0

    def update(self, t):
        
        if (self.server.lastCommand.equals("drive") and ((t - self.server.timeOfLastCommand) > self.server.driveRequest[3])):
            self.drive(self.server.driveRequest)

        else:
            self.setVelocitySmooth(0, 0)
    
            
    







#if __name__ == '__main__':
#    robot = Robot()




    

