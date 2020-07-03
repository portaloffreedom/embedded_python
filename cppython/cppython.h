//
// Created by matteo on 7/3/20.
//

#ifndef EMBEDDED_PYTHON_CPPYTHON_H
#define EMBEDDED_PYTHON_CPPYTHON_H

extern "C" {
// This define allows python sequences to be addressable in 64 bits instead of 32
// https://www.python.org/dev/peps/pep-0353/
#define PY_SSIZE_T_CLEAN
//#define Py_REF_DEBUG #I believe we need the debug python version for this
#include <Python.h>
};

#endif //EMBEDDED_PYTHON_CPPYTHON_H
