
#include <iostream>
#include "clipp.h"

#include "display.hpp"

#include "pscrypt.hpp"

using namespace clipp; 
using std::cout; 
using std::string;


static PyObject* display_backlight(PyObject *self, PyObject *args)
{
    uint32_t rgb;
    if(!PyArg_ParseTuple(args, "I:backlight", &rgb))
        return NULL;
    Display::getInstanse()->setBackLight(rgb);
    Py_RETURN_NONE;
}

static PyMethodDef EmbMethods[] = {
    {"backlight", display_backlight, METH_VARARGS, "Set backlight color"},
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

    if(script->finalize() < 0) {
        return 120;
    }

    return 0;
}
