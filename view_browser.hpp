#pragma once

#include "view.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <mutex>
#include <functional>

#include "natural_sort.hpp"

namespace fs = std::filesystem;

typedef std::function<void(std::string)> ActionCallback;

class BrowserView : public View{
public:
    BrowserView(View ** currentView, View * parentView, ActionCallback action) :
    View(currentView, parentView),
    _action(action),
    _selectedEntry(0),
    _windowPosition(0){}

    virtual void onEnter(){
        populate(_currentPath);
        _needUpdate = true;
    }

    virtual void onKeyUp(){
        if(_selectedEntry > 0) {
            _selectedEntry--;
        }
        _needUpdate = true;
    }

    virtual void onKeyDown(){
        if(_selectedEntry < _entryList.size()) {
            _selectedEntry++;
        }
        _needUpdate = true;
    }

    virtual void onKeyLeft(){
        Exit();
        _needUpdate = true;
    }

    virtual void onKeyRight(){
        const std::lock_guard<std::mutex> lock(_mutex);
        if(_selectedEntry > 0) {
            if(fs::is_directory(_entryList[_selectedEntry - 1])) {
                _currentPath = _entryList[_selectedEntry - 1].path();
                _positions.push_back(_selectedEntry);
                _selectedEntry = 0;
            }
        } else {
            _currentPath = _currentPath.parent_path();
            _selectedEntry = _positions.back();
            _positions.pop_back();
        }
        populate(_currentPath);
        _needUpdate = true;
        
    }

    virtual void onKeyOk(){
        if(_selectedEntry > 0) {
            _action(_entryList[_selectedEntry - 1].path().string());
        } else {
            _action(_currentPath.string());
        }
        _needUpdate = true;
    }

    virtual void render(char * buffer, int rows, int cols){
        const std::lock_guard<std::mutex> lock(_mutex);
        int lines = rows < _entryList.size() + 1 ? rows : _entryList.size() + 1;
        if(_selectedEntry < _windowPosition) {
            _windowPosition = _selectedEntry;
        }
        if(_selectedEntry > _windowPosition + rows - 1) {
            _windowPosition = _selectedEntry - (rows - 1);
        }
        const char * format;
        if(_currentPath == _rootDir) {
            if(_selectedEntry == 0)
                _selectedEntry = 1;

            if(_windowPosition == 0) 
                _windowPosition = 1;
        }
        for (int i = 0; i < lines; i++) {
            if(i + _windowPosition == _selectedEntry) {
                format = ">%-14.14s<";
            } else {
                format = " %-14.14s ";
            }
            string line;
            if(i + _windowPosition > 0) {
                line = _entryList[i + _windowPosition - 1].path().filename();
                // if(fs::is_directory(_entryList[i + _windowPosition - 1])) {
                // }
                if(line.length() > cols - 2) {
                    line.erase(4, line.length() - (cols - 3));
                    line.insert(4, "~");

                }
            }else{
                line = "/..";
            }
            sprintf(buffer + cols * i, format, line.c_str());
        } 
    }

    void setRootDir(const fs::path& rootDir) {
        _rootDir = rootDir;
        _currentPath = _rootDir;
    }

private:
    void populate(fs::path path) {
        if(fs::is_directory(path))
        _entryList.clear();
        std::ranges::for_each(
            std::filesystem::directory_iterator{path},
            [this](const auto& dirEntry) { _entryList.push_back(dirEntry); });
        std::sort(_entryList.begin(), _entryList.end(), 
            [](const fs::directory_entry& a, const fs::directory_entry& b){
                std::string sA = (fs::is_directory(a) ? "@" : "z") + std::string(fs::path(a).filename());
                std::string sB = (fs::is_directory(b) ? "@" : "z") + std::string(fs::path(b).filename());
                bool ass = SI::natural::compare<std::string>(sA, sB);
                return ass;
            });
    }

private:
    fs::path _rootDir;
    fs::path _currentPath;

    std::vector<fs::directory_entry> _entryList;
    std::vector<int> _positions;
    int _selectedEntry;
    int _windowPosition;

    ActionCallback _action;

    std::mutex _mutex;
};