#include "cppython/cppython.h"
#include "cppython/CPPyObject.h"

#include <iostream>
#include <sstream>

int high_level_embedding(int argc, char *argv[])
{
    wchar_t *program = Py_DecodeLocale(argv[0], nullptr);
    if (program == nullptr) {
        throw std::runtime_error("Fatal error: cannot decode first argument");
    }
    Py_SetProgramName(program);
    Py_Initialize();
    PyRun_SimpleString("from time import time, ctime\n"
                       "print('Today is', ctime(time()))\n");
    if (Py_FinalizeEx() < 0) {
        throw std::runtime_error("Error finalizing the Python interpreter");
    }
    PyMem_RawFree(program);
    return 0;
}

int low_level_embedding(int argc, char *argv[])
{
    int i;

    if (argc < 3) {
        std::clog << "Usage: call python_file function_name [args]" << std::endl;
        return 1;
    }

    std::wstringstream path;
    path << Py_GetPath() << ":./python";
    Py_SetPath(path.str().c_str());

    Py_Initialize();
    {
        python::Object module;
#ifdef USE_NAMES
        module.set_name("empty module");
#endif

        {
            python::Object name = PyUnicode_DecodeFSDefault(argv[1]);
#ifdef USE_NAMES
            name.set_name("pName");
#endif
            if (!name) {
                throw std::runtime_error("PyUnicode_DecodeFSDefault failed");
            }

            module = PyImport_Import(name.get());
#ifdef USE_NAMES
            module.set_name("module");
#endif
        }


        if (module) {
            python::Object pFunc = PyObject_GetAttrString(*module, argv[2]);
#ifdef USE_NAMES
            pFunc.set_name("pFunc");
#endif
            // pFunc is a new reference

            if (pFunc && PyCallable_Check(pFunc.get())) {
                python::Object args = PyTuple_New(argc - 3);
#ifdef USE_NAMES
                args.set_name("args");
#endif
                python::Object pValue;
#ifdef USE_NAMES
                pValue.set_name("pValue empty");
#endif
                for (i = 0; i < argc - 3; ++i) {
                    pValue = PyLong_FromLong(atoi(argv[i + 3]));
#ifdef USE_NAMES
                    pValue.set_name("pValue " + std::to_string(i));
#endif
                    if (!pValue) {
                        throw std::runtime_error("Cannot convert arguments");
                    }
                    // pValue reference stolen here
                    PyTuple_SetItem(*args, i, *pValue);
                }
                pValue = PyObject_CallObject(*pFunc, args.get());
#ifdef USE_NAMES
                pValue.set_name("pValue result");
#endif
                python::Object res = pValue;
                if (res) {
                    std::cout << "Result of call: " << PyLong_AsLong(*res) << std::endl;
                } else {
                    PyErr_Print();
                    throw std::runtime_error("Call failed");
                }
            } else {
                if (PyErr_Occurred())
                    PyErr_Print();
                std::clog << "Cannot find function  \"" << argv[2] << '"' << std::endl;
            }
        } else {
            PyErr_Print();
            std::stringstream msg;
            msg << "Failed to load \"" << argv[1] << '"';
            throw std::runtime_error(msg.str());
        }
    }

    if (Py_FinalizeEx() < 0) {
        return 120;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    std::cout << "Hello, World!" << std::endl;
    return low_level_embedding(argc, argv);
}