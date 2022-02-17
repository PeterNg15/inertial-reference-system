import numpy as np;


def accelToVel(acc: dict, angle: dict):

    return;

"""
position: is a dictionary that contains info about current position
dX: change in distance in X direction (meter)
dY: change in distance in Y direction (meter)
"""
def displacement(position: dict, dX: float, dY: float):
    position["x"] += dX;
    position["y"] += dY;
    return;

"""
This function returns the equivalent angle (rad) that is between 0 <= theta <= 2pi
angle: in rad; Relative to north. This angle will return a rad between 0 and 2pi;
"""
def correctAngle(angle: float):
    if( angle >= 0 & angle < (2*np.pi)):
        return angle;
    elif(angle < 0 ):
        return (2 * np.pi) - angle;
    elif(angle > (2*np.pi)):
        return angle - (2 * np.pi);
    return;