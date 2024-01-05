#pragma once
#include "stdbool.h"

class View {

protected:
    View(View ** currentView, View * parentView) : 
    _currentView(currentView), 
    _parentView(parentView) {}

public:
    virtual const char * getName() = 0;
    virtual void onEnter() = 0;
    virtual void onKeyUp(bool done) = 0;
    virtual void onKeyDown(bool done) = 0;
    virtual void onKeyLeft(bool done) = 0;
    virtual void onKeyRight(bool done) = 0;
    virtual void onKeyOk(bool done) = 0;
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
    void EnterView(View* view) {
        view->onEnter();
        *_currentView = view;
    }

    void Exit() {
        *_currentView = _parentView;
    }

protected:
    bool _needUpdate;

    View ** _currentView;
    View * _parentView;

};




