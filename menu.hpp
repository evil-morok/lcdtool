#pragma once
#include <vector>

class Menu;

class MenuItem {

friend Menu;

protected:
    MenuItem(MenuItem* parent) : 
    _parent(parent), 
    _previous(nullptr), 
    _next(nullptr), 
    _root(nullptr), 
    _home(nullptr) {}

public:

    MenuItem * addItem(const char * label, View * view = nullptr, bool home = false) {
        auto item = new MenuItem(this);
        item->label = label;
        item->_root = _root;
        item->view = view;
        if(home) {
            _root->_parent = item;
        }
        if(isNode()) {
            items.back()->_next = item;
            item->_previous = items.back();
        }
        items.push_back(item);
        return item;
    }

    bool isNode() {
        return items.size() > 0;
    }

    MenuItem * getParent() {
        return _parent;
    }

    ~MenuItem() {
        for (MenuItem * n : items) {
            delete(n);
        }
    }

private:
    MenuItem * _parent;

public:
    std::vector<MenuItem *> items;
    View * view;

protected:
    MenuItem * _previous;
    MenuItem * _next;
    MenuItem * _root;
    MenuItem * _home;

public:
    const char * label;
};

class Menu : public MenuItem {

public:
    Menu() : MenuItem(nullptr) {
        _root = this;
        label = "";
    };

    bool prevItem() {
        if(selected->_previous) {
            selected = selected->_previous;
            return true;
        }
        return false;
    }

    bool nextItem() {
        if(selected->_next) {
            selected = selected->_next;
            return true;
        }
        return false;
    }
    MenuItem * selected;

};