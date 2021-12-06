import sys
import pantograph
import random
import math
import itertools
from datetime import datetime
import CommandServer
import Robot



class ChalkBot(object):
    def __init__(self):
        self.shape = pantograph.Rect(120, 150, 20, 20, "#f00")
        self.theta = 0
        self.xvel = 0
        self.yvel = 0
        self.rvel = 0
        self.robot = Robot.Robot()

    def update(self, canvas):

        self.Robot.update(datetime.now().timestamp())
        self.xvel = self.Robot.xvel
        self.yvel = self.Robot.yvel
        self.rvel = self.Robot.rvel
        self.theta = self.Robot.theta

        rect = self.shape.get_bounding_rect()

        if rect.left <= 0 or rect.right >= canvas.width:
            self.xvel *= -1
        if rect.top <= 0 or rect.bottom >= canvas.height:
            self.yvel *= -1

        self.theta += self.rvel
        if self.theta > math.pi:
            self.theta -= 2 * math.pi

        self.Robot.theta = self.theta
        self.Robot.x = self.x
        self.Robot.y = self.y

        

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



    
