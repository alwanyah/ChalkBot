import math
import numpy as np


# Multipliers: multiply with the corresponding pwm to calculate the movement.

V_MAX = 5   # m/second, which equals 18km/h
V_FACTOR = V_MAX / 255

R_MAX = math.pi # radians per Second
R_FACTOR = (1 / 255) * (R_MAX/1000) # max rotational velocity of 180° per Second


class Robot:
    ACTION_QUEUE_CAPACITY = 5

    def __init__(self):
        # FIXME!!
        # in the real chalkbot x is north, y is west,
        # heading is counterclockwise and 0 means facing north
        self.x = 1000 # mm east
        self.y = 1000 # mm south
        self.theta = 0 # rad clockwise, 0 = facing east
        self.timestamp = 0 # ms

        self.r_pwm = 0 # 0 to 255, rotation speed (counterclockwise)
        self.v_pwm = 0 # 0 to 255, forward (vorward?) speed
        self.p_pwm = 0 # 0 to 255, print speed

        self.last_command = None
        self.time_of_last_command = self.timestamp

        self.drive_request = [0,0,0,0] # v, r, p, t
        self.status_motion = "stopped"
        self.goto_point = [0,0,0] # x, y, p

        self.action_queue = []
        self.is_executing_action = False
        self.action_start_timestamp = 0

    def reset(self):
        self.__init__()

    def replace_actions(self, queue):
        assert len(queue) <= self.ACTION_QUEUE_CAPACITY

        self.is_executing_action = False
        self.action_queue = queue

    def append_actions(self, queue):
        assert len(queue) <= self.ACTION_QUEUE_CAPACITY

        count = min(len(queue), self.ACTION_QUEUE_CAPACITY - len(self.action_queue))
        self.action_queue += queue[:count]
        return count

    def set_velocity_smooth(self, v, r):
        self.v_pwm = v
        self.r_pwm = r

    def update(self, timestamp):
        time_delta = timestamp - self.timestamp
        self.timestamp = timestamp

        if self.is_executing_action:
            self.behavior(time_delta)
        elif any(self.action_queue):
            self.is_executing_action = True
            self.action_start_timestamp = self.timestamp
            self.behavior(time_delta)
        else:
            self.legacy_behavior(time_delta)

        self.move(time_delta)

    def behavior(self, time_delta):
        action_type = self.action_queue[0]["type"]
        action_properties = self.action_queue[0]["properties"]
        if action_type == "drive":
            self.action_drive(time_delta, action_properties)
        elif action_type == "goto":
            self.action_goto(time_delta, action_properties)
        else:
            print("skipping unknown action: " + action_type)
            self.next_action()

    def next_action(self):
        self.set_velocity_smooth(0, 0)
        self.p_pwm = 0
        self.status_motion = "stopped"
        self.is_executing_action = False
        self.action_queue.pop(0)

    def action_drive(self, _time_delta, properties):
        action_duration = self.timestamp - self.action_start_timestamp
        target_duration = properties["duration"] * 1000
        if action_duration >= target_duration:
            self.next_action()
        else:
            self.v_pwm = properties["forward"] * 255
            self.r_pwm = properties["clockwise"] * -255
            self.p_pwm = properties["print"] * 255
            self.status_motion = "moving"

    def action_goto(self, time_delta, properties):
        target_x = properties["east"] * 1000
        target_y = properties["north"] * -1000
        target_print = properties["print"] * 255

        xrel = (target_x - self.x) * math.cos(self.theta) + (target_y - self.y) * math.sin(self.theta)
        yrel = -(target_x - self.x) * math.sin(self.theta) + (target_y - self.y) * math.cos(self.theta)

        distance = math.sqrt(xrel * xrel + yrel * yrel)
        angle = -np.arctan2(yrel, xrel)

        max_distance_error = time_delta * 2
        max_angle_error = time_delta / 1000

        if distance > max_distance_error:
            if abs(angle) > max_angle_error:
                self.v_pwm = 0
                self.r_pwm = angle / abs(angle) * 255
                if angle / max_angle_error < 2:
                    self.r_pwm = self.r_pwm / 2

            else:
                self.v_pwm = 255   #  255 is the max pwm
                self.r_pwm = 0
                if distance / max_distance_error < 2:
                    self.v_pwm = self.v_pwm / 2

            self.p_pwm = target_print
            self.status_motion = "moving"

        else:
            self.next_action()

    def legacy_behavior(self, time_delta):
        time_since_last_command = self.timestamp - self.time_of_last_command

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

    def move(self, time_delta):
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
