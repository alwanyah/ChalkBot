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
from collections import namedtuple
import json


robot = Robot.Robot()

factor = 1

dots = []
radius = 10/factor
offset = [30/factor, 20/factor]             # dont move the paint, move the robot image

reset = False

quick = False

t = 0             # Simulationtime in Milliseconds

updatePeriod = 30
#print(updatePeriod)

robot_ticks = 5   # amount of updates the robot will clalculate per frame

def signal_handler(sig, frame):
    if (quick):
        with open("config.json", "r") as jsonFile:
            data = json.load(jsonFile)

        data["timer_interval"] = updatePeriod

        with open("config.json", "w") as jsonFile:
            json.dump(data, jsonFile)

    CommandServer.close()
    print("\nClosing Server!")
    sys.exit()

signal.signal(signal.SIGINT, signal_handler)

# This class represents the shape of the robot, moving on the simulation canvas.
# The metrics used by Robot.py are based on the real ChalkBot and the coordinates are in mm.
# For the simulation we choose a default metric of one pixel equals one cm,
# thus we transform the robot coordinates to the simulation coordinates by dividing with 10
# as well as dividing by our adjustable 'factor' parameter,
# which is to determine the scale of the simulation.
class ChalkBot(object):
    def __init__(self):
        self.shape = pantograph.Image("tutrobot.png",robot.x/(10*factor), robot.y/(10*factor), 60/factor, 40/factor)
        self.theta = robot.theta

    def update(self, canvas):

        global t, dots, reset

        if (reset):
            canvas.draw("reset")
            reset = False

        for i in range(robot_ticks):
            robot.update(t)
            t += updatePeriod/robot_ticks
            if robot.p_pwm:
                canvas.draw("save", x=robot.x/(10*factor), y=robot.y/(10*factor), radius=radius, lineColor="#f00")
            CommandServer.t = t
        
        #for dot in dots:
            #pantograph.Circle(dot[0]/10*factor, dot[1]/10*factor, radius, "#f00").draw(canvas)
            #canvas.fill_circle(dot[0], dot[1], radius, color="#f00")
        #   canvas.draw("brush", x=dot[0]/10*factor, y=dot[1]/10*factor, radius=radius, lineColor="#f00")

        self.shape.x = robot.x/(10*factor) - offset[0]
        self.shape.y = robot.y/(10*factor) - offset[1]

        self.shape.rotate(robot.theta)

        if (not quick or robot.status_motion=="stopped"):
            canvas.draw("brush")
            self.shape.draw(canvas)

class ChalkBotSimulation(pantograph.PantographHandler):
    def setup(self):
        self.xvel = 0
        self.yvel = 0
        self.chalkbot = ChalkBot()

    def update(self):
        self.clear_rect(0, 0, self.width, self.height)
        self.chalkbot.update(self)

    def on_key_down(self, event):
        global dots, robot, t, reset
        if event.key_code == 27:
            reset = True
            t = 0
            dots = []
            robot = Robot.Robot()
            CommandServer.reset()



def main():
    global factor, radius, offset, updatePeriod
    print("Ctrl+C to close Server")
    print("Press ESC to reset canvas and chalkbot.")
    print("----------------------------------")
    factor = float(input("Set canvas size (default 1): "))
    quick = float(input("Simulationmode normal : 0          quickmode : 1\n"))==1
    radius = 10/factor
    offset = [30/factor, 20/factor]


    with open("config.json", "r") as jsonFile:
        data = json.load(jsonFile)

        updatePeriod = data["timer_interval"] # milliseconds

        if (quick):
            data["timer_interval"] = 1.0

            with open("config.json", "w") as jsonFile:
                json.dump(data, jsonFile)




    CommandServer.init()
    print("==================================")
    app = pantograph.SimplePantographApplication(ChalkBotSimulation)
    #print(app.constr_args)
    app.run()
    


if __name__ == '__main__':
    main()



