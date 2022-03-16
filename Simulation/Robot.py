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

#
# Multipliers: multiply with the corresponding pwm to calculate the movement.

v_max = 5   # m/second, which equals 18km/h
v_factor = v_max / 255

r_max = math.pi # radians per Second
r_factor = (1 / 255) * (r_max/1000) # max rotational velosity of 180° per Second

class Robot(object):
    def __init__(self):
        self.x = 1000
        self.y = 1000
        self.theta = 0
        self.t = 0

        self.r_pwm = 0
        self.v_pwm = 0
        self.p_pwm = 0



    def setVelocitySmooth(self, v, r):
        self.v_pwm = 0
        self.r_pwm = 0

    def update(self, t):

        timeDelta = t - self.t
        self.t = t
        time_since_last_command = self.t - CommandServer.timeOfLastCommand

        if (CommandServer.lastCommand=="drive" and (time_since_last_command < CommandServer.driveRequest[3])):

            self.v_pwm = CommandServer.driveRequest[0]
            self.r_pwm = CommandServer.driveRequest[1]
            self.p_pwm = CommandServer.driveRequest[2] > 0
            CommandServer.status_motion = "moving"

        elif (CommandServer.lastCommand=="goto"):

            xrel = (CommandServer.goto_point[0]-self.x)*math.cos(self.theta) + (CommandServer.goto_point[1]-self.y)*math.sin(self.theta)
            yrel = -(CommandServer.goto_point[0]-self.x)*math.sin(self.theta) + (CommandServer.goto_point[1]-self.y)*math.cos(self.theta)

            distance = math.sqrt(xrel*xrel + yrel*yrel)
            angle = -np.arctan2(yrel, xrel)
            self.p_pwm = CommandServer.goto_point[2]

            maxDistanceError = timeDelta*2
            maxAngleError = timeDelta/1000

            if distance > maxDistanceError:                 #

                if abs(angle) > maxAngleError:
                    self.v_pwm = 0
                    self.r_pwm = angle/abs(angle) * 255
                    if angle/maxAngleError < 2:
                        self.r_pwm = self.r_pwm/2

                else:
                    self.v_pwm = 255   #  255 is the max pwm
                    self.r_pwm = 0
                    if distance/maxDistanceError < 2:
                        self.v_pwm = self.v_pwm /2
                CommandServer.status_motion = "moving"


            else:
                self.p_pwm = 0
                self.setVelocitySmooth(0,0)
                CommandServer.status_motion = "stopped"

        else:
            self.p_pwm = 0
            self.setVelocitySmooth(0,0)
            CommandServer.status_motion = "stopped"

        #behavior

        # We assume a max velocity of the original ChalkBot of 5 m/s = 5 mm/ms
        # To get the Velocity, we also normalize the pwm.

        velocity = self.v_pwm * v_factor

        xvel = math.cos(self.theta) * velocity
        yvel = math.sin(self.theta) * velocity

        self.x += timeDelta * xvel
        self.y += timeDelta * yvel

        rvel = self.r_pwm * r_factor

        self.theta -= rvel * timeDelta
        if self.theta > math.pi:
            self.theta -= 2 * math.pi

        CommandServer.pose = [self.x, self.y]
        CommandServer.orientation = self.theta
        #CommandServer.t = self.t



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




