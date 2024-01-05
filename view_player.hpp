#pragma once
#include "view.hpp"
#include <iostream>
#include <functional>

typedef std::function<void()> VolumeUpCallback;
typedef std::function<void()> VolumeDownCallback;
typedef std::function<void()> PausePlayCallback;

class PlayerView : public View{
public:
    PlayerView(View ** currentView, View * parentView,
        VolumeUpCallback volumeUp,
        VolumeDownCallback volumeDown,
        PausePlayCallback pausePlay
    ) :
    _volumeUp(volumeUp), _volumeDown(volumeDown), _pausePlay(pausePlay),
    View(currentView, parentView){
    }

    virtual const char * getName() {
        return "Player";
    }
    virtual void onEnter(){}
    virtual void onKeyUp(bool done){
        if (!done) _volumeUp();
        _needUpdate = true;
    }
    virtual void onKeyDown(bool done){
        if (!done) _volumeDown();
        _needUpdate = true;
    }
    virtual void onKeyLeft(bool done){
        if (!done) Exit();
        _needUpdate = true;
    }
    virtual void onKeyRight(bool done){
        _needUpdate = true;
    }
    virtual void onKeyOk(bool done){
        if (!done) _pausePlay();
        _needUpdate = true;
    }
    virtual void render(char * buffer, int rows, int cols){
        sprintf(buffer, "%-32.32s", "---- Player view ----");
    }

private:
    VolumeUpCallback _volumeUp;
    VolumeDownCallback _volumeDown;
    PausePlayCallback _pausePlay;
};