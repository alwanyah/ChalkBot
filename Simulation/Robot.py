import math
import numpy as np

from math2d import * 



# Multipliers: multiply with the corresponding pwm to calculate the movement.

V_MAX = 5   # m/second, which equals 18km/h
V_FACTOR = V_MAX / 255

R_MAX = math.pi # radians per Second
R_FACTOR = (1.0 / 255.0) * (R_MAX) # max rotational velocity of 180° per Second


# parameters from the firmware for future
wheelDiameter = 135.0 # mm
maxRPM = 313.0 # max RPM of the motors with PWM 255
wheelCircumference = wheelDiameter * math.pi
maxVelocity = wheelCircumference * maxRPM / 60.0 # mm/s

# measured factor for the current chalkbot
correctionFactor = 4.0 / 3.0;


class Robot:
    def __init__(self):
    
        # this is the true position of the robot in the world, not known to the controller
        self.x = 1000 # mm north
        self.y = 1000 # mm west
        self.theta = 0 # rad counterclockwise
        self.t = 0 # ms timestamp

        self.r_pwm = 0 # 0 to 255, rotation speed (counterclockwise)
        self.v_pwm = 0 # 0 to 255, forward (vorward?) speed
        self.p_pwm = 0 # 0 to 255, print speed

        
        # CONTROLLER (TODO: separate file)
        
        # estimated pose of the robot (this is what robot knows)
        self.robotPose = Pose2D()

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
        
    def set_print_speed(self, p):
        self.p_pwm = p
        
    def update_robot(self, time_delta):
        # We assume a max velocity of the original ChalkBot of 5 m/s = 5 mm/ms
        # To get the Velocity, we also normalize the pwm.
        
        # calcuate the distance moved by the robot based on the PWM command
        velocity = maxVelocity * (self.v_pwm / 255.0)
        distance = time_delta * velocity
        #distance /= correctionFactor

        self.x += math.cos(self.theta) * distance
        self.y += math.sin(self.theta) * distance

        # calculate rotation
        rvel = self.r_pwm * R_FACTOR

        self.theta += rvel * time_delta
        if self.theta > math.pi:
            self.theta -= 2 * math.pi
            
    def timeToDistance(self, timeDelta, pwm):
        targetVelocity = maxVelocity * (pwm / 255.0)
        distance = timeDelta * targetVelocity
        distance /= correctionFactor
        return distance

    def update(self, t):
        time_delta = t - self.t
        self.t = t
        time_since_last_command = self.t - self.time_of_last_command
        
        # calculate odometry
        self.robotPose.rotation = self.theta # IMU
        distanceMoved = self.timeToDistance(self.v_pwm, time_delta / 1000.0)
        self.robotPose.translate(distanceMoved, 0);


        if self.last_command == "drive" and time_since_last_command < self.drive_request[3]:
            self.v_pwm = self.drive_request[0]
            self.r_pwm = self.drive_request[1]
            self.p_pwm = self.drive_request[2]
            self.status_motion = "moving"

        elif self.last_command == "goto":
        
            p_pwm = self.goto_point[2]
        
            target = Vector2(self.goto_point[0], self.goto_point[1])
            targetPointLocal = self.robotPose / target;
            
            #xrel =  (self.goto_point[0]-self.x)*math.cos(self.theta) + (self.goto_point[1]-self.y)*math.sin(self.theta)
            #yrel = -(self.goto_point[0]-self.x)*math.sin(self.theta) + (self.goto_point[1]-self.y)*math.cos(self.theta)

            # TODO: should be config
            max_rotation_pwm = 64.0
            max_distance_error = 30
            
            distance = clamp(targetPointLocal.abs(), -max_rotation_pwm, max_rotation_pwm);

            #distance = math.sqrt(xrel*xrel + yrel*yrel)
            #angle = -np.arctan2(yrel, xrel)
            #
            #max_distance_error = time_delta*2
            #max_angle_error = time_delta/1000

            if abs(distance) > max_distance_error:
            
                angle = targetPointLocal.angle() * 1024.0
                angle = clamp(angle, -max_rotation_pwm, max_rotation_pwm);
            
                if abs(angle) < max_rotation_pwm:
                    self.set_velocity_smooth(distance, angle)
                    self.set_print_speed(p_pwm)
                    print("drive")
                else:
                    self.set_velocity_smooth(0, angle)
                    self.set_print_speed(0) # don't print when turning
                    print("turn {}".format(targetPointLocal.angle() * 1024.0))

                self.status_motion = "moving"

            else:
                self.set_print_speed(0)
                self.set_velocity_smooth(0,0)
                self.status_motion = "stopped"

        else:
            self.set_print_speed(0)
            self.set_velocity_smooth(0,0)
            self.status_motion = "stopped"
            
        

        self.update_robot(time_delta / 1000.0)