
from ChalkBot import ChalkBotHTTPClient

if __name__ == "__main__":
    chalkbot = ChalkBotHTTPClient("10.0.4.99")
    #chalkbot = ChalkBotHTTPClient("192.168.178.58")

    # check status to test the basic connection
    print("ChalkBot Status: ")
    print("  MOTION:      {}".format( chalkbot.status() ))
    print("  IMU STATE:   {}".format( chalkbot.status_imu() ))
    print("  ORIENTATION: {}".format( chalkbot.orientation() ))
    print("  POSE:        {}".format( chalkbot.pose() ))
