#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <map>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "magic_enum.hpp"

using std::string;

class PythonScript {

private:
    PythonScript() : 
        _isFinalized(false), 
        _pyModule(NULL),
        _threadPeriodic(nullptr) {   
    }

    void _importModule(string directory, string modulename, PyMethodDef* EmbMethods){
        static PyModuleDef EmbModule = {
            PyModuleDef_HEAD_INIT, "display", NULL, -1, EmbMethods,
            NULL, NULL, NULL, NULL
        };
        PyImport_AppendInittab("display", [](){ return PyModule_Create(&EmbModule); } );
        Py_Initialize();
        PyGILState_STATE gstate = PyGILState_Ensure();  
        PyRun_SimpleString("import sys");
        PyRun_SimpleString(string("sys.path.append(\"" + directory + "\")").data());
        PyObject *pName = PyUnicode_DecodeFSDefault(modulename.data());
        _pyModule = PyImport_Import(pName);
        PyObject* config = PyObject_GetAttrString(_pyModule, "CONFIG");

        auto PyMusicDir = PyDict_GetItemString(config, "MusicDir");
        // PyObject* repr = PyObject_Repr(PyMusicDir);
        PyObject* str = PyUnicode_AsEncodedString(PyMusicDir, "utf-8", "~E~");

        musicDirectory = PyBytes_AS_STRING(str);
        Py_DECREF(pName);
        PyGILState_Release(gstate);
        if(_pyModule != NULL) {
            _threadPeriodic = new std::thread(_periodic);
        }
    }
public:
    const char* musicDirectory;

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

    bool executeEvent(const char * func, const char * param = "") {
        assert(_pyModule != NULL);
        const std::lock_guard<std::mutex> lock(_mutex);
        // PyGILState_STATE gstate = PyGILState_Ensure();
        bool result = false;
        PyObject* pFunc = PyObject_GetAttrString(_pyModule, func);
        if(pFunc){
            if(PyCallable_Check(pFunc)){
                PyObject* pArgs = PyTuple_New(1);
                assert(PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(param)) == 0);
                PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
                if(pValue) {
                    result = PyObject_IsTrue(pValue);
                } else {
                    PyErr_Print();
                }
                Py_DECREF(pArgs);
                Py_XDECREF(pValue);
            }
        }
        Py_XDECREF(pFunc);
//        PyGILState_Release(gstate);
        return result;
    }

    int finalize() {
        assert(!_isFinalized);
        PyGILState_STATE gstate = PyGILState_Ensure();      
        Py_XDECREF(_pyModule);
        PyGILState_Release(gstate);
        _isFinalized = true;
        _threadPeriodic->join();
        int res = Py_FinalizeEx();
        return res;
    }

    ~PythonScript() {
        if(!_isFinalized) {
            finalize();
        }
        if(_threadPeriodic != nullptr) {
            delete(_threadPeriodic);
            _threadPeriodic = nullptr;
        }
    }

private:
    static void _periodic() {
        PythonScript * instance = PythonScript::getInstance();
        while(!instance->_isFinalized){
            if(!instance->executeEvent("every1ms")) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

private:
    PyObject* _pyModule;
    volatile bool _isFinalized;
    std::thread* _threadPeriodic;
    std::mutex _mutex;
};