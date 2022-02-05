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

time_since_last_command = 0

class Robot(object):
    def __init__(self):
        self.x = 100
        self.y = 100
        self.xvel = 0
        self.yvel = 0
        self.theta = 0
        self.rvel = 0
        #self.t = datetime.now()
        self.lastUpdate = 0
        self.lastCommand = "None"
        self.lastCommandTime = 0
        self.printing = False
        CommandServer.init()
    

    def setVelocitySmooth(self, v, r):
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0

    def update(self):

        global time_since_last_command
        t = datetime.timestamp(datetime.now()) * 1000
        time_since_last_command = t - CommandServer.timeOfLastCommand

        if (CommandServer.lastCommand=="drive" and (time_since_last_command < CommandServer.driveRequest[3])):
            
            self.xvel = math.cos(self.theta) * CommandServer.driveRequest[0] * 50/255
            self.yvel = math.sin(self.theta) * CommandServer.driveRequest[0] * 50/255
            self.rvel = CommandServer.driveRequest[1] / (255 * 64)
            self.printing = CommandServer.driveRequest[2] > 0
            CommandServer.status_motion = "moving"

        elif (CommandServer.lastCommand=="goto"):

            xrel = (CommandServer.goto_point[0]-self.x)*math.cos(self.theta) + (CommandServer.goto_point[1]-self.y)*math.sin(self.theta)
            yrel = -(CommandServer.goto_point[0]-self.x)*math.sin(self.theta) + (CommandServer.goto_point[1]-self.y)*math.cos(self.theta)

            distance = math.sqrt(xrel*xrel + yrel*yrel)
            angle = -np.arctan2(yrel, xrel)
            self.printing = CommandServer.goto_point[2] > 0

            maxDistanceError = 30 # mm
            maxAngleError = 1/100 # Radians - maximum Error

            if distance > maxDistanceError:

                if abs(angle) > maxAngleError:
                    self.xvel = 0
                    self.yvel = 0
                    self.rvel = angle/abs(angle) / 64

                else:
                    self.xvel = math.cos(self.theta) * 50
                    self.yvel = math.sin(self.theta) * 50
                    self.rvel = 0
                CommandServer.status_motion = "moving"
            else:
                self.printing = False
                self.setVelocitySmooth(0,0)
                CommandServer.status_motion = "stopped"

        else:
            self.printing = False
            self.setVelocitySmooth(0,0)
            CommandServer.status_motion = "stopped"

        #behavior
        self.x += self.xvel
        self.y += self.yvel
        
        self.theta -= self.rvel
        if self.theta > math.pi:
            self.theta -= 2 * math.pi

        CommandServer.pose = [self.x, self.y]
        CommandServer.orientation = self.theta



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


    

