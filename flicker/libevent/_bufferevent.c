/*************************************************************************
	> File Name: libevent/utils.c
	> Author: wangjiaxi
	> Mail: progwong@gmail.com 
	> Created Time: 2017年03月21日 星期二 18时21分29秒
 ************************************************************************/

#include<Python.h>
#include <structmember.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "_base.h"
#include "_buffer.h"
#include "_bufferevent.h"

typedef struct _LibBufferEventObject{
    PyObject_HEAD
    struct bufferevent *bev;
    PyObject *read_callback;
    PyObject *write_callback;
    PyObject *event_callback;
    PyObject *userdata;
    LibBufferObject *input;
    LibBufferObject *output;
    LibEventBaseObject *base;
    PyObject *weakrefs;
    int fd;
}LibBufferEventObject;


static void
event_read_callback(struct bufferevent *bev, void *ptr)
{
    /*(struct bufferevent *bev, void *ptr)*/
    LibBufferEventObject *self = (LibBufferEventObject *)ptr;

    if (self->read_callback != NULL){
        PyObject *result = PyObject_CallFunction(self->read_callback, "OO", self, self->userdata);
        if (result == NULL) {
            lib_base_store_error(self->base);
        } else {
            Py_DECREF(result);
        }
    }
}


static void
event_write_callback
(struct bufferevent *bev, void *ptr)
{
    LibBufferEventObject *self = (LibBufferEventObject *)ptr;
    if (self->write_callback != NULL){
        PyObject *result = PyObject_CallFunction(self->write_callback, "OO", self, self->userdata);
        if (result == NULL) {
            lib_base_store_error(self->base);
        } else {
            Py_DECREF(result);
        }
    }
}


static void
event_event_callback
(struct bufferevent *bev, short events, void *ptr)
{
    LibBufferEventObject *self = (LibBufferEventObject *)ptr;
    if (self->event_callback != NULL){
        PyObject *result = PyObject_CallFunction(self->event_callback, "OiO", self, events, self->userdata);
        if (result == NULL) {
            lib_base_store_error(self->base);
        } else {
            Py_DECREF(result);
        }
    }
}


static int
lib_buffer_event_init(LibBufferEventObject *self, PyObject *args, PyObject *kwds){

    LibEventBaseObject *base;
    int fd;
    int options=0;
    if (!PyArg_ParseTuple(args, "O!i|i", &LibEventBaseType, &base, &fd, &options))
        return -1;

    self->bev = bufferevent_socket_new(base->base, fd, options);
    if(self->bev == NULL){
        PyErr_NoMemory();
        return -1;
    }

    self->fd = fd;
    self->base = base;
    Py_INCREF(base);

    self->userdata = Py_None;
    Py_INCREF(Py_None);

    self->input=lib_buffer_create(bufferevent_get_input(self->bev));
    self->output=lib_buffer_create(bufferevent_get_output(self->bev));
    return 0;
}


static PyObject*
lib_buffer_event_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    LibBufferEventObject *s;
    s = (LibBufferEventObject *)type->tp_alloc(type, 0);
    if (s != NULL) {
        s->base = NULL;
        s->bev = NULL;
        s->userdata = NULL;
        s->input = NULL;
        s->output = NULL;
        s->read_callback = NULL;
        s->write_callback = NULL;
        s->event_callback = NULL;
        s->weakrefs = NULL;

    }
    return (PyObject *)s;
}


//evutil_make_socket_nonblocking(evutil_socket_t fd)

static int
lib_bufferevent_traverse(LibBufferEventObject *self, visitproc visit, void *arg)
{
    Py_VISIT(self->input);
    Py_VISIT(self->output);
    Py_VISIT(self->read_callback);
    Py_VISIT(self->write_callback);
    Py_VISIT(self->event_callback);
    Py_VISIT(self->userdata);
    Py_VISIT(self->base);
    return 0;
}


static void
lib_buffer_event_clear(LibBufferEventObject *self)
{
   if (self->bev != NULL) {
        bufferevent_free(self->bev);
        self->bev = NULL;
    }
    if (self->input != NULL) {
        self->input->buffer = NULL;
        Py_CLEAR(self->input);
    }
    if (self->output != NULL) {
        self->output->buffer = NULL;
        Py_CLEAR(self->output);
    }

    Py_CLEAR(self->base);
    Py_CLEAR(self->read_callback);
    Py_CLEAR(self->write_callback);
    Py_CLEAR(self->event_callback);
    Py_CLEAR(self->userdata);
}


