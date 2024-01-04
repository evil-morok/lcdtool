#pragma once
#include "view.hpp"
#include <iostream>

class PlayerView : public View{
public:
    PlayerView(View ** currentView, View * parentView) :
    View(currentView, parentView){

    }
    virtual void onEnter(){}
    virtual void onKeyUp(){}
    virtual void onKeyDown(){}
    virtual void onKeyLeft(){}
    virtual void onKeyRight(){}
    virtual void onKeyOk(){
        Exit();
    }
    virtual void render(char * buffer, int rows, int cols){
        sprintf(buffer, "%-32.32s", "---- Player view ----");
    }

private:

};