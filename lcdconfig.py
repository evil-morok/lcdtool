import display

def onStart(context):
    print("=============INIT!===================")
    for i in range(0xffffff):
        display.backlight(i)