static void
lib_buffer_event_dealloc(LibBufferEventObject *self)
{
if (self->weakrefs != NULL) {
        PyObject_ClearWeakRefs((PyObject *) self);
    }
    lib_buffer_event_clear(self);
    Py_TYPE(self)->tp_free(self);
}


static PyObject *
lib_buffer_event_set_callback(LibBufferEventObject *self, PyObject *args){
   /*
    void bufferevent_setcb(struct bufferevent *bufev,
    bufferevent_data_cb readcb, bufferevent_data_cb writecb,
    bufferevent_event_cb eventcb, void *cbarg);
   */

    PyObject *read_callback;
    PyObject *write_callback;
    PyObject *event_callback;
    PyObject *userdata=Py_None;

    bufferevent_lock(self->bev);

    if (!PyArg_ParseTuple(args, "OOO|O", &read_callback, &write_callback,
     &event_callback, &userdata))
        return NULL;

    if (self->read_callback != read_callback) {
        PyObject *old = self->read_callback;
        self->read_callback = (read_callback == Py_None ? NULL : read_callback);
        Py_XINCREF(self->read_callback);
        Py_XDECREF(old);
    }


    if (self->write_callback != write_callback) {
        PyObject *old = self->write_callback;
        self->write_callback = (write_callback == Py_None ? NULL : write_callback);
        Py_XINCREF(self->write_callback);
        Py_XDECREF(old);
     }


    if (self->event_callback != event_callback) {
        PyObject *old = self->event_callback;
        self->event_callback = (event_callback == Py_None ? NULL : event_callback);
        Py_XINCREF(self->event_callback);
        Py_XDECREF(old);
    }

    if (self->userdata != userdata) {
        PyObject *old = self->userdata;
        self->userdata = userdata;
        Py_INCREF(userdata);
        Py_XDECREF(old);
    }

    bufferevent_setcb(
    self->bev,
    read_callback == Py_None ? NULL : event_read_callback,
    write_callback == Py_None ? NULL : event_write_callback,
    event_callback == Py_None ? NULL : event_event_callback,
    self);

    bufferevent_unlock(self->bev);
    Py_RETURN_NONE;
}


/*void bufferevent_setwatermark(struct bufferevent *bufev, short events,
    size_t lowmark, size_t highmark);*/
static PyObject*
lib_buffer_event_set_watermark(LibBufferEventObject *self, PyObject*args)
{

    size_t highmark=0, lowmark=0;
    short events;
    if (!PyArg_ParseTuple(args, "iii", &events, &lowmark, &highmark))
        return NULL;
    bufferevent_setwatermark(self->bev, events, lowmark, highmark);
    Py_RETURN_NONE;

}


static PyObject *
lib_buffer_event_enable(LibBufferEventObject *self, PyObject *args)
{
    int what;
    if (!PyArg_ParseTuple(args, "i", &what))
        return NULL;
    bufferevent_enable(self->bev, what);
    Py_RETURN_NONE;
}


static PyObject *
lib_buffer_event_set_timeouts(LibBufferEventObject *self, PyObject *args)
{
    double read;
    double write;
    struct timeval read_tv;
    struct timeval write_tv;

    if (!PyArg_ParseTuple(args, "dd", &read, &write))
        return NULL;

    timeval_init(&read_tv, read);
    timeval_init(&write_tv, write);

    bufferevent_set_timeouts(self->bev,
        read <= 0 ? NULL : &read_tv,
        write <= 0 ? NULL : &write_tv);
    Py_RETURN_NONE;
}

static PyObject *
lib_buffer_event_disable(LibBufferEventObject *self, PyObject *args)
{
    int what;
    if (!PyArg_ParseTuple(args, "i", &what))
        return NULL;
    bufferevent_disable(self->bev, what);
    Py_RETURN_NONE;
}


static PyObject*
lib_buffer_event_read(LibBufferEventObject *self, PyObject *args)
{
    size_t length=1234;
    length = evbuffer_get_length(bufferevent_get_input(self->bev));
    return PyLong_FromSize_t(length);
}


static PyObject *
lib_buffer_event_lock(LibBufferEventObject *self, PyObject *args)
{
    bufferevent_lock(self->bev);
    Py_RETURN_NONE;
}


