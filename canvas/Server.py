#https://docs.python.org/3/library/http.server.html
from http.server import SimpleHTTPRequestHandler, BaseHTTPRequestHandler
import socketserver

import os
import sys
import threading
import traceback
import json
from time import sleep

# HACK: add the path to the ChalkBot
sys.path.append(os.path.join(os.path.dirname(__file__), '../ChalkBotControl'))

from ChalkBot import ChalkBotHTTPClient
from ChalkBot import ChalkBot


PORT = 8800

# debug stuff: print all methods of an object
def print_methods(o):
  object_methods = [method_name for method_name in dir(o) if callable(getattr(o, method_name))]
  print(object_methods)

module = None

class MyHandler(BaseHTTPRequestHandler):

    def do_POST(self):
        print("executing do_POST")

        # get the length of the data to read
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length)
        print(data)

        print(self.path)


        if self.path == "/status_motion":
            response = "stopped"
        elif self.path == "/status_imu":
            response = "no imu here"
        elif self.path == "/orientation":
            response = "1.73"
        elif self.path == "/pose":
            response = "1000;300;1.73"
        else:
            response = "unknown command: " + self.path

        print(response)

        # header
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

        # response
        self.wfile.write(str(response).encode("utf8"))


class ChalkbotHandler(SimpleHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

    '''
    def do_GET(self):
        #super().do_GET()
        #super(NaoQiHandler, self).do_GET()
        import os
        self._set_headers()
        self.wfile.write(str(path).encode("utf8"))
    '''
    
    def sendToRobot(self, coordinates):
        chalkbot = ChalkBotHTTPClient("127.0.0.1:8000")
        #chalkbot = ChalkBot("10.0.4.99")
    
        for j in range(0, len(coordinates['coords'])):
            # need to scale coordinates to make the drawing more visable
            for i in range(0, len(coordinates['coords'][j])):
                coordinates['coords'][j][i][0] *= 10
                coordinates['coords'][j][i][1] *= 10

        for j in range(0, len(coordinates['coords'])):
            for i in range(0, len(coordinates['coords'][j])):
                # need to wait until goto is done drawing
                while (chalkbot.status() == "moving"):
                    ...

                # print("Going to x:", coordinates['coords'][0][i][0], ", y:", coordinates['coords'][0][i][1])
                # move to first coordinate without drawing, draw the rest
                if i == 0:
                    chalkbot.goto(coordinates['coords'][j][i][0], coordinates['coords'][j][i][1], 0)
                else:
                    chalkbot.goto(coordinates['coords'][j][i][0], coordinates['coords'][j][i][1], 255)
                # sleep for 50 ms because deltaTime in Simulation would be zero and thus divide by 0 and so no gotos are skipped
                sleep(0.05)

    def do_HEAD(self):
        self._set_headers()

    def do_POST(self):
        try:
            # get the length of the data to read
            length = int(self.headers.get('content-length'))
            data = self.rfile.read(length)

            print(self.requestline)
            print(self.path)
            print(self.command)

            print(self.headers.keys())

            print(length)
            print(data)
            # parse json data
            msg = json.loads(data)
            
            
            
            # TODO: this is to remove the helpe ently from the directory. 
            # There must be a better solution.
            save_to_file = msg['file']
            del msg['file']

            # do something with the data
            if save_to_file:
                file = open("coords.json", "w")
                file.write(json.dumps(msg))
                file.close()
            else:
                self.sendToRobot(msg)
                
            print(msg)

            result = "DONE!"

            # send a response
            self._set_headers()
            self.wfile.write(str(result).encode("utf8"))

        except Exception:
            self._set_headers()
            trace = traceback.format_exc()
            print(trace)
            self.wfile.write(str(trace).encode("utf8"))

    def log_message(self, format, *args):
        return


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass


if __name__ == "__main__":

  ThreadedTCPServer.allow_reuse_address = True
  server = ThreadedTCPServer(("", PORT), ChalkbotHandler)
  #server = ThreadedTCPServer(("", PORT), MyHandler)

  print("Serving at: http://127.0.0.1:{}".format(PORT))
  server.serve_forever()
  print("Stopped serving: http://127.0.0.1:{}".format(PORT))
