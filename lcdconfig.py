import display
import random
from os import path
from mpd import MPDClient
from subprocess import call

CONFIG = {
    "MusicDir": "/mnt/Storage/Rogov"
}

client = MPDClient()

def checkConnection():
    try:
        client.ping() != "OK"
    except:
        client.idletimeout = None
        client.connect("localhost", 6600)

playing: bool = False

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

    checkConnection()

    return True

def onStart(arg):
    print("=============INIT!===================")
    checkConnection()
    print("MPD Version {}".format(client.mpd_version))

def onStop(arg):
    print("=============Stop!===================")
    client.close()
    client.disconnect()

def onPlayPath(arg):
    global playing
    print("=============Play!===================")
    music_path = path.relpath(arg, CONFIG["MusicDir"])
    print(music_path)
    client.clear()
    client.add(music_path)
    client.play(0)
    playing = True

def onPausePlay(arg):
    global playing
    playing = not playing
    client.pause(0 if playing else 1)

def onVolumeUp(arg):
    client.volume(10)

def onVolumeDown(arg):
    client.volume(-10)

def onPowerOff(arg):
    call(['poweroff'])
    print("=============PowerOff!===================")

def onKeyUp(arg):
    pass

def onKeyDown(arg):
    pass

def onKeyLeft(arg):
    pass

def onKeyRight(arg):
    pass

def onKeyOk(arg):
    pass

def onRender(arg):
    global playing
    if arg == "Player":
        status = client.status()
        print(status)
        song = client.playlistinfo(status["song"])
        print(song)
        title = song[0]["title"]
        bar = "{:1.1} {:>3.3}".format(
            ">" if playing else "",
            status["volume"]
        )
        display.print(0, 0, "{:^16.16}{:<16.16}".format(title, bar))
        return True
    return False