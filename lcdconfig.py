import display
import random
from os import path
from mpd import MPDClient
from subprocess import call
from typing import Literal
import socket
from time import gmtime, strftime

CONFIG = {
    "MusicDir": "/mnt/Storage/Rogov"
}

client = MPDClient()

class TextScroller:
    def __init__(self, len : int) -> None:
        self.len = len
        self.text = ''
        self.pos = 0

    def _get(self, n):
        return self.text[n % len(self.text)]

    def scroll(self, text : str, continous = False):
        if text != self.text and not continous:
            self.pos = 0
        else:
            self.pos = self.pos + 1
            if self.pos > len(self.text):
                self.pos = 0
        self.text = text
        if len(text) > self.len:
            return "".join([self._get(i + self.pos) for i in range(self.len)])
        else:
            return text

scroller = TextScroller(16)

def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        # doesn't even have to be reachable
        s.connect(('10.254.254.254', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

def checkConnection():
    try:
        client.ping() != "OK"
    except:
        client.idletimeout = None
        client.connect("localhost", 6600)

TransitionTime = 2000

rgb = [0, 0, 0]
newRGB = [0, 0, 0]
deltas = [0, 0, 0]

def playingState() -> Literal['play', 'pause', 'stop']:
    return client.status()["state"]

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

    print("=============Play!===================")
    music_path = path.relpath(arg, CONFIG["MusicDir"])
    print(music_path)
    client.clear()
    client.add(music_path)
    client.play(0)

def onPausePlay(arg):
    client.pause(1 if playingState() == 'play' else 0)

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
    if arg == "Player":
        if playingState() == 'play':
            status = client.status()
            # print(status)
            song = client.playlistinfo(status["song"])
            # print(song)
            title = song[0]["title"]
            top = "{}.{} -- ".format(
                song[0]["track"], 
                title
            )
            bar = "Playing Vol:{:>3.3}".format(
                status["volume"]
            )
            display.print(0, 0, "{:^16.16}{:<16.16}".format(scroller.scroll(top), bar))
        else:
            bar = "IP: {}; DT: {} -- ".format(get_ip(), strftime("%Y-%m-%d %H:%M:%S", gmtime()))
            display.print(0, 0, "{:^16.16}{:<16.16}".format("Stopped", scroller.scroll(bar, True)))
        return True
    return False