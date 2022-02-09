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
        self.velocity = 0
        self.theta = 0
        self.rvel = 0
        self.t = 0

        self.lastCommand = "None"
        self.lastCommandTime = 0
        self.updatePeriod = 10 # milliseconds

        self.printing = False



    def setVelocitySmooth(self, v, r):
        self.velocity = 0
        self.rvel = 0

    def update(self):

        global time_since_last_command
        self.t += self.updatePeriod
        time_since_last_command = self.t - CommandServer.timeOfLastCommand

        if (CommandServer.lastCommand=="drive" and (time_since_last_command < CommandServer.driveRequest[3])):

            self.velocity = CommandServer.driveRequest[0]
            self.rvel = CommandServer.driveRequest[1] / (255 * 64)
            self.printing = CommandServer.driveRequest[2] > 0
            CommandServer.status_motion = "moving"

        elif (CommandServer.lastCommand=="goto"):

            xrel = (CommandServer.goto_point[0]-self.x)*math.cos(self.theta) + (CommandServer.goto_point[1]-self.y)*math.sin(self.theta)
            yrel = -(CommandServer.goto_point[0]-self.x)*math.sin(self.theta) + (CommandServer.goto_point[1]-self.y)*math.cos(self.theta)

            distance = math.sqrt(xrel*xrel + yrel*yrel)
            angle = -np.arctan2(yrel, xrel)
            self.printing = CommandServer.goto_point[2] > 0
            
            maxDistanceError = self.updatePeriod * 2
            maxAngleError = self.updatePeriod/1000

            if distance > maxDistanceError:                 # 

                if abs(angle) > maxAngleError:
                    self.velocity = 0
                    self.rvel = angle/abs(angle) / 64

                else:
                    self.velocity = 255   #  255 is the max pwm
                    self.rvel = 0
                    if distance/maxDistanceError < 2:
                        self.velocity = self.velocity /2
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

        # We assume a max velocity of the original ChalkBot of 5 m/s = 5 mm/ms
        # To get the Velocity, we also normalize the pwm.
        
        xvel = math.cos(self.theta) * self.velocity
        yvel = math.sin(self.theta) * self.velocity

        self.x += 5 * self.updatePeriod * xvel / 255
        self.y += 5 * self.updatePeriod * yvel / 255

        self.theta -= self.rvel * self.updatePeriod/10 # this Translation could be more accurate
        if self.theta > math.pi:
            self.theta -= 2 * math.pi

        CommandServer.pose = [self.x, self.y]
        CommandServer.orientation = self.theta
        CommandServer.t = self.t



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




