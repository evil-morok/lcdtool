#pragma once

#include "view.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include "natural_sort.hpp"

namespace fs = std::filesystem;

class BrowserView : public View{
public:
    BrowserView(View ** currentView, View * parentView) :
    View(currentView, parentView){

    }
    virtual void onEnter(){
        populate(_rootDir);
    }
    virtual void onKeyUp(){}
    virtual void onKeyDown(){}
    virtual void onKeyLeft(){}
    virtual void onKeyRight(){}
    virtual void onKeyOk(){
        Exit();
    }
    virtual void render(char * buffer, int rows, int cols){
        sprintf(buffer, "%-32.32s", "Browser view");
    }

    void setRootDir(const fs::path& rootDir) {
        _rootDir = rootDir;
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
                std::string sA = (fs::is_directory(a) ? "0" : "1") + std::string(fs::path(a).filename());
                std::string sB = (fs::is_directory(b) ? "0" : "1") + std::string(fs::path(b).filename());
                return SI::natural::compare<std::string>(sA, sB);
            });
    }

private:
    fs::path _rootDir;
    fs::path _currentPath;

    std::vector<fs::directory_entry> _entryList;

};