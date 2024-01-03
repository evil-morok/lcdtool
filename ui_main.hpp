#pragma once

#include <iostream>
#include <vector>

#include "ui.hpp"

#include "menu.hpp"

class MainUi : public Ui {
public:
    MainUi() : _windowPosition(0){
        _menu.selected = _menu.addItem("Player");
        _menu.addItem("Browse files")->addItem("asdgsdrrg")->getParent()->addItem("11111111111");
        _menu.addItem("Power")->
            addItem("No")->getParent()->
            addItem("Yes");
            _needUpdate = true;
        _windowPosition = 0;
    }
    virtual void onKeyUp(){
        _menu.prevItem();
        _needUpdate = true;
    }
    virtual void onKeyDown(){
        _menu.nextItem();
        _needUpdate = true;
    }
    virtual void onKeyLeft(){
        if(_pathBack.size() > 0) {
            _menu.selected = _pathBack.back();
            _pathBack.pop_back();
        }
        _needUpdate = true;
    }
    virtual void onKeyRight(){
        if(_menu.selected->isNode()) {
            _pathBack.push_back(_menu.selected);
            _menu.selected = _menu.selected->items.front();
            _needUpdate = true;
        }
    }
    virtual void onKeyOk(){
        _needUpdate = true;
    }
    virtual void render(char * buffer, int rows, int cols){
        auto items = _menu.selected->getParent()->items;
        auto it = std::find(items.begin(), items.end(), _menu.selected);
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
            if(items[i + _windowPosition] == _menu.selected) {
                format = ">%-14.14s<";
            } else {
                format = " %-14.14s ";
            }
            sprintf(buffer + cols * i, format, items[i + _windowPosition]->label);
        } 
    }
private:
    Menu _menu;
    int _windowPosition;
    std::vector<MenuItem*> _pathBack;
};
