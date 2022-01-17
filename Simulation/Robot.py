from __future__ import print_function
import sys
import pantograph
import random
import math
import itertools
from datetime import datetime
import CommandServer
import time
import numpy as np
#
#import threading
#import traceback
#import json

distance = 0

time_since_last_command = 0

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
        CommandServer.init()
    

    #driveRequest has following structure: [v_pwm, r_pwm, p_pwm, duration]
    def drive(self, driveRequest):
        self.xvel = math.cos(self.theta) * driveRequest[0] * 5/255
        self.yvel = math.sin(self.theta) * driveRequest[0] * 5/255
        self.rvel = driveRequest[1] /1024

    def goto(self):
        
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0

        if abs(CommandServer.angle) > math.pi/100:
            self.rvel = CommandServer.angle/abs(CommandServer.angle) /256
            CommandServer.angle = CommandServer.angle - self.rvel
        else:
            self.xvel = 5 * math.cos(self.theta)
            self.yvel = 5 * math.sin(self.theta)
            CommandServer.distance = CommandServer.distance - 50

    def setVelocitySmooth(self, v, r):
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0

    def update(self):

        global time_since_last_command
        t = datetime.timestamp(datetime.now()) * 1000
        time_since_last_command = t - CommandServer.timeOfLastCommand

        if (CommandServer.lastCommand=="drive" and (time_since_last_command < CommandServer.driveRequest[3])):
            self.drive(CommandServer.driveRequest)
            CommandServer.status_motion = "moving"

        elif (CommandServer.lastCommand=="goto" and CommandServer.distance > 0):
            
            self.goto()
            CommandServer.status_motion = "moving"
        else:
            self.setVelocitySmooth(0,0)
            CommandServer.status_motion = "stopped"

"""
if __name__ == '__main__':
    robot = Robot()
    print(CommandServer.timeOfLastCommand)
    print(CommandServer.lastCommand)
    time.sleep(10)
    print(CommandServer.timeOfLastCommand)
    print(CommandServer.lastCommand)
    time.sleep(10)
    print(CommandServer.timeOfLastCommand)
    print(CommandServer.lastCommand)
"""


    

