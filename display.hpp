#pragma once

#include <wiringPi.h>
#include "softPwm.h"
#include <mcp23017.h>
#include <lcd.h>

#include <stdint.h>

#include <thread>
#include <mutex>
#include <chrono>

union BtnMapUnion{
    volatile uint8_t btnMap8_t;
    struct {
        volatile uint8_t up     : 1;
        volatile uint8_t down   : 1;
        volatile uint8_t left   : 1;
        volatile uint8_t right  : 1;
        volatile uint8_t ok     : 1;
        volatile uint8_t        : 3;
    } btnMapStruct;
};

class Display {

    static const uint32_t DebounceInterval_us = 50;
    static const uint32_t DebounceMultiplifier = 10;

public:
    enum Buttons {
        btnUp = 112, 
        btnDown = 109, 
        btnLeft = 110, 
        btnRight = 111, 
        btnOk = 108
    };

    enum BtnMap {
        map_up = 0,
        map_down,
        map_left,
        map_right,
        map_ok
    };

    struct Values {
        volatile uint32_t up;
        volatile uint32_t down;
        volatile uint32_t left;
        volatile uint32_t right;
        volatile uint32_t ok;
    };

private:
    Display() : _display(0), _readButtons(true) {
        _init();
        _btnMap.btnMap8_t = 0;
    }

    void _init() {
        wiringPiSetup();
        mcp23017Setup(100, 0x20);

        for(int i = 100; i < 108; i++)
            pinMode(i,OUTPUT);

        for(int i = 108; i < 116; i++)
            pinMode(i,INPUT);


        digitalWrite(107,1);                                                       
        digitalWrite(101,0);                                                       
        _display = lcdInit(2,16,4,100,102,103,104,105,106,0,0,0,0);   

        pinMode(0, OUTPUT);
        softPwmCreate(0, 0, 255);
        pinMode(2, OUTPUT);
        softPwmCreate(2, 0, 255);
        pinMode(3, OUTPUT);
        softPwmCreate(3, 0, 255);

        clear();

        _threadButtonReader = new std::thread(_buttonReader);

    }
public:
    ~Display() {
        softPwmStop(0);
        softPwmStop(2);
        softPwmStop(3);
        _readButtons = false;
        _threadButtonReader->join();
        delete(_threadButtonReader);
    }

    static Display* getInstance() {
        static Display instance;
        return &instance;
    }

    void setBackLight(uint32_t rgb) {
        softPwmWrite(3, rgb & 0xff);
        softPwmWrite(2, (rgb >> 8) & 0xff);
        softPwmWrite(0, (rgb >> 16) & 0xff);
    }

    void print(uint8_t row, uint8_t col, const char *string) {
        lcdPosition(_display, col, row);
        lcdPuts(_display, string);
    }

    void clear() {
        lcdHome(_display);
        lcdClear(_display);
    }

    BtnMapUnion getButton() {
        return _btnMap;
    }

#define FILTER(x)\
            instance->_buttonValues.x = instance->_buttonValues.x - instance->_buttonValues.x / DebounceInterval_us;\
            instance->_buttonValues.x = instance->_buttonValues.x + x * DebounceMultiplifier;\
            instance->_btnMap.btnMapStruct.x = instance->_buttonValues.x > DebounceInterval_us * DebounceMultiplifier / 2

private:
    static void _buttonReader() {
        Display* instance = Display::getInstance();
        while(instance->_readButtons) {
            int up = digitalRead(btnUp);
            int down = digitalRead(btnDown);
            int left = digitalRead(btnLeft);
            int right = digitalRead(btnRight);
            int ok = digitalRead(btnOk);
            FILTER(up);
            FILTER(down);
            FILTER(left);
            FILTER(right);
            FILTER(ok);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

private:
    int _display;
    volatile bool _readButtons;
    std::thread* _threadButtonReader;
    Values _buttonValues;
    BtnMapUnion _btnMap;
};

