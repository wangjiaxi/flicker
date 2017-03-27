/*************************************************************************
	> File Name: _base.c
	> Author: wangjiaxi
	> Mail: progwang@gmail.com
	> Created Time: 2017年03月23日 星期四 09时11分01秒
 ************************************************************************/

#include <Python.h>
#include <structmember.h>

#include <event2/event.h>
#include <event2/util.h>

#include "_base.h"


void
timeval_init(struct timeval *tv, double time)
{
    tv->tv_sec = (time_t) time;
    tv->tv_usec = (suseconds_t) ((time - tv->tv_sec) * 1000000);
}


void
lib_base_store_error(LibEventBaseObject *self)
{
    if (self->error_type == NULL) {
        // Store exception for later reuse and signal loop to stop
        PyErr_Fetch(&self->error_type, &self->error_value, &self->error_traceback);
        event_base_loopbreak(self->base);
    }
}


static PyObject *
lb_event_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    LibEventBaseObject *self = (LibEventBaseObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->base = NULL;
        self->method = NULL;
        self->error_type = NULL;
        self->error_value = NULL;
        self->error_traceback = NULL;
    }
    return (PyObject *)self;
}

static int
lb_base_event_init(LibEventBaseObject *self, PyObject *args, PyObject *kwds)
{

    self->base = event_base_new();
    if (self->base == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    return 0;
}

/*int event_base_priority_init(struct event_base *base, int n_priorities);*/
static PyObject *
lb_event_base_priority_init(LibEventBaseObject *self, PyObject *args){
    int priority;

    if(!PyArg_ParseTuple(args, "i", &priority))
        return NULL;

    event_base_priority_init(self->base, priority);
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *
lb_event_base_reinit(LibEventBaseObject *self, PyObject *args){
    event_reinit(self->base);
    Py_RETURN_NONE;

}


static PyObject *
lib_base_evalute_error_response(LibEventBaseObject *self)
{
    if (self->error_type != NULL) {
        // The loop was interrupted due to an error, re-raise
        PyErr_Restore(self->error_type, self->error_value, self->error_traceback);
        self->error_type = NULL;
        self->error_value = NULL;
        self->error_traceback = NULL;
        return NULL;
    }
    Py_RETURN_NONE;
}


/*int event_base_loop(struct event_base *base, int flags); */
static PyObject *lib_event_base_loop(LibEventBaseObject *self, PyObject *args){
    int flags=0;

    if(!PyArg_ParseTuple(args, "|i", &flags))
        return NULL;

    event_base_loop(self->base, flags);

    return lib_base_evalute_error_response(self);

}


/*int event_base_dispatch(struct event_base *base);*/

static PyObject  *
lib_event_base_dispatch(LibEventBaseObject *self, PyObject *args){
    event_base_dispatch(self->base);
    return lib_base_evalute_error_response(self);
}

static void
lb_base_dealloc(LibEventBaseObject *self)
{
    Py_CLEAR(self->base);
    Py_CLEAR(self->error_type);
    Py_CLEAR(self->error_value);
    Py_CLEAR(self->error_traceback);
    Py_CLEAR(self->method);
    if (self->base!=NULL)
        event_base_free(self->base);

    Py_TYPE(self)->tp_free(self);
}


/*int event_base_loopbreak(struct event_base *base); */
static PyObject*
lib_event_base_loopbreak(LibEventBaseObject *self, PyObject *args){
    event_base_loopbreak(self->base);
    Py_RETURN_NONE;

}


static PyMethodDef lb_base_methods[] = {
    {"loop", (PyCFunction)lib_event_base_loop, METH_VARARGS,
     "loop"},
    {"reinit", (PyCFunction)lb_event_base_reinit, METH_NOARGS,
     "reinit"},
    {"priority_init", (PyCFunction)lb_event_base_priority_init, METH_VARARGS,
     "priority"},
    {"dispatch", (PyCFunction)lib_event_base_dispatch, METH_NOARGS,
     "dispatch"},
    {"loopbreak", (PyCFunction)lib_event_base_loopbreak, METH_NOARGS,
     "lookbreak"},
    {NULL},
};



static PyMemberDef lb_base_members[] = {
    {NULL},
};

PyTypeObject LibEventBaseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "event.Base",         /* tp_name */
    sizeof(LibEventBaseObject), /* tp_basicsize */
    0,                    /* tp_itemsize */
    (destructor)lb_base_dealloc, /* tp_dealloc */
    0,                    /* tp_print */
    0,                    /* tp_getattr */
    0,                    /* tp_setattr */
    0,                    /* tp_reserved */
    0,                    /* tp_repr */
    0,                    /* tp_as_number */
    0,                    /* tp_as_sequence */
    0,                    /* tp_as_mapping */
    0,                    /* tp_hash */
    0,                    /* tp_call */
    0,                    /* tp_str */
    0,                    /* tp_getattro */
    0,                    /* tp_setattro */
    0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,   /* tp_flags */
    "libevent base event",  /* tp_doc */
    0,                    /* tp_traverse */
    0,                    /* tp_clear */
    0,                    /* tp_richcompare */
    0,                    /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    lb_base_methods,       /* tp_methods */
    lb_base_members,       /* tp_members */
    0,                    /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,                    /* tp_descr_get */
    0,                    /* tp_descr_set */
    0,                    /* tp_dictoffset */
    (initproc)lb_base_event_init,  /* tp_init */
    0,                    /* tp_alloc */
    lb_event_new,           /* tp_new */
};
