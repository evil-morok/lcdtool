import display
import random
from time import sleep

End = False

def every1ms(context):
    sleep(2)
    display.backlight(random.randint(0, 0xffffff))


def onStart(context):
    print("=============INIT!===================")


def onStop(context):
    print("=============Stop!===================")
