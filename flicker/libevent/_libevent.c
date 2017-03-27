
#include <Python.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <event2/util.h>
#include <event2/listener.h>


#include "_base.h"
#include "_event.h"
#include "_buffer.h"
#include "_bufferevent.h"



static PyObject *
util_make_socket_nonblocking(PyObject *self, PyObject *args)
{
    evutil_socket_t sock;
    if (!PyArg_ParseTuple(args, "i", &sock))
        return NULL;
    evutil_make_socket_nonblocking(sock);
    Py_RETURN_NONE;

}


PyMethodDef
methods[] = {
    // exported functions
    {"make_socket_nonblocking", (PyCFunction)util_make_socket_nonblocking, METH_VARARGS, NULL},
    {NULL, NULL},
};


static PyModuleDef libevent_module = {
    PyModuleDef_HEAD_INIT,
    "libevent",
    "libevent",
    -1,
    methods, NULL, NULL, NULL, NULL
};


PyMODINIT_FUNC
PyInit_libevent(void)
{
    PyObject* m;

    if (PyType_Ready(&LibEventBaseType) < 0)
        return NULL;

    LibEventType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&LibEventType) < 0)
        return NULL;

    LibBufferType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&LibBufferType) < 0)
        return NULL;

    LibBufferEventType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&LibBufferEventType) < 0)
        return NULL;

    m = PyModule_Create(&libevent_module);

    if (m == NULL)
        return NULL;

    Py_INCREF(&LibEventBaseType);
    PyModule_AddObject(m, "Base", (PyObject *)&LibEventBaseType);

    Py_INCREF(&LibEventType);
    PyModule_AddObject(m, "Event", (PyObject *)&LibEventType);

    Py_INCREF(&LibBufferType);
    PyModule_AddObject(m, "Buffer", (PyObject *)&LibBufferType);

    Py_INCREF(&LibBufferEventType);
    PyModule_AddObject(m, "BufferEvent", (PyObject *)&LibBufferEventType);


    PyModule_AddIntMacro(m, EV_READ);
    PyModule_AddIntMacro(m, EV_WRITE);
    PyModule_AddIntMacro(m, EV_SIGNAL);
    PyModule_AddIntMacro(m, EV_PERSIST);


    PyModule_AddIntMacro(m, BEV_OPT_CLOSE_ON_FREE);
    PyModule_AddIntMacro(m, BEV_OPT_THREADSAFE);
    PyModule_AddIntMacro(m, BEV_OPT_DEFER_CALLBACKS);
    PyModule_AddIntMacro(m, BEV_OPT_UNLOCK_CALLBACKS);

    PyModule_AddIntMacro(m, BEV_EVENT_READING);
    PyModule_AddIntMacro(m, BEV_EVENT_WRITING);
    PyModule_AddIntMacro(m, BEV_EVENT_EOF);
    PyModule_AddIntMacro(m, BEV_EVENT_ERROR);
    PyModule_AddIntMacro(m, BEV_EVENT_TIMEOUT);
    PyModule_AddIntMacro(m, BEV_EVENT_CONNECTED);

    PyModule_AddIntMacro(m, BEV_OPT_CLOSE_ON_FREE);
    PyModule_AddIntMacro(m, BEV_OPT_THREADSAFE);
    PyModule_AddIntMacro(m, BEV_OPT_DEFER_CALLBACKS);
    PyModule_AddIntMacro(m, BEV_OPT_UNLOCK_CALLBACKS);

    PyModule_AddIntMacro(m, EV_RATE_LIMIT_MAX);

    return m;
}
