
#include <iostream>
#include "clipp.h"
#include <wiringPi.h>
#include <mcp23017.h>
#include <lcd.h>


#define PY_SSIZE_T_CLEAN
#include <Python.h>

using namespace clipp; 
using std::cout; 
using std::string;


static void reprint(PyObject *obj) {
    PyObject* repr = PyObject_Repr(obj);
    PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
    const char *bytes = PyBytes_AS_STRING(str);

    printf("REPR: %s\n", bytes);

    Py_XDECREF(repr);
    Py_XDECREF(str);
}


int main(int argc, char* argv[]) { 
    string script = "";
    enum class mode {none, init, run};

    mode selected = mode::none;

    auto cli = (        
        required("--init").set(selected, mode::init) |
        (required("--run").set(selected, mode::run) & value("script", script)) 
    );

    //cout << usage_lines(cli, argv[0]) << std::endl;

    if(!parse(argc, argv, cli)) {
        cout << "Error. Usage:" << std::endl << usage_lines(cli, argv[0]) << std::endl;
        exit(0);
    }
    
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/mnt/Storage/sources/lcdtool/.\")");
    pName = PyUnicode_DecodeFSDefault("lcdconfig");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "init");
        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, NULL);
            Py_XDECREF(pValue);
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    }
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    // wiringPiSetup();
    // mcp23017Setup(100, 0x20);

    // for(int i = 100; i < 108; i++)
    //       pinMode(i,OUTPUT);

    // digitalWrite(107,1);                                                       
    // digitalWrite(101,0);                                                       
    // display=lcdInit(2,16,4,100,102,103,104,105,106,0,0,0,0);   

    // pinMode(0,OUTPUT);
    // pinMode(2,OUTPUT);
    // pinMode(3,OUTPUT);

    // switch (selected)
    // {
    // case mode::line1:
    //     lcdPosition(display, 0, 0);
    //     lcdPuts(display, line1.c_str());
    //     break;

    // case mode::line2:
    //     lcdPosition(display, 0, 1);
    //     lcdPuts(display, line2.c_str());
    //     break;

    // case mode::home:
    //     lcdHome(display);
    //     lcdClear(display);

    // case mode::on:
    //     digitalWrite(0, r);
    //     digitalWrite(2, g);
    //     digitalWrite(3, b);
    //     break;

    // case mode::off:            
    //     digitalWrite(0, 0);
    //     digitalWrite(2, 0);
    //     digitalWrite(3, 0);
    //     break;

    // case mode::up:
    //     cout << "Up" << std::endl;
    //     break;

    // case mode::down:
    //     cout << "Down" << std::endl;
    //     break;

    // case mode::left:
    //     cout << "Left" << std::endl;
    //     break;

    // case mode::right:
    //     cout << "Right" << std::endl;
    //     break;

    // case mode::ok:
    //     cout << "Ok" << std::endl;
    //     break;

    // case mode::none:
    // default:
    //     break;
    // }
    return 0;
}
