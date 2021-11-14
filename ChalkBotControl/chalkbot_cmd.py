import cmd, sys

from ChalkBot import ChalkBot

class ChalkBotCMD(cmd.Cmd):
    intro = 'Welcome to the ChalkBot shell.   Type help or ? to list commands.\n'
    prompt = '(chalkbot) '
    file = None
    robot = ChalkBot("10.0.4.99") # default address

    # ----- basic commands -----
    def do_connect(self, arg):
        'Connect to the robot:  connect "10.0.4.99"'
        self.robot = ChalkBot(arg)
        
    def do_status(self, arg):
        'Print robot status: status'
        
        print("ChalkBot Status: ")
        print("  MOTION:      {}".format( self.robot.status() ))
        print("  IMU STATE:   {}".format( self.robot.status_imu() ))
        print("  ORIENTATION: {}".format( self.robot.orientation() ))
        print("  POSE:        {}".format( self.robot.pose() ))
        
    def do_exit(self, arg):
        'Exit ChalkBot command line'
        print('Ending ChalkBot CMD')
        self.close()
        return True

    # ----- record commands and playback -----
    def do_record(self, arg):
        'Save future commands to filename:  RECORD rose.cmd'
        self.file = open(arg, 'w')
    def do_playback(self, arg):
        'Playback commands from a file:  PLAYBACK rose.cmd'
        self.close()
        with open(arg) as f:
            self.cmdqueue.extend(f.read().splitlines())        
    def close(self):
        'stop recording commands and close'
        if self.file:
            self.file.close()
            self.file = None
            
    # ----- command line functionality -----
    def precmd(self, line):
        'catch every command and record it to file except playback'
        line = line.lower()
        if self.file and 'playback' not in line:
            # print line to file
            print(line, file=self.file)
        return line
        
    def onecmd(self, line):
        'catch exceptions and do not stop command line'
        try:
            return super().onecmd(line)
        except Exception as ex:
            print("[Exception] {}".format(ex))
            # display error message
            return False # don't stop
            
    # ----- drive commands -----
    def do_drive(self, args):
        """
        Simple drive without sensor feedback (open loop). 
        
        Parameters:
           v_pwm    - PWM for driving forward (positive) and backward (negative) [-255,255]
           r_pwm    - PWM for turning left (positive) and right (negative) [-255,255]
           p_pwm    - PWM for Printing [0, 255]
           duration - duration of the command in ms
        """
        self.robot.drive(*parse(args))
        
    def do_goto(self, args):
        """
        Turn to and drive to a relative position: 
            goto <x> <y> <p_pwm>
            
        Parameters:
            x     - relative x coorinate in mm (positive = forward)
            y     - relative y coordinate in mm (positive = left)
            p_pwm - PWM for Printing [0, 255]
            
        Example: drive 1m forward and activate the printer
        
            goto 1000 0 255
        """
        self.robot.goto_blocking(*parse(args))
    
def parse(arg):
    'Convert a series of zero or more numbers to an argument tuple'
    return tuple(map(int, arg.split()))

if __name__ == '__main__':
    ChalkBotCMD().cmdloop()