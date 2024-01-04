#pragma once

#include <iostream>
#include <vector>

#include "view.hpp"

#include "menu.hpp"

#include "view_player.hpp"

class MainView : public View {

public:
    MainView(Menu * menu, View ** currentView) :
    View(currentView, nullptr),
    _windowPosition(0),
    _menu(menu){
        _needUpdate = true;
        _windowPosition = 0;
    }
    virtual void onEnter(){}

    virtual void onKeyUp(){
        _menu->prevItem();
        _needUpdate = true;
    }
    virtual void onKeyDown(){
        _menu->nextItem();
        _needUpdate = true;
    }
    virtual void onKeyLeft(){
        if(_pathBack.size() > 0) {
            _menu->selected = _pathBack.back();
            _pathBack.pop_back();
        }
        _needUpdate = true;
    }
    virtual void onKeyRight(){
        if(_menu->selected->isNode()) {
            _pathBack.push_back(_menu->selected);
            _menu->selected = _menu->selected->items.front();
            _needUpdate = true;
        } else {
            if(_menu->selected->view) {
                EnterView(_menu->selected->view);
            }
        }
    }
    virtual void onKeyOk(){
        _needUpdate = true;
    }
    virtual void render(char * buffer, int rows, int cols){
        auto items = _menu->selected->getParent()->items;
        auto it = std::find(items.begin(), items.end(), _menu->selected);
        int index = it - items.begin();
        int lines = rows < items.size() ? rows : items.size();
        if(index < _windowPosition) {
            _windowPosition = index;
        }
        if(index > _windowPosition + rows - 1) {
            _windowPosition = index - (rows - 1);
        }
        const char * format;
        for (int i = 0; i < lines; i++) {
            if(items[i + _windowPosition] == _menu->selected) {
                format = ">%-14.14s<";
            } else {
                format = " %-14.14s ";
            }
            sprintf(buffer + cols * i, format, items[i + _windowPosition]->label);
        } 
    }
private:
    Menu * _menu;
    int _windowPosition;
    std::vector<MenuItem*> _pathBack;

};
