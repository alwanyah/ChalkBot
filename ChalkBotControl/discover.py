#!/usr/bin/env python3

import socket

PORT = 8002
ADDRESS = "239.255.67.66"

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.settimeout(1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
s.bind(("0.0.0.0", 0))
s.sendto(b"hello", (ADDRESS, PORT))

try:
    while True:
        data, addr = s.recvfrom(512)
        print("{}: {}".format(addr[0], data))
except TimeoutError:
    pass
