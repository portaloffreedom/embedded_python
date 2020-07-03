//
// Created by matteo on 7/3/20.
//

#ifndef EMBEDDED_PYTHON_CPPYOBJECT_H
#define EMBEDDED_PYTHON_CPPYOBJECT_H

#include "cppython.h"
#include <string>
#include <iostream>

#define USE_NAMES 1

/* Whenever an object reference is passed into or out of a function, it is part of the function’s interface
 * specification whether ownership is transferred with the reference or not.
 *
 * Most functions that return a reference to an object pass on ownership with the reference. In particular, all
 * functions whose function it is to create a new object, such as PyLong_FromLong() and Py_BuildValue(), pass ownership
 * to the receiver. Even if the object is not actually new, you still receive ownership of a new reference to that
 * object. For instance, PyLong_FromLong() maintains a cache of popular values and can return a reference to a cached
 * item.
 *
 * Many functions that extract objects from other objects also transfer ownership with the reference, for instance
 * PyObject_GetAttrString(). The picture is less clear, here, however, since a few common routines are exceptions:
 * PyTuple_GetItem(), PyList_GetItem(), PyDict_GetItem(), and PyDict_GetItemString() all return references that you
 * borrow from the tuple, list or dictionary.
 *
 * The function PyImport_AddModule() also returns a borrowed reference, even though it may actually create the object it
 * returns: this is possible because an owned reference to the object is stored in sys.modules.
 *
 * When you pass an object reference into another function, in general, the function borrows the reference from you — if
 * it needs to store it, it will use Py_INCREF() to become an independent owner. There are exactly two important
 * exceptions to this rule: PyTuple_SetItem() and PyList_SetItem(). These functions take over ownership of the item
 * passed to them — even if they fail! (Note that PyDict_SetItem() and friends don’t take over ownership — they are
 * “normal.”)
 *
 * When a C function is called from Python, it borrows references to its arguments from the caller. The caller owns a
 * reference to the object, so the borrowed reference’s lifetime is guaranteed until the function returns. Only when
 * such a borrowed reference must be stored or passed on, it must be turned into an owned reference by calling
 * Py_INCREF().
 *
 * The object reference returned from a C function that is called from Python must be an owned reference — ownership is
 * transferred from the function to its caller.
 *
 * TODO PyTuple_GetItem(), PyList_GetItem(), PyDict_GetItem(), and PyDict_GetItemString() return not owned references,
 * need to increase [increase reference when retrieving]
 *
 * TODO PyTuple_SetItem() and PyList_SetItem(). These functions take over ownership of the item
 * passed to them — even if they fail! (Note that PyDict_SetItem() and friends don’t take over ownership — they are
 * “normal.”) [increase reference before inserting? only if you want to keep a local copy]
*/

namespace python {

class Object {
    PyObject *object;
#ifdef USE_NAMES
    std::string name;
#endif
public:
    Object()
        : object(nullptr)
    {}
    Object(PyObject *object)
            : object(object)
    {}

#ifdef USE_NAMES
    Object(PyObject *object, std::string name)
            : object(object), name(std::move(name))
    {}
#endif

    Object(const Object &other)
            : object(other.object)
#ifdef USE_NAMES
            , name(other.name)
#endif
    {
        increase_ref();
    }

    Object(Object &&other) noexcept
            : object(other.object)
#ifdef USE_NAMES
            , name(std::move(other.name))
#endif
    {
        if (this != &other)
            other.object = nullptr;
    }

    Object& operator=(const Object &other)
    {
        if (this != &other)
        {
            decrease_ref();
            object = other.object;
#ifdef USE_NAMES
            name = other.name;
#endif
            increase_ref();
        }
        return *this;
    }

    Object& operator=(Object &&other) noexcept
    {
        if (this != &other)
        {
            decrease_ref();
            object = other.object;
#ifdef USE_NAMES
            name = std::move(other.name);
#endif
            other.object = nullptr;
        }
        return *this;
    }

    ~Object()
    {
        decrease_ref();
    }

    bool operator!() const
    {
        return object == nullptr;
    }
    explicit operator bool() const
    {
        return object != nullptr;
    }

    PyObject *operator*() const
    {
        return object;
    }

    [[nodiscard]] PyObject *get() const
    {
        return object;
    }

#ifdef USE_NAMES
    void set_name(std::string p_name)
    {
        this->name = std::move(p_name);
    }
#endif

private:
    void increase_ref() {
#ifdef USE_NAMES
        if (object != nullptr)
            std::cout << "Incrementing object " << name << " to value " << object->ob_refcnt+1 << std::endl;
#endif
        Py_XINCREF(object);
    }
    void decrease_ref() {
#ifdef USE_NAMES
        if (object != nullptr)
            std::cout << "Decrementing object " << name << " to value " << object->ob_refcnt-1 << std::endl;
#endif
        Py_XDECREF(object);
    }
};

}


#endif //EMBEDDED_PYTHON_CPPYOBJECT_H
