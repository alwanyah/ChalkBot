from __future__ import print_function

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

PORT = 8000

# debug stuff
def print_methods(o):
  object_methods = [method_name for method_name in dir(o) if callable(getattr(o, method_name))]
  print(object_methods)

module = None


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
          # python 2.7
          if hasattr(self.headers, 'getheader'):
            length = int(self.headers.getheader('content-length'))
          else: # python 3.x
            length = int(self.headers.get('content-length'))

          data = self.rfile.read(length)

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
  
  print("Serving at port: {}".format(PORT))
  server.serve_forever()
  print("Stopped serving at port: {}".format(PORT))
  