
#include <iostream>
#include "clipp.h"

#include "display.hpp"

#include "pscrypt.hpp"

#include "view_player.hpp"
#include "view_browser.hpp"
#include "view_power_off.hpp"
#include "view_main.hpp"

#include <thread>

using namespace clipp; 
using std::cout; 
using std::string;

#define ROWS 2
#define COLS 16

#define KEY_FIRST_DELAY_MS 700
#define KEY_DELAY_MS 200

bool WeAreWorking = true;

std::mutex Mutex;

static PyObject* display_backlight(PyObject *self, PyObject *args)
{
    Py_BEGIN_ALLOW_THREADS;
    uint32_t rgb;
    if(!PyArg_ParseTuple(args, "I:backlight", &rgb))
        return NULL;
    Display::getInstance()->setBackLight(rgb);
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

static PyObject* display_print(PyObject *self, PyObject *args)
{
    Py_BEGIN_ALLOW_THREADS;
    uint8_t row, col;
    const char * str;
    if(!PyArg_ParseTuple(args, "bbs:print", &row, &col, &str))
        return NULL;
    {
        const std::lock_guard<std::mutex> lock(Mutex);
        Display::getInstance()->print(row, col, str);
    }
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

static PyObject* display_cls(PyObject *self, PyObject *args)
{
    Py_BEGIN_ALLOW_THREADS;
    Display::getInstance()->clear();
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

static PyMethodDef EmbMethods[] = {
    {"backlight", display_backlight, METH_VARARGS, "Set backlight color"},
    {"print", display_print, METH_VARARGS, "Print chars at the defined position"},
    {"clear", display_cls, METH_VARARGS, "Clears the screen"},
    {NULL, NULL, 0, NULL}
};

View* CurrentView;

void UpdateDisplay() {
    static int updateDelay = 0;
    char buffer[ROWS * COLS + 1];
    while (WeAreWorking) {
        if(updateDelay++ > 500 || CurrentView->NeedUpdate()) {
            if(!PythonScript::getInstance()->executeEvent("onRender", CurrentView->getName())) {
                CurrentView->render(buffer, ROWS, COLS);
                buffer[ROWS * COLS] = 0;
                {
                    const std::lock_guard<std::mutex> lock(Mutex);
                    Display::getInstance()->print(0, 0, buffer);
                }
            }
            updateDelay = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

static Menu MainMenu;

static MainView mainMenu(&MainMenu, &CurrentView);

void StartPlayer(std::string playPath) {
    PythonScript::getInstance()->executeEvent("onPlayPath", playPath.c_str());
}

void PowerOff() {
    PythonScript::getInstance()->executeEvent("onPowerOff");
}

void VolumeUp() {
    PythonScript::getInstance()->executeEvent("onVolumeUp");
}
void VolumeDown() {
    PythonScript::getInstance()->executeEvent("onVolumeDown");
}
void PausePlay() {
    PythonScript::getInstance()->executeEvent("onPausePlay");
}

static PlayerView Player(&CurrentView, &mainMenu, VolumeUp, VolumeDown, PausePlay);
static BrowserView Browser(&CurrentView, &mainMenu, StartPlayer);
static PowerView Power(&CurrentView, &mainMenu, PowerOff);

int main(int argc, char* argv[]) { 
    string confDir = "";
    enum class mode {none, init, daemon};

    MainMenu.selected = MainMenu.addItem("Player", &Player, true);
    MainMenu.addItem("Browse files", &Browser);
    MainMenu.addItem("Power")->
        addItem("No")->getParent()->
        addItem("Yes", &Power);

    CurrentView = &mainMenu;

    mode selected = mode::none;

    auto cli = (        
    //    required("--init").set(selected, mode::init) |
        (required("--daemon").set(selected, mode::daemon) & value("confDir", confDir)) 
    );

    if(!parse(argc, argv, cli)) {
        cout << "Error. Usage:" << std::endl << usage_lines(cli, argv[0]) << std::endl;
        exit(0);
    }

    PythonScript * script;

    try{
        script = PythonScript::initModule(confDir, string("lcdconfig"), EmbMethods);
    } catch (...) {
        std::cout << "Python init error";
        exit(255);
    }

    Browser.setRootDir(script->musicDirectory);



    std::thread threadUpdateDisplay(UpdateDisplay);

    script->executeEvent("onStart");

    int delay = KEY_FIRST_DELAY_MS;

    while (true)
    {        
        BtnMapUnion keyPressed;

        while((keyPressed = Display::getInstance()->getButton()).btnMap8_t == 0) {
            delay = KEY_FIRST_DELAY_MS;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

        if(keyPressed.btnMap8_t == 20) {
            break;
        }
        if(keyPressed.btnMapStruct.up) {
            CurrentView->onKeyUp(script->executeEvent("onKeyUp", CurrentView->getName()));
        } else if(keyPressed.btnMapStruct.down) {
            CurrentView->onKeyDown(script->executeEvent("onKeyDown", CurrentView->getName()));
        } else if(keyPressed.btnMapStruct.left) {
            CurrentView->onKeyLeft(script->executeEvent("onKeyLeft", CurrentView->getName()));
        } else if(keyPressed.btnMapStruct.right) {
            CurrentView->onKeyRight(script->executeEvent("onKeyRight", CurrentView->getName()));
        } else if(keyPressed.btnMapStruct.ok) {
            CurrentView->onKeyOk(script->executeEvent("onKeyOk", CurrentView->getName()));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        delay = KEY_DELAY_MS;
    }
    script->executeEvent("onStop");

    WeAreWorking = false;

    threadUpdateDisplay.join();

    if(script->finalize() < 0) {
        return 120;
    }

    return 0;
}