static PyObject *
lib_buffer_event_unlock(LibBufferEventObject *self, PyObject *args)
{
    bufferevent_unlock(self->bev);
    Py_RETURN_NONE;
}


static PyObject *
lib_buffer_event_write(LibBufferEventObject *self, PyObject *args)
{
    PyObject *data;
    char *buf;
    Py_ssize_t length;

    if (!PyArg_ParseTuple(args, "O", &data))
        Py_RETURN_NONE;

    if (PyObject_AsReadBuffer(data, (const void **) &buf, &length) != 0) {
            Py_RETURN_NONE;
        }
    bufferevent_write(self->bev, buf , length);
    Py_RETURN_NONE;
}

static PyMethodDef lib_buffer_event_methods[] = {
    {"disable", (PyCFunction)lib_buffer_event_disable, METH_VARARGS,
     "disabled the buffer event"},

    {"__exit__", (PyCFunction)lib_buffer_event_unlock, METH_NOARGS,
     "disabled the buffer event"},

    {"unlock", (PyCFunction)lib_buffer_event_unlock, METH_NOARGS,
     "disabled the buffer event"},

     {"lock", (PyCFunction)lib_buffer_event_lock, METH_NOARGS,
     "disabled the buffer event"},

     {"__enter__", (PyCFunction)lib_buffer_event_lock, METH_NOARGS,
     "disabled the buffer event"},

    {"enable", (PyCFunction)lib_buffer_event_enable, METH_VARARGS,
     "enable the buffer event"},

    {"set_watermark", (PyCFunction)lib_buffer_event_set_watermark, METH_VARARGS,
     "set buffer event water mark\n :params int event,\n int lowermark \n int highmark ; "},

    {"set_callback", (PyCFunction)lib_buffer_event_set_callback, METH_VARARGS,
     "set callback. read_callback, write callback, eventcallback"},

    {"read", (PyCFunction)lib_buffer_event_read, METH_NOARGS, "read"},

    {"write", (PyCFunction)lib_buffer_event_write, METH_VARARGS, "write"},

    {"set_timeouts", (PyCFunction)lib_buffer_event_set_timeouts, METH_VARARGS, "set timeouts"},

    {NULL},
};


static PyMemberDef lib_buffer_event_members[] = {
    {"fd", T_INT, offsetof(LibBufferEventObject, fd), READONLY, "socket fileno that event bind "},
    {"event_callback", T_INT, offsetof(LibBufferEventObject, event_callback), READONLY, "call back"},
    {"write_callback", T_OBJECT, offsetof(LibBufferEventObject, write_callback), READONLY, "write callback"},
    {"read_callback", T_OBJECT, offsetof(LibBufferEventObject, read_callback), READONLY, "read callback "},
    {"input", T_OBJECT, offsetof(LibBufferEventObject, input), READONLY, "input"},
    {"output", T_OBJECT, offsetof(LibBufferEventObject, output), READONLY, "the base"},
    {"userdata", T_OBJECT, offsetof(LibBufferEventObject, userdata), READONLY, "the userdata"},
    {"base", T_OBJECT, offsetof(LibBufferEventObject, base), READONLY, "event base"},
    {NULL}
};


PyTypeObject LibBufferEventType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "event.BufferEvent",         /* tp_name */
    sizeof(LibBufferEventObject), /* tp_basicsize */
    0,                      /* tp_itemsize */
    (destructor)lib_buffer_event_dealloc, /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC|Py_TPFLAGS_BASETYPE,  /* tp_flags */
   "libevent buffer event\n initiate that Object need to items,\n"
    "BaseEvent instance and socket fd",  /* tp_doc */
    (traverseproc)lib_bufferevent_traverse,                           /* tp_traverse */
    (inquiry)lib_buffer_event_clear, /* tp_clear */
    0,                    /* tp_richcompare */
    offsetof(LibBufferEventObject, weakrefs),  /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    lib_buffer_event_methods,       /* tp_methods */
    lib_buffer_event_members,       /* tp_members */
    0,                    /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,                    /* tp_descr_get */
    0,                    /* tp_descr_set */
    0,                    /* tp_dictoffset */
    (initproc)lib_buffer_event_init,  /* tp_init */
    0,                    /* tp_alloc */
    lib_buffer_event_new,           /* tp_new */
};


