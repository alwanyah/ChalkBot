from http.server import BaseHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn
from threading import Thread
# import json
# from robot import Robot


class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        # pylint: disable=invalid-name

        #print("executing do_POST")
        # get the length of the data to read
        length = int(self.headers.get('content-length'))
        data = self.rfile.read(length)
        if self.path == "/status_motion":
            response = self.server.robot.status_motion
        elif self.path == "/status_imu":
            response = "N/A"
        elif self.path == "/orientation":
            response = str(self.server.robot.theta)
        elif self.path == "/pose":
            response = str(self.server.robot.x) + ";" + str(self.server.robot.y) + ";" + str(self.server.robot.theta)
        elif self.path =="/drive":
            self.server.set_drive(str(data))
            response = self.server.robot.drive_request
        elif self.path =="/goto_point":
            self.server.goto(str(data))
            response = self.server.robot.goto_point
        else:
            response = "unknown command: " + self.path
        #print(response)
        # header
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        # response
        self.wfile.write(str(response).encode("utf8"))


class CommandServer(HTTPServer):
    def __init__(self, robot, server_address):
        super().__init__(server_address, RequestHandler)
        self.robot = robot

    def set_drive(self, parameters):
        parameter_list = parameters.split(";")

        self.robot.drive_request[0] = float(parameter_list[0][2:])
        if abs(self.robot.drive_request[0]) > 255:
            self.robot.drive_request[0] = 255 * self.robot.drive_request[0] / abs(self.robot.drive_request[0])

        self.robot.drive_request[1] = float(parameter_list[1])
        if abs(self.robot.drive_request[1]) > 255:
            self.robot.drive_request[1] = 255 * self.robot.drive_request[1] / abs(self.robot.drive_request[1])

        self.robot.drive_request[2] = int(parameter_list[2])
        self.robot.drive_request[3] = int(parameter_list[3][:len(parameter_list[3])-1])

        self.robot.last_command = "drive"
        self.robot.time_of_last_command = self.robot.t

    def goto(self, parameters):
        #print(parameters)
        parameter_list = parameters.split(";")
        self.robot.goto_point[0] = float(parameter_list[0][2:])
        self.robot.goto_point[1] = float(parameter_list[1])
        self.robot.goto_point[2] = float(parameter_list[2][:len(parameter_list[2])-1])
        self.robot.last_command = "goto"
        self.robot.time_of_last_command = self.robot.t


class ThreadedCommandServer(ThreadingMixIn, CommandServer):
    pass


class CommandServerThread(Thread):
    def __init__(self, robot, server_address):
        super().__init__()
        self.server = ThreadedCommandServer(robot, server_address)

    def run(self):
        self.server.serve_forever()

    def stop(self):
        self.server.shutdown()
        self.server.server_close()
        self.join()
