
import requests
import time

class ChalkBotHTTPClient:
    def __init__(self, address):
        self.server_url = "http://{}".format(address)
        self.verbose = False

    def send(self, command, data):
        url = "{}/{}".format(self.server_url, command)
        
        if self.verbose:
            print("POST REQUEST: ")
            print("  URL: {}".format(url))
            print("  DATA: {}".format(data))
            
        r = requests.post(url, data = data)
        
        if self.verbose:
            print("  RESPONSE: {};{};{}".format(r.status_code, r.reason, r.text) )
        
        return r.text

    def debug(self, code):
        return self.send("debug", "{}".format(code))

    def status(self):
        return self.send("status_motion", "blub")
        
    def status_imu(self):
        return self.send("status_imu", "blub")
        
    def orientation(self):
        return float(self.send("orientation", "blub"))
        
    def pose(self):
        msg = self.send("pose", "blub")
        return [float(v) for v in msg.split(";")]

    def motor(self, a1, a2, a3, a4, a5):
        return self.send("motor", "{};{};{};{};{}".format(a1, a2, a3, a4, a5))    
    
    # open loop driving
    def drive(self, translation_pwm, rotation_pwm, p_pwm, duration):
        '''
        Simple drive without sensor feedback (open loop). 
        
        #Parameters:
        #   translation_pwm
        #   rotation_pwm
        #   p_pwm
        #   duration
        '''
        return self.send("drive", "{};{};{};{}".format(translation_pwm, rotation_pwm, p_pwm, duration))
    
    # blocking version of drive
    def drive_blocking(self, translation_pwm, rotation_pwm, p_pwm, duration):
        self.drive(translation_pwm, rotation_pwm, p_pwm, duration)
        
        while self.status() != "stopped":
            print("drive ...")
            time.sleep(1)
    
    
    # closed loop driving
    def drive_imu(self, translation_pwm, angle, p_pwm, duration):
        return self.send("drive_imu", "{};{};{};{}".format(translation_pwm, angle, p_pwm, duration))
        
    # blocking version of drive_imu
    def drive_imu_blocking_time(self, translation_pwm, angle, p_pwm, duration):
        self.drive_imu(translation_pwm, angle, p_pwm, duration)
        
        while self.status() != "stopped":
            print("drive_imu ...")
            time.sleep(1)
            
    
    # odometry based driving
    def goto(self, x, y, p_pwm):
        return self.send("goto_point", "{};{};{}".format(x, y, p_pwm))
        
    # blocking version of goto
    def goto_blocking(self, x, y, p_pwm):
        self.goto(x, y, p_pwm)
        
        while self.status() != "stopped":
            print("drive_imu ...")
            time.sleep(1)

if __name__ == "__main__":
    chalkbot = ChalkBotHTTPClient("10.0.4.99")
    
    # for debugging
    chalkbot.verbose = True
    
    # check status to test the basic connection
    chalkbot.status()
    chalkbot.status_imu()
    chalkbot.orientation()
    print(chalkbot.pose())