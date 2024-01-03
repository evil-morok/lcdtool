#pragma once
#include "stdbool.h"

class Ui {
public:
    virtual void onKeyUp() = 0;
    virtual void onKeyDown() = 0;
    virtual void onKeyLeft() = 0;
    virtual void onKeyRight() = 0;
    virtual void onKeyOk() = 0;
    virtual void render(char * buffer, int rows, int cols) = 0;

public:
    bool NeedUpdate() {
        if(_needUpdate) {
            _needUpdate = false;
            return true;
        }
        return false;
    }
protected:
    bool _needUpdate;

};




