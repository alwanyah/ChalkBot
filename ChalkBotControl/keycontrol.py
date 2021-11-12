'''
pip3 install pynput
'''


from pynput import keyboard
import threading
from threading import Thread
from threading import Event

from ChalkBot import ChalkBot
import math

class MyThread(Thread):
    def __init__(self, event):
        Thread.__init__(self)
        self.stopped = event
        
        self.up = 0
        self.down = 0
        self.left = 0
        self.right = 0
        self.p = 0
        
        self.velocity = 0
        self.rotation = 0
        
        # connect to the robot
        self.robot = ChalkBot("10.0.4.99")
        # reset orientation
        self.orientation = self.robot.orientation()

    def run(self):
        while not self.stopped.wait(0.1):
            #print("my thread")
            self.process_keys()
    
    def process_keys(self):
        #print("{};{};{};{};{}".format(self.up, self.down, self.left, self.right, self.p))
        
        # translation
        forward_speed = 128
        backwards_speed = 64
        self.velocity = self.up*forward_speed - self.down*backwards_speed
        
        # rotation: absolute orientation control
        angle_step = 5.0
        self.orientation += self.left*angle_step - self.right*angle_step
        self.robot.drive_imu(self.velocity, self.orientation/180.0*math.pi, self.p*255, 1000)
        
        # rotation: relative speed control (open loop)
        '''
        angle_speed = 64 # PWM
        rotation_velocity = (self.left - self.right) * angle_speed
        self.robot.drive(self.velocity, int(rotation_velocity), self.p*255, 1000)
        '''
        
        print("{};{};{}".format(self.velocity, self.orientation, self.p*255))

stopFlag = Event()
thread = MyThread(stopFlag)
thread.start()


def on_press(key):
    try:
        #print('Alphanumeric key pressed: {0} '.format(key.char))
        
        c = key.char.lower()
        
        if c == 'p':
            thread.p = 1
        elif c == 'w':
            thread.up = 1
        elif c == 's':
            thread.down = 1
        elif c == 'a':
            thread.left = 1
        elif c == 'd':
            thread.right = 1
            
        elif c == 'q':
            thread.orientation += 90
        elif c == 'e':
            thread.orientation -= 90
            
        #if thread.up + thread.down > 0 and keyboard.Controller.shift_pressed:
        #    thread.p = 1
            
            
    except AttributeError:
        #print('special key pressed: {0}'.format(key))
        pass
        

def on_release(key):
    try:
        #print('Key released: {0}'.format(key))
        
        
        
        if key == keyboard.Key.esc:
            # Stop listener
            # this will stop the timer
            stopFlag.set()
            return False
        
        c = key.char.lower()
        
        if c == 'p':
            thread.p = 0
        elif c == 'w':
            thread.up = 0
        elif c == 's':
            thread.down = 0
        elif c == 'a':
            thread.left = 0
        elif c == 'd':
            thread.right = 0
            
        #if thread.up + thread.down == 0 and keyboard.Controller.shift_pressed:
        #    thread.p = 0
            
            
    except AttributeError:
        #print('special key pressed: {0}'.format(key))
        pass

# Collect events until released
with keyboard.Listener(
        on_press=on_press,
        on_release=on_release) as listener:
    listener.join()
