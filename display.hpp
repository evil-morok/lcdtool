#pragma once

#include <wiringPi.h>
#include "softPwm.h"
#include <mcp23017.h>
#include <lcd.h>

#include <stdint.h>

class Display {

private:
    Display() : _display(0) {
        _init();
    }

    void _init() {
        wiringPiSetup();
        mcp23017Setup(100, 0x20);

        for(int i = 100; i < 108; i++)
            pinMode(i,OUTPUT);

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
    }
public:
    ~Display() {
        softPwmStop(0);
        softPwmStop(2);
        softPwmStop(3);
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

private:
    int _display;
};

