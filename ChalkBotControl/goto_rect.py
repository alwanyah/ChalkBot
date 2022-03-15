
from ChalkBot import ChalkBot
import math


def draw_rect_odometry(length_x, length_y):
    robot.goto_blocking(       0,        0, 0)
    robot.goto_blocking(length_x,        0, 255)
    robot.goto_blocking(length_x, length_y, 255)
    robot.goto_blocking(       0, length_y, 255)
    robot.goto_blocking(       0,        0, 255)
    

if __name__ == "__main__":
    robot = ChalkBot("10.0.4.99")
    
    # enable debug if necessary
    #robot.debug(10)
    
    length_x = 1000
    length_y = 1000

    # draw the rect using orometry navigation 
    draw_rect_odometry(length_x, length_y)
    
    
  
