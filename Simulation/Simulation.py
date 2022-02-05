import sys
import pantograph
import random
import math
import itertools
from datetime import datetime
import CommandServer
import Robot
import gc
import signal

robot = Robot.Robot()

factor = 1

dots = []

radius = 10/factor

def signal_handler(sig, frame):
    CommandServer.close()
    print("\nClosing Server!")
    sys.exit()

signal.signal(signal.SIGINT, signal_handler)

class ChalkBot(object):
    def __init__(self):
        self.shape = pantograph.Image("tutrobot.png",robot.x/(10*factor), robot.y/(10*factor), 60/factor, 40/factor)
        self.theta = robot.theta
        #self.xvel = robot.xvel
        #self.yvel = robot.yvel
        #self.rvel = robot.rvel
        #self.ink = 10

    def update(self, canvas): # ca 1 update alle 12 ms aber wir gehen von 10 ms aus, also 100 pro sekunde.

        robot.update()
       
        self.shape.x = robot.x/(10*factor)
        self.shape.y = robot.y/(10*factor)

        self.shape.rotate(robot.theta)

        self.shape.draw(canvas)


class ChalkBotSimulation(pantograph.PantographHandler):
    def setup(self):
        self.xvel = 0
        self.yvel = 0
        self.chalkbot = ChalkBot()




    def update(self):
        self.clear_rect(0, 0, self.width, self.height)

        if robot.printing:
            dots.append([self.chalkbot.shape.x, self.chalkbot.shape.y])

        for dot in dots:
            pantograph.Circle(dot[0], dot[1], radius, "#f00").draw(self)

        self.chalkbot.update(self)

def main():
    print("Ctrl+C to close Server")
    app = pantograph.SimplePantographApplication(ChalkBotSimulation)
    app.run()


if __name__ == '__main__':

    factor = float(input("Size of Simulation canvas: "))
    main()


    
