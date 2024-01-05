#pragma once
#include "view.hpp"
#include <iostream>
#include <functional>

typedef std::function<void()> ShutdownCallback;

class PowerView : public View{
public:
    PowerView(View ** currentView, View * parentView, ShutdownCallback shutdown) :
    View(currentView, parentView),
    _shutdown(shutdown){
    }

    virtual const char * getName() {
        return "PowerOff";
    }

    virtual void onEnter(){
        _shutdown();
        _needUpdate = true;
    }
    virtual void onKeyUp(bool done){}
    virtual void onKeyDown(bool done){}
    virtual void onKeyLeft(bool done){}
    virtual void onKeyRight(bool done){}
    virtual void onKeyOk(bool done){}
    
    virtual void render(char * buffer, int rows, int cols){
        sprintf(buffer, "%-32.32s", " Shutting down ");
    }

private:
    ShutdownCallback _shutdown;
};