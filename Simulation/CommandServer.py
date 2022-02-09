from __future__ import print_function
from typing import get_args

import math
from datetime import datetime

# note this for Pythong 2.7 and 3.x compatibility

from http.server import SimpleHTTPRequestHandler
import socketserver


import threading
import traceback
import json
import numpy as np
import Robot

PORT = 8000

lastCommand = None
t = 0
timeOfLastCommand = t

driveRequest = [0,0,0,0]
status_motion = "stopped"
goto_point = [0,0,0]
server = None 

pose = [0,0]
orientation = 0

def reset():
    global lastCommand, t, timeOfLastCommand, driveRequest, status_motion, goto_point, pose, orientation
    lastCommand = None
    t = 0
    timeOfLastCommand = t

    driveRequest = [0,0,0,0]
    status_motion = "stopped"
    goto_point = [0,0,0]

    pose = [0,0]
    orientation = 0

def init():
    global server
    ThreadedTCPServer.allow_reuse_address = True
    server = ThreadedTCPServer(("", PORT), SimulationHandler)
    threading.Thread(target=server.serve_forever).start()
def log(x):
    if (x > 0):
        return math.log(x)
    elif (x < 0):
        return -1 * math.log(-1 * x)
    else:
        return 0
    

def set_drive(parameters):

    global driveRequest, lastCommand, timeOfLastCommand
    list = parameters.split(";")

    driveRequest[0] = float(list[0][2:])
    if abs(driveRequest[0]) > 255:
        driveRequest[0] = 255 * driveRequest[0]/abs(driveRequest[0])
    
    driveRequest[1] = float(list[1])
    if abs(driveRequest[1]) > 255:
        driveRequest[1] = 255 * driveRequest[1]/abs(driveRequest[1])

    driveRequest[2] = int(list[2])
    driveRequest[3] = int(list[3][:len(list[3])-1])

    lastCommand = "drive"
    timeOfLastCommand = t

def goto(parameters):
    global goto_point, lastCommand, timeOfLastCommand
    #print(parameters)
    list = parameters.split(";")
    goto_point[0] = float(list[0][2:])
    goto_point[1] = float(list[1])
    goto_point[2] = float(list[2][:len(list[2])-1])
    lastCommand = "goto"
    timeOfLastCommand = t
    

def close():
    server.shutdown()
    server.server_close()

class SimulationHandler(SimpleHTTPRequestHandler):

    def do_POST(self):
        #print("executing do_POST")
        # get the length of the data to read
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length)
        if self.path == "/status_motion":
            response = status_motion
        elif self.path == "/status_imu":
            response = "N/A"
        elif self.path == "/orientation":
            response = str(orientation)
        elif self.path == "/pose":
            response = str(pose[0])+";"+str(pose[1])+";"+str(orientation)
        elif self.path =="/drive":
            set_drive(str(data))
            response = driveRequest
        elif self.path =="/goto_point":
            goto(str(data))
            response = goto_point
        else:
            response = "unknown command: " + self.path
        #print(response)
        # header
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        # response
        self.wfile.write(str(response).encode("utf8"))


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
        pass
