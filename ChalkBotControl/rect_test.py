
from ChalkBot import ChalkBot
import math


# the print starts in the lower-left corner
def draw_rect(start_direction, length_x, length_y):

    print("turnTo start direction: {}".format(start_direction))
    robot.turnTo(start_direction)
    
    print("drive {}mm".format(length_x))
    robot.drive_forward(length_x, True)
    
    print("turnBy right")
    robot.turnRight()
    
    print("drive {}mm".format(length_y))
    robot.drive_forward(length_y, True)
    
    print("turnBy right")
    robot.turnRight()
    
    print("drive {}mm".format(length_x))
    robot.drive_forward(length_x, True)
    
    print("turnBy right")
    robot.turnRight()
    
    print("drive {}mm".format(length_y))
    robot.drive_forward(length_y, True)
    
    print("turnTo start direction: {}".format(start_direction))
    robot.turnTo(start_direction)

    print("DONE")
    
    
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

    # draw rect with an absolute orientation
    #draw_rect(0, length_x, length_y)
    
    # draw rect with a relative orientation
    #draw_rect(robot.orientation(), length_x, length_y)
    #draw_rect(robot.orientation(), length_x, length_y)
    #draw_rect(robot.orientation(), length_x, length_y)
    
    # draw the rect using orometry navigation 
    draw_rect_odometry(length_x, length_y)
    #draw_rect_odometry(length_x, length_y)
    #draw_rect_odometry(length_x, length_y)
    
    # move away
    #robot.drive_forward(length_y*2, False)
    
  
