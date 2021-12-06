from __future__ import print_function
from typing import get_args


from datetime import datetime

# note this for Pythong 2.7 and 3.x compatibility
try:
  from SimpleHTTPServer import SimpleHTTPRequestHandler
  import SocketServer as socketserver
except ImportError:
  from http.server import SimpleHTTPRequestHandler
  import socketserver


import threading
import traceback
import json

import Robot

PORT = 8000

# debug stuff
def print_methods(o):
  object_methods = [method_name for method_name in dir(o) if callable(getattr(o, method_name))]
  print(object_methods)

module = None

import enum
class Commands(enum.Enum):
    DRIVE = 1
    MOTOR_TEST = 2
    DRIVE_IMU = 3
    GOTO = 4

lastCommand = None
timeOfLastCommand = 0

driveRequest = [0,0,0,0]

def set_drive(parameters):
        list = parameters.split(";")
        driveRequest[0] = int(list[0][2:])
        driveRequest[1] = int(list[1])
        driveRequest[2] = int(list[2])
        driveRequest[3] = int(list[3][:len(list[3])-1])
        lastCommand = "drive"
        timeOfLastCommand = datetime.now.timestamp() * 1000

class SimulationHandler(SimpleHTTPRequestHandler):


    def do_POST(self):
        print("executing do_POST")

        # get the length of the data to read
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length)
         
        if self.path == "/status_motion":
            response = "stopped"
        elif self.path == "/status_imu":
            response = "no imu here"
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

def initServer():
    ThreadedTCPServer.allow_reuse_address = True
    server = ThreadedTCPServer(("", PORT), SimulationHandler)
    server.serve_forever()
    return server

          
if __name__ == "__main__":
  
    
  ThreadedTCPServer.allow_reuse_address = True
  #server = ThreadedTCPServer(("", PORT), ChalkbotHandler)
  server = ThreadedTCPServer(("", PORT), SimulationHandler)

  print("Serving at port: {}".format(PORT))
  server.serve_forever()
  print("Stopped serving at port: {}".format(PORT))
  