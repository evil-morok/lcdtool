import display
import random
from time import sleep

TransitionTime = 2000

rgb = [0, 0, 0]
newRGB = [0, 0, 0]
deltas = [0, 0, 0]

def every1ms(context):
    global rgb, newRGB, deltas, TransitionTime
    
    if abs(rgb[0] - newRGB[0]) < 1 and abs(rgb[1] - newRGB[1]) < 1 and abs(rgb[2] - newRGB[2]) < 1:
        newRGB = [
            random.randint(0, 255), 
            random.randint(0, 255), 
            random.randint(0, 255)
        ]
        deltas = [
            (newRGB[0] - rgb[0]) / TransitionTime, 
            (newRGB[1] - rgb[1]) / TransitionTime, 
            (newRGB[2] - rgb[2]) / TransitionTime
        ]

    rgb[0] += deltas[0]
    rgb[1] += deltas[1]
    rgb[2] += deltas[2]

    color = (int(rgb[0]) & 0xff) + ((int(rgb[1]) & 0xff) << 8) + ((int(rgb[2]) & 0xff) << 16)

    display.backlight(color)

def onStart(context):
    print("=============INIT!===================")


def onStop(context):
    print("=============Stop!===================")
