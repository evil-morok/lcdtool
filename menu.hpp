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

    MenuItem * addItem(const char * label, bool home = false) {
        auto item = new MenuItem(this);
        item->label = label;
        item->_root = _root;
        if(home) {
            _root->_parent = item;
        }
        if(isNode()) {
            _items.back()->_next = item;
            item->_previous = _items.back();
        }
        _items.push_back(item);
        return item;
    }

    bool isNode() {
        return _items.size() > 0;
    }

    MenuItem * getParent() {
        return _parent;
    }

    ~MenuItem() {
        for (MenuItem * n : _items) {
            delete(n);
        }
    }

private:
    MenuItem * _parent;
    std::vector<MenuItem *> _items;

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
        _item = 0;
    };

    void render(char * buffer, int len, int items = 2) {
        if(current->isNode()) {

        } else {

        }
        snprintf(buffer, len, "%-16.16s", current->label);
    }

    bool prevItem() {
        if(current->_previous) {
            current = current->_previous;
            return true;
        }
        return false;
    }

    bool nextItem() {
        if(current->_next) {
            current = current->_next;
            return true;
        }
        return false;
    }
    MenuItem * current;

private:
    int _item;
};