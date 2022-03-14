import math
import numpy as np


# Multipliers: multiply with the corresponding pwm to calculate the movement.

V_MAX = 5   # m/second, which equals 18km/h
V_FACTOR = V_MAX / 255

R_MAX = math.pi # radians per Second
R_FACTOR = (1 / 255) * (R_MAX/1000) # max rotational velocity of 180° per Second


class Robot:
    def __init__(self):
        self.x = 1000 # mm north
        self.y = 1000 # mm west
        self.theta = 0 # rad counterclockwise
        self.t = 0 # ms timestamp

        self.r_pwm = 0 # 0 to 255, rotation speed (counterclockwise)
        self.v_pwm = 0 # 0 to 255, forward (vorward?) speed
        self.p_pwm = 0 # 0 to 255, print speed

        self.last_command = None
        self.time_of_last_command = self.t

        self.drive_request = [0,0,0,0] # v, r, p, t
        self.status_motion = "stopped"
        self.goto_point = [0,0,0] # x, y, p

    def reset(self):
        self.__init__()

    # def reset(self):
    #     self.last_command = None
    #     self.t = 0
    #     self.time_of_last_command = self.t

    #     self.drive_request = [0,0,0,0]
    #     self.status_motion = "stopped"
    #     self.goto_point = [0,0,0]

    #     self.pose = [0,0]
    #     self.orientation = 0

    def set_velocity_smooth(self, v, r):
        self.v_pwm = v
        self.r_pwm = r

    def update(self, t):
        time_delta = t - self.t
        self.t = t
        time_since_last_command = self.t - self.time_of_last_command

        if self.last_command == "drive" and time_since_last_command < self.drive_request[3]:
            self.v_pwm = self.drive_request[0]
            self.r_pwm = self.drive_request[1]
            self.p_pwm = self.drive_request[2]
            self.status_motion = "moving"

        elif self.last_command == "goto":
            xrel = (self.goto_point[0]-self.x)*math.cos(self.theta) + (self.goto_point[1]-self.y)*math.sin(self.theta)
            yrel = -(self.goto_point[0]-self.x)*math.sin(self.theta) + (self.goto_point[1]-self.y)*math.cos(self.theta)

            distance = math.sqrt(xrel*xrel + yrel*yrel)
            angle = -np.arctan2(yrel, xrel)
            self.p_pwm = self.goto_point[2]

            max_distance_error = time_delta*2
            max_angle_error = time_delta/1000

            if distance > max_distance_error:
                if abs(angle) > max_angle_error:
                    self.v_pwm = 0
                    self.r_pwm = angle/abs(angle) * 255
                    if angle/max_angle_error < 2:
                        self.r_pwm = self.r_pwm/2

                else:
                    self.v_pwm = 255   #  255 is the max pwm
                    self.r_pwm = 0
                    if distance/max_distance_error < 2:
                        self.v_pwm = self.v_pwm /2

                self.status_motion = "moving"

            else:
                self.p_pwm = 0
                self.set_velocity_smooth(0,0)
                self.status_motion = "stopped"

        else:
            self.p_pwm = 0
            self.set_velocity_smooth(0,0)
            self.status_motion = "stopped"

        #behavior

        # We assume a max velocity of the original ChalkBot of 5 m/s = 5 mm/ms
        # To get the Velocity, we also normalize the pwm.

        velocity = self.v_pwm * V_FACTOR

        xvel = math.cos(self.theta) * velocity
        yvel = math.sin(self.theta) * velocity

        self.x += time_delta * xvel
        self.y += time_delta * yvel

        rvel = self.r_pwm * R_FACTOR

        self.theta -= rvel * time_delta
        if self.theta > math.pi:
            self.theta -= 2 * math.pi
