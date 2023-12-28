
#include <iostream>
#include "clipp.h"

#include "display.hpp"

#include "pscrypt.hpp"

using namespace clipp; 
using std::cout; 
using std::string;


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
    {"print", display_print, METH_VARARGS, "Print chars at the defined position"},
    {"clear", display_cls, METH_VARARGS, "Clears the screen"},
    {NULL, NULL, 0, NULL}
};


int main(int argc, char* argv[]) { 
    string confDir = "";
    enum class mode {none, init, daemon};

    mode selected = mode::none;

    auto cli = (        
        required("--init").set(selected, mode::init) |
        (required("--daemon").set(selected, mode::daemon) & value("confDir", confDir)) 
    );

    if(!parse(argc, argv, cli)) {
        cout << "Error. Usage:" << std::endl << usage_lines(cli, argv[0]) << std::endl;
        exit(0);
    }

    PythonScript * script = PythonScript::initModule(confDir, string("lcdconfig"), EmbMethods);

    script->executeEvent(PythonScript::onStart);

    while (true)
    {        
        BtnMapUnion keyPressed;

        while((keyPressed = Display::getInstanse()->getButton()).btnMap8_t == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if(keyPressed.btnMap8_t == 20) {
            break;
        }

        if(keyPressed.btnMapStruct.up) {
            script->executeEvent(PythonScript::onKeyUp);
        } else if(keyPressed.btnMapStruct.down) {
            script->executeEvent(PythonScript::onKeyDown);
        } else if(keyPressed.btnMapStruct.left) {
            script->executeEvent(PythonScript::onKeyLeft);
        } else if(keyPressed.btnMapStruct.right) {
            script->executeEvent(PythonScript::onKeyRight);
        } else if(keyPressed.btnMapStruct.ok) {
            script->executeEvent(PythonScript::onKeyOk);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    script->executeEvent(PythonScript::onStop);

    if(script->finalize() < 0) {
        return 120;
    }

    return 0;
}
