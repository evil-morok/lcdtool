#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "magic_enum.hpp"

using std::string;


class PythonScript {

public:
    enum Events { 
        onStart, onStop, onMenuSelected, onKeyPressed
    };

private:
    PythonScript() : 
        _isFinalized(false), 
        _pyModule(NULL) {   
    }

    void _importModule(string directory, string modulename, PyMethodDef* EmbMethods){
        static PyModuleDef EmbModule = {
            PyModuleDef_HEAD_INIT, "display", NULL, -1, EmbMethods,
            NULL, NULL, NULL, NULL
        };
        PyImport_AppendInittab("display", [](){ return PyModule_Create(&EmbModule); } );
        Py_Initialize();
        _pyContext = PyDict_New();    
        PyRun_SimpleString("import sys");
        PyRun_SimpleString(string("sys.path.append(\"" + directory + "\")").data());
        PyObject *pName = PyUnicode_DecodeFSDefault(modulename.data());
        _pyModule = PyImport_Import(pName);
        Py_DECREF(pName);
    }

public:
    static PythonScript* initModule(string directory, string modulename, PyMethodDef* EmbMethods) {
        PythonScript* instance = getInstance();
        assert(instance->_pyModule == NULL);
        instance->_importModule(directory, modulename, EmbMethods);
        return instance;
    }

    static PythonScript* getInstance(){
        static PythonScript instance;
        assert(!instance._isFinalized);
        return &instance;
    }

    void executeEvent(Events e) {
        assert(_pyModule != NULL);
        PyObject* pFunc = PyObject_GetAttrString(_pyModule, magic_enum::enum_name(e).data());
        if(pFunc && PyCallable_Check(pFunc)){
            PyObject* pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, _pyContext);
            PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            Py_XDECREF(pValue);
        }
        Py_XDECREF(pFunc);
    }

    int finalize() {
        assert(!_isFinalized);
        Py_DECREF(_pyContext);        
        Py_XDECREF(_pyModule);
        _isFinalized = true;
        return Py_FinalizeEx();
    }

    ~PythonScript() {
        if(!_isFinalized) {
            finalize();
        }
    }

    static void reprint(PyObject *obj) {
        PyObject* repr = PyObject_Repr(obj);
        PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
        const char *bytes = PyBytes_AS_STRING(str);

        printf("REPR: %s\n", bytes);

        Py_XDECREF(repr);
        Py_XDECREF(str);
    }

private:
    PyObject* _pyModule;
    PyObject* _pyContext;
    bool _isFinalized;

};