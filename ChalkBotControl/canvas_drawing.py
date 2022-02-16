from ChalkBot import ChalkBotHTTPClient
import json
from time import sleep

if __name__ == "__main__":

    var = input("Type 0 to draw with the ChalkBot, type 1 to draw in the Simulation: ")
    if var == "0":
        chalkbot = ChalkBotHTTPClient("10.0.4.99")
    elif var == "1":
        chalkbot = ChalkBotHTTPClient("127.0.0.1:8000")
    else: 
        print("Invalid input. Type 0 or 1.")
        exit(1)

    file = open("../canvas/coords.json", "r")
    coordinates = json.load(file)
    file.close()
    
    # need to scale coordinates to make the drawing more visable
    for i in range (0, len(coordinates['coords'][0])):
        coordinates['coords'][0][i][0] *= 10
        coordinates['coords'][0][i][1] *= 10

    for i in range (0, len(coordinates['coords'][0])):
        # need to wait until goto is done drawing
        while (True):
            if (chalkbot.status() == "stopped"):
                break;

        # print("Going to x:", coordinates['coords'][0][i][0], ", y:", coordinates['coords'][0][i][1])
        # move to first coordinate without drawing, draw the rest
        if i == 0:
            chalkbot.goto(coordinates['coords'][0][i][0], coordinates['coords'][0][i][1], 0)
        else:
            chalkbot.goto(coordinates['coords'][0][i][0], coordinates['coords'][0][i][1], 255)
        # sleep for 50 ms because deltaTime in Simulation would be zero and thus divide by 0 and so no gotos are skipped
        sleep(0.05)