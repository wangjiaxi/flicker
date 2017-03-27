/*************************************************************************
	> File Name: libevent/utils.c
	> Author: wangjiaxi
	> Mail: progwong@gmail.com 
	> Created Time: 2017年03月21日 星期二 18时21分29秒
 ************************************************************************/

#include<Python.h>
#include <structmember.h>
#include <sys/time.h>
#include <event2/event.h>
#include <event2/util.h>
#include"_base.h"
#include "_bufferevent.h"
typedef struct _LibEventObject {
    PyObject_HEAD
    LibEventBaseObject *base;
    struct event *event;
    PyObject *callback;
    PyObject *userdata;
    PyObject *weakrefs;

    int fd;
} LibEventObject;


static void
lib_event_callback(evutil_socket_t fd, short what, void *userdata)
{
    LibEventObject *self = (LibEventObject *) userdata;

    PyObject *result = PyObject_CallFunction(self->callback, "OiiO", self, fd, what, self->userdata);
    if (result == NULL) {
        lib_base_store_error(self->base);
    } else {
        Py_DECREF(result);
    }
}



/*struct event *event_new(struct event_base *base, evutil_socket_t fd,
    short what, event_callback_fn cb,
    void *arg);
    */
static int
lib_event_init(LibEventObject *self, PyObject *args, PyObject *kwargs){
    LibEventBaseObject *base;
    int fd;
    int what;
    PyObject *callback;
    PyObject *userdata = Py_None;

    // | Indicates that the remaining arguments in the Python argument list are optional.
    if (!PyArg_ParseTuple(args, "O!iiO|O", &LibEventBaseType, &base, &fd, &what, &callback, &userdata))
        return -1;

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "the callback must be callable");
        return -1;
        }

    self->event = event_new(base->base, fd, what, lib_event_callback, self);

    if (self->event == NULL)
        return -1;

    self->base = base;
    Py_INCREF(base); /*  because base is borrowed object */

    self->fd = fd;

    self->callback = callback;
    Py_INCREF(callback);

    self->userdata = userdata;
    Py_INCREF(userdata);

    return 0;
 }


static PyObject *
lib_event_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LibEventObject *s = (LibEventObject *)type->tp_alloc(type, 0);
    if (s != NULL) {
        s->base = NULL;
        s->event = NULL;
        s->callback = NULL;
        s->userdata = NULL;
        s->weakrefs = NULL;

    }
    return (PyObject *)s;
}


/*void event_free(struct event *event);*/
static void
lib_event_clear(LibEventObject *self)
{
   if (self->event != NULL) {
        event_free(self->event);
        self->event = NULL;
    }
    Py_CLEAR(self->callback);
    Py_CLEAR(self->userdata);
    Py_CLEAR(self->base);
}


static void
lib_event_dealloc(LibEventObject *self)
{
    if (self->weakrefs != NULL) {
        PyObject_ClearWeakRefs((PyObject *) self);
    }
    lib_event_clear(self);
    Py_TYPE(self)->tp_free(self);
}

/*int event_add(struct event *ev, const struct timeval *tv);*/
static PyObject *lib_event_add(LibEventObject *self, PyObject *args){
    double time_val = 1;
    struct timeval tv;

    if (!PyArg_ParseTuple(args, "|d", &time_val))
       return NULL;

    timeval_init(&tv, time_val);

    if (time_val > 0)
        event_add(self->event, &tv);
    else
        event_add(self->event, NULL);

    Py_RETURN_NONE;

}

static PyObject *lib_event_del(LibEventObject *self, PyObject *args){

    event_del(self->event);

    Py_RETURN_NONE;

}


/*int event_priority_set(struct event *event, int priority);*/
static int lib_event_priority_set(LibEventObject *self, PyObject *args){
    int priority;
    if(!PyArg_ParseTuple(args, "i", &priority))
        return -1;

    if (event_priority_set(self->event, priority) < 0 )
        return -1;

    return 0;
}


/*int event_pending(const struct event *ev, short what, struct timeval *tv_out);*/
static int lib_event_pending(LibEventObject *self, PyObject *args){
    int what, result;
    float time_val = -1;
    struct timeval tv;

    /* time_val is optional argument*/
    if(!PyArg_ParseTuple(args, "i|d", &what, &time_val))
        return -1;

    timeval_init(&tv, time_val);

    if (time_val > 0){
        result = event_pending(self->event, what, &tv);
    }else{
        result = event_pending(self->event, what, &tv);
    }

    if(result < 0)
        return -1;
    return 0;
}


static PyMethodDef lib_event_methods[] = {
    {"delete", (PyCFunction)lib_event_del, METH_NOARGS,
     "delete the event"},
    {"add", (PyCFunction)lib_event_add, METH_VARARGS,
     "add the event"},
    {"set_priority", (PyCFunction)lib_event_priority_set, METH_VARARGS,
     "set the event priority"},
    {"pending", (PyCFunction)lib_event_pending, METH_VARARGS,
     "check event status is pending"},
    {NULL},
};



static PyMemberDef lib_event_members[] = {
    {"fd", T_INT, offsetof(LibEventObject, fd), READONLY,
     "socket fileno that event bind "},
    {"base", T_OBJECT, offsetof(LibEventObject, base), READONLY,
     "the base this event is assigned to"},
    {"callback", T_OBJECT, offsetof(LibEventObject, callback), READONLY,
     "the callback to execute"},
    {"userdata", T_OBJECT, offsetof(LibEventObject, userdata), READONLY,
     "the userdata to pass to callback"},
    {NULL}
};


PyTypeObject LibEventType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "event.Event",         /* tp_name */
    sizeof(LibEventObject), /* tp_basicsize */
    0,                      /* tp_itemsize */
    (destructor)lib_event_dealloc, /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC|Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "libevent base",        /* tp_doc */
    0,                    /* tp_traverse */
    0,                    /* tp_clear */
    0,                    /* tp_richcompare */
    offsetof(LibEventObject, weakrefs),                    /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    lib_event_methods,       /* tp_methods */
    lib_event_members,       /* tp_members */
    0,                    /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,                    /* tp_descr_get */
    0,                    /* tp_descr_set */
    0,                    /* tp_dictoffset */
    (initproc)lib_event_init,  /* tp_init */
    0,                    /* tp_alloc */
    lib_event_new,           /* tp_new */
};


