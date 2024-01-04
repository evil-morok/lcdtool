
#include <iostream>
#include "clipp.h"

#include "display.hpp"

#include "pscrypt.hpp"

#include "view_player.hpp"
#include "view_browser.hpp"
#include "view_main.hpp"

#include <thread>

using namespace clipp; 
using std::cout; 
using std::string;

#define ROWS 2
#define COLS 16

#define KEY_FIRST_DELAY_MS 700
#define KEY_DELAY_MS 80

bool WeAreWorking = true;

static PyObject* display_backlight(PyObject *self, PyObject *args)
{
    Py_BEGIN_ALLOW_THREADS;
    uint32_t rgb;
    if(!PyArg_ParseTuple(args, "I:backlight", &rgb))
        return NULL;
    Display::getInstanse()->setBackLight(rgb);
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
    Display::getInstanse()->print(row, col, str);
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

static PyObject* display_cls(PyObject *self, PyObject *args)
{
    Py_BEGIN_ALLOW_THREADS;
    Display::getInstanse()->clear();
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

static PyMethodDef EmbMethods[] = {
    {"backlight", display_backlight, METH_VARARGS, "Set backlight color"},
    // {"print", display_print, METH_VARARGS, "Print chars at the defined position"},
    {"clear", display_cls, METH_VARARGS, "Clears the screen"},
    {NULL, NULL, 0, NULL}
};

View* CurrentView;

void UpdateDisplay() {
    static int updateDelay = 0;
    char buffer[ROWS * COLS + 1];
    while (WeAreWorking) {
        if(updateDelay++ > 1000 || CurrentView->NeedUpdate()) {
            CurrentView->render(buffer, ROWS, COLS);
            buffer[ROWS * COLS] = 0;
            Display::getInstanse()->print(0, 0, buffer);
            updateDelay = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

static Menu MainMenu;

static MainView mainMenu(&MainMenu, &CurrentView);
static PlayerView Player(&CurrentView, &mainMenu);

void StartPlayer(std::string playPath) {
    std::cout << playPath << "\n";
}

static BrowserView Browser(&CurrentView, &mainMenu, StartPlayer);

int main(int argc, char* argv[]) { 
    string confDir = "";
    enum class mode {none, init, daemon};

    MainMenu.selected = MainMenu.addItem("Player", &Player);
    MainMenu.addItem("Browse files", &Browser);
    MainMenu.addItem("Power")->
        addItem("No")->getParent()->
        addItem("Yes");

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

    PythonScript * script = PythonScript::initModule(confDir, string("lcdconfig"), EmbMethods);

    Browser.setRootDir(script->musicDirectory);

    std::thread threadUpdateDisplay(UpdateDisplay);

    script->executeEvent("onStart");

    int delay = KEY_FIRST_DELAY_MS;

    while (true)
    {        
        BtnMapUnion keyPressed;

        while((keyPressed = Display::getInstanse()->getButton()).btnMap8_t == 0) {
            delay = KEY_FIRST_DELAY_MS;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

// keyPressed = Display::getInstanse()->getButton();

        if(keyPressed.btnMap8_t == 20) {
            break;
        }
        if(keyPressed.btnMap8_t == 0) {
            script->executeEvent("onKeyUp");
        }
        if(keyPressed.btnMapStruct.up) {
            script->executeEvent("onKeyUp");
            CurrentView->onKeyUp();
        } else if(keyPressed.btnMapStruct.down) {
            script->executeEvent("onKeyDown");
            CurrentView->onKeyDown();
        } else if(keyPressed.btnMapStruct.left) {
            script->executeEvent("onKeyLeft");
            CurrentView->onKeyLeft();
        } else if(keyPressed.btnMapStruct.right) {
            script->executeEvent("onKeyRight");
            CurrentView->onKeyRight();
        } else if(keyPressed.btnMapStruct.ok) {
            script->executeEvent("onKeyOk");
            CurrentView->onKeyOk();
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
