#https://docs.python.org/3/library/http.server.html
from http.server import SimpleHTTPRequestHandler, BaseHTTPRequestHandler
import socketserver


import threading
import traceback
import json

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
          print(msg)

          # do something with the data

          result = "DONE!"

          # sed a response
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
  
  print("Serving at port: {}".format(PORT))
  server.serve_forever()
  print("Stopped serving at port: {}".format(PORT))
  