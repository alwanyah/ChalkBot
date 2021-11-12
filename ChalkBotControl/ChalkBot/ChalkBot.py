
import math

from .ChalkBotHTTPClient import ChalkBotHTTPClient

# a wrappen class for more advanced control
class ChalkBot(ChalkBotHTTPClient):
    def __init__(self, address):
        super().__init__(address)

        # we use constant velocity
        self.velocity = 64
        
        self.printPWM = 255
        
        # this is the internal global target angle
        self.targetAngle = 0
        
    # distance - target distance to drive in mm
    # pwm - target pwm to be used
    # return: needed time in ms
    def distanceToTime(self, distance, pwm): 
        wheelDiameter = 135.0 # mm  
        maxRPM = 313.0 # max RPM of the motors
        wheelUmfang = wheelDiameter*math.pi # mm
        maxVelocity = wheelUmfang * maxRPM / 60.0 # mm/s
        targetVelocity = maxVelocity * (pwm / 255.0)
        timeNeeded = distance / targetVelocity # in s
        
        # measured factor for the current chalkbot
        timeNeeded *= 4.0 / 3.0
        
        return timeNeeded*1000.0 # in ms
    
    # turn on the spot to a global angle
    def turnTo(self, angle):
        self.targetAngle = angle
        # todo: esimate the time needed for the turn
        # note: no printing while turning
        self.drive_imu_blocking_time(0, angle, 0, 3000) # turn
        
    # turn by a relative angle
    # NOTE: we don't need to unwrap here, the robot does it for us
    def turnBy(self, angle):
        self.turnTo(self.targetAngle + angle)
        
    def turnRight(self):
        self.turnBy(-math.pi/2.0)
        
    def turnLeft(self):
        self.turnBy(math.pi/2.0)

    # drive with a global angle no print
    def driveTo(self, angle, distance, doPrint = False):
        duration = self.distanceToTime(distance, self.velocity)
        #print(duration)
        
        if doPrint:
            self.drive_imu_blocking_time(self.velocity, angle, self.printPWM, duration)
        else:
            self.drive_imu_blocking_time(self.velocity, angle, 0            , duration)
    
    # drive in the current direction of the global angle
    def drive(self, distance, doPrint = False):
        print("drive: {}".format(distance))
        self.driveTo(self.targetAngle, distance, doPrint)


if __name__ == "__main__":
    robot = ChalkBot("10.0.4.99")

    # enable debug output on the server
    robot.debug(1)

    a0   = 0
    a90  = math.pi/2.0

    print("turnTo front")
    robot.turnTo(0)
    
    print("drive front 0.5m")
    robot.drive(500)
    
    print("turnBy right")
    robot.turnBy(a90)
    
    print("drive right 0.5m")
    robot.drive(500)
    
    print("turnBy back")
    robot.turnBy(a90)
    
    print("drive back 0.5m")
    robot.drive(500)
    
    print("turnBy left")
    robot.turnBy(a90)
    
    print("drive left 0.5m")
    robot.drive(500)
    
    print("turnTo front")
    robot.turnTo(a0)
    
    # disable debug on the server
    robot.server.debug(0)