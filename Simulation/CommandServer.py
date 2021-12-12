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

import Robot

PORT = 8000

lastCommand = None
timeOfLastCommand = datetime.timestamp(datetime.now())
driveRequest = [0,0,0,0]
status_motion = "stopped"

def init():
    
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

    driveRequest[0] = log(float(list[0][2:]))
    driveRequest[1] = log(float(list[1]))
    
    driveRequest[2] = int(list[2])
    driveRequest[3] = int(list[3][:len(list[3])-1])

    lastCommand = "drive"
    timeOfLastCommand = datetime.timestamp(datetime.now()) * 1000


class SimulationHandler(SimpleHTTPRequestHandler):

    def do_POST(self):
        print("executing do_POST")
        # get the length of the data to read
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length)
        if self.path == "/status_motion":
            response = status_motion
        elif self.path == "/status_imu":
            response = "N/A"
        elif self.path == "/orientation":
            response = "1.73"
        elif self.path == "/pose":
            response = "1000;300;1.73"
        elif self.path =="/drive":
            set_drive(str(data))
            response = driveRequest
        else:
            response = "unknown command: " + self.path
        print(response)
        # header
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        # response
        self.wfile.write(str(response).encode("utf8"))


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
        pass
