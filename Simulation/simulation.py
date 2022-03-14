#!/usr/bin/env python3

import signal
import json
import tornado
from pantograph import PantographHandler, SimplePantographApplication, Image
from command_server import CommandServerThread
from robot import Robot


COMMAND_SERVER_PORT = 8000
PANTHOGRAPH_SERVER_PORT = 8080
ROBOT_TICKS = 5  # amount of updates the robot will clalculate per frame


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

        for _ in range(ROBOT_TICKS):
            self.application.robot.update(self.application.timestamp)
            self.application.timestamp += self.application.update_period / ROBOT_TICKS
            if self.application.robot.p_pwm > 0:
                self.application.dots.append([self.application.robot.x, self.application.robot.y])

        for dot in self.application.dots:
            #pantograph.Circle(dot[0]/10*factor, dot[1]/10*factor, self.application.radius, "#f00").draw(self)
            #self.fill_circle(dot[0], dot[1], self.application.radius, color="#f00")
            self.draw(
                "brush",
                x=dot[0]/10*self.application.factor,
                y=dot[1]/10*self.application.factor,
                radius=self.application.radius,
                lineColor="#f00",
            )

        self.application.shape.x = self.application.robot.x / (10 * self.application.factor) - self.application.offset[0]
        self.application.shape.y = self.application.robot.y / (10 * self.application.factor) - self.application.offset[1]

        self.application.shape.rotate(self.application.robot.theta)

        self.application.shape.draw(self)

    def on_key_down(self, event):
        if event.key_code == 27: # ESC (TODO: find where key codes are defined)
            self.application.t = 0
            self.application.dots = []

            # don't create a new object here because the reference is shared with the command server
            self.application.robot.reset()


class Simulation(SimplePantographApplication):
    def __init__(self, factor, update_period):
        super().__init__(SimulationHandler)
        self.factor = factor
        self.update_period = update_period
        self.shape = Image("tutrobot.png", 0, 0, 60/factor, 40/factor)
        self.dots = []
        self.radius = 10 / factor
        self.offset = [30/factor, 20/factor]  # don't move the paint, move the robot image
        self.timestamp = 0  # Simulationtime in Milliseconds
        self.robot = Robot()


def main():
    factor = float(input("Size of Simulation canvas: "))

    print("Ctrl+C to close Server")
    print("Press ESC to reset the canvas.")

    with open("config.json", "rb") as json_file:
        data = json.load(json_file)

    update_period = data["timer_interval"]  # milliseconds
    #print(updatePeriod)

    app = Simulation(factor, update_period)
    server = CommandServerThread(app.robot, ("", COMMAND_SERVER_PORT))
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

    #print(app.constr_args)
    app.run(port=PANTHOGRAPH_SERVER_PORT)


if __name__ == "__main__":
    main()
