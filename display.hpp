#pragma once

#include <wiringPi.h>
#include "softPwm.h"
#include <mcp23017.h>
#include <lcd.h>

#include <stdint.h>

#include <thread>
#include <mutex>
#include <chrono>

class Display {

    static const uint32_t DebounceInterval_us = 50;

public:
    enum Buttons {
        btnUp = 112, 
        btnDown = 109, 
        btnLeft = 110, 
        btnRight = 111, 
        btnOk = 108
    };

    struct Values {
        uint32_t up;
        uint32_t down;
        uint32_t left;
        uint32_t right;
        uint32_t ok;
    };

private:
    Display() : _display(0), _readButtons(true) {
        _init();
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

        lcdHome(_display);
        lcdClear(_display);

        lcdPosition(_display, 0, 0);
        lcdPuts(_display, "epwiojpeofjgp[ewrojgpoerjgpeojfpejfvpeojfvpefjvperjvfpe]");

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

    static Display* getInstanse() {
        static Display instance;
        return &instance;
    }

    void setBackLight(uint32_t rgb) {
        softPwmWrite(3, rgb & 0xff);
        softPwmWrite(2, (rgb >> 8) & 0xff);
        softPwmWrite(0, (rgb >> 16) & 0xff);
    }

    uint8_t getButton() {
        

    }

#define FILTER(x)\
            instance->_buttonValues.x -= instance->_buttonValues.x / DebounceInterval_us;\
            instance->_buttonValues.x += x * 10

private:
    static void _buttonReader() {
        Display* instance = Display::getInstanse();
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

private:
    int _display;
    volatile bool _readButtons;
    std::thread* _threadButtonReader;
    Values _buttonValues;
};

