import sys
import pantograph
import random
import math
import itertools
from datetime import datetime
import CommandServer
import Robot

robot = Robot.Robot()

class ChalkBot(object):
    def __init__(self):
        self.shape = pantograph.Image("tutrobot.png",100, 100, 150, 120)
        self.theta = 0
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0

    def update(self, canvas):

        robot.update()
        self.xvel = robot.xvel
        self.yvel = robot.yvel
        self.rvel = robot.rvel
        self.theta = robot.theta

        rect = self.shape.get_bounding_rect()

        if rect.left <= 0 or rect.right >= canvas.width:
            self.xvel *= -1
        if rect.top <= 0 or rect.bottom >= canvas.height:
            self.yvel *= -1

        self.theta += self.rvel
        if self.theta > math.pi:
            self.theta -= 2 * math.pi

        robot.theta = self.theta
        

        

        self.shape.translate(self.xvel, self.yvel)
        self.shape.rotate(self.theta)
        self.shape.draw(canvas)


class ChalkBotSimulation(pantograph.PantographHandler):
    def setup(self):
        self.xvel = 0
        self.yvel = 0
        self.chalkbot = ChalkBot()


    def update(self):
        self.clear_rect(0, 0, self.width, self.height)
        self.chalkbot.update(self)

if __name__ == '__main__':
    app = pantograph.SimplePantographApplication(ChalkBotSimulation)
    app.run()



    
