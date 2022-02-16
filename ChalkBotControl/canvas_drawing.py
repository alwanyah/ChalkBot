from ChalkBot import ChalkBotHTTPClient
import json
from time import sleep

if __name__ == "__main__":

    chalkbot = ChalkBotHTTPClient("127.0.0.1:8000")

    file = open("../canvas/coords.json", "r")
    coordinates = json.load(file)
    
    for i in range (0, len(coordinates['coords'][0])):
        coordinates['coords'][0][i][0] *= 10
        coordinates['coords'][0][i][1] *= 10

    for i in range (0, len(coordinates['coords'][0])):
        while (True):
            if (chalkbot.status() == "stopped"):
                break;

        print("Going to x: ", coordinates['coords'][0][i][0], ", y: ", coordinates['coords'][0][i][1])
        # move to first coordinate without drawing, draw the rest
        if i == 0:
            chalkbot.goto(coordinates['coords'][0][i][0], coordinates['coords'][0][i][1], 0)
        else:
            chalkbot.goto(coordinates['coords'][0][i][0], coordinates['coords'][0][i][1], 255)
        # sleep for 1 ms because deltaTime in Simulation would be zero and thus divide by 0
        sleep(0.05)