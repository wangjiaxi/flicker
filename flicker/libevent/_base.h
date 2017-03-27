#include<Python.h>

typedef struct _LibEventBaseObject {
    PyObject_HEAD
    struct event_base *base;
    PyObject *method;
    PyObject *error_type;
    PyObject *error_value;
    PyObject *error_traceback;

}LibEventBaseObject;

extern PyTypeObject LibEventBaseType;
extern void lib_base_store_error(LibEventBaseObject *self);

extern void timeval_init(struct timeval *tv, double time);


