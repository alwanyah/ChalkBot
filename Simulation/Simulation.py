#!/usr/bin/env python3

import signal
import json
import tornado
from pantograph import PantographHandler, SimplePantographApplication, Image
from CommandServer import CommandServerThread
from Robot import Robot
import argparse
#import webbrowser

COMMAND_SERVER_PORT = 8000
PANTHOGRAPH_SERVER_PORT = 8080
PHYSICS_TIME_STEP = 6.0 # the update cycle of the physical simulation in ms

# This class represents the shape of the robot, moving on the simulation canvas.
# The metrics used by Robot.py are based on the real ChalkBot and the coordinates are in mm.
# For the simulation we choose a default metric of one pixel equals one cm,
# thus we transform the robot coordinates to the simulation coordinates by dividing with 10
# as well as dividing by our adjustable 'factor' parameter,
# which is to determine the scale of the simulation.
class SimulationHandler(PantographHandler):
    # pylint: disable=abstract-method # Method 'data_received' is abstract in class 'RequestHandler' but is not overridden

    def update(self):
        self.clear_rect(0, 0, self.width, self.height)
        
        # simulate
        # target time for the physical simulation in this rendering step
        t = self.application.timestamp + self.application.update_period
        while self.application.timestamp < t:
            self.application.robot.update(self.application.timestamp)
            self.application.timestamp += PHYSICS_TIME_STEP
            
            # printer is active
            if self.application.robot.p_pwm > 0:
                # draw with chalk in the background buffer
                self.draw(
                    "save",
                    x = self.application.robot.x / (10 * self.application.factor),
                    y = self.application.robot.y / (10 * self.application.factor),
                    radius=self.application.radius,
                    density = 10,
                    color="#000"
                )

        # calculate robots positon 
        self.application.shape.x = self.application.robot.x / (10 * self.application.factor) - self.application.offset[0]
        self.application.shape.y = self.application.robot.y / (10 * self.application.factor) - self.application.offset[1]
        self.application.shape.rotate(self.application.robot.theta)

        # render the scene
        self.draw("brush")
        self.application.shape.draw(self)

    def on_key_down(self, event):
        if event.key_code == 27: # ESC (TODO: find where key codes are defined)
            # don't create a new object here because the reference is shared with the command server
            self.application.robot.reset()
            # reset canvas
            self.draw("reset")


class Simulation(SimplePantographApplication):
    def __init__(self, factor, update_period):
        super().__init__(SimulationHandler)
        
        brush_radius = 10
        robot_width  = 40
        robot_length = 60
        
        # scale all sizes
        self.factor = factor
        self.radius = brush_radius / factor # radus for the brush
        self.offset = [robot_length/2/factor, robot_width/2/factor]  # don't move the paint, move the robot image
        self.shape  = Image("tutrobot.png", 0, 0, robot_length/factor, robot_width/factor)
        
        # the update time for the rendering is used internaly by Pantograph
        self.update_period = update_period
        self.timestamp = 0  # Simulationtime in Milliseconds
        self.robot = Robot()


def main():
    # read the arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--size", help="canvas size", type=float, default=1)
    parser.add_argument("-q", "--quick", help="enable the quick simulation mode.", action="store_true")
    args = parser.parse_args()

    # print status of used parameters
    print("Ctrl+C to close Server")
    print("Press ESC to reset the canvas.")
    print("----------------------------------")
    print("Set canvas size to {} (default 1)".format(args.size))
    print("Simulationmode set to '{}' (default 'normal')".format('quick' if args.quick else 'normal'))

    with open("config.json", "rb") as json_file:
        data = json.load(json_file)

    # time interval for the rendering
    update_period = data["timer_interval"]  # milliseconds
    if (args.quick):
        update_period = 1000.0 # render once in a second
    print("Update Period {}".format(update_period))


    app = Simulation(factor = args.size, update_period = update_period)
    server = CommandServerThread(app.robot, ("", COMMAND_SERVER_PORT), False)
    server.start()

    def shutdown_callback():
        print("\nClosing Server!")
        server.stop()
        tornado.ioloop.IOLoop.current().stop()
        
    def signal_handler(_sig, _frame):
        # don't intercept second ctrl+c if shutdown hangs
        signal.signal(signal.SIGINT, signal.SIG_DFL)

        tornado.ioloop.IOLoop.current().add_callback_from_signal(shutdown_callback)

    signal.signal(signal.SIGINT, signal_handler)

    # automatically open webbrowser
    #webbrowser.open('http://127.0.0.1:{}'.format(PANTHOGRAPH_SERVER_PORT), new=2)
    
    #print(app.constr_args)
    app.run(port=PANTHOGRAPH_SERVER_PORT)
    
    

if __name__ == "__main__":
    main()
