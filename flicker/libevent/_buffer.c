#include<Python.h>
#include <structmember.h>

#include <event2/buffer.h>

#include "_base.h"
#include "_buffer.h"
#include <sys/socket.h>

static PyObject *
lib_buffer_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LibBufferObject *s = (LibBufferObject *)type->tp_alloc(type, 0);
    if (s != NULL) {
        s->base = NULL;
        s->buffer = NULL;
    }
    return (PyObject *)s;
}

LibBufferObject *
lib_buffer_create(struct evbuffer *buffer)
{

    LibBufferObject *result = (LibBufferObject *)LibBufferType.tp_alloc(&LibBufferType, 0);
    if (result == NULL) {
        return NULL;
    }

    result->buffer = buffer;
    result->base = NULL;
    return result;
}

static int
lib_buffer_init(LibBufferObject  *self, PyObject *args, PyObject *kwds){
    self->buffer = evbuffer_new();

    if (self->buffer == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    return 0;

}


/*void event_free(struct event *event);*/
static void
lib_buffer_clear(LibBufferObject *self)
{
   if (self->buffer != NULL) {
        evbuffer_free(self->buffer);
        self->buffer = NULL;
    }
    Py_CLEAR(self->base);
}


static void
lib_buffer_dealloc(LibBufferObject *self, PyObject *args)
{
    lib_buffer_clear(self);
    Py_TYPE(self)->tp_free(self);
}


static PyObject *
lib_buffer_lock(LibBufferObject *self, PyObject *args){
    evbuffer_lock(self->buffer);
    Py_RETURN_NONE;
}


static PyObject *
lib_buffer_unlock(LibBufferObject *self, PyObject *args){
    evbuffer_unlock(self->buffer);
    Py_RETURN_NONE;
}


/*evbuffer_get_contiguous_space(const struct evbuffer *buf);*/
static int
get_buffer_length(LibBufferObject *self, PyObject *args){
    size_t length;
    length = evbuffer_get_length(self->buffer);
    return PyLong_FromSize_t(length);
}


static PyObject *
lib_buffer_add(LibBufferObject *self, PyObject *args){
    PyObject *data;
    char *buf;
    Py_ssize_t length;

    if (!PyArg_ParseTuple(args, "O", &data))
        Py_RETURN_NONE;

    if (PyObject_AsReadBuffer(data, (const void **) &buf, &length) != 0) {
            Py_RETURN_NONE;
        }
    evbuffer_add(self->buffer, buf, length);
    Py_RETURN_NONE;
}


static int
lib_buffer_add_to(LibBufferObject *self, PyObject *args){
    int result;
    PyObject *dst;

    if (!PyArg_ParseTuple(args, "O", &dst))
        return -1;

    if (LibBufferCheck(dst)) {
        result = evbuffer_add_buffer(self->buffer, ((LibBufferObject *) dst)->buffer);
        return result;
        }
    return -1;
}



static PyObject*
lib_buffer_remove(LibBufferObject *self, PyObject *args)
{
    char *data;
    Py_ssize_t size;
    Py_ssize_t length=-1;
    PyObject *result;

    if (!PyArg_ParseTuple(args, "|n", &length))
        return NULL;

    if (length == -1) {
        length = evbuffer_get_length(self->buffer);
    } else if (length < 0) {
        PyErr_Format(PyExc_TypeError, "can't read %d bytes from the buffer", (int) length);
        return NULL;
    }

    if (length == 0) {
        return PyBytes_FromString("");
    }

    result = PyBytes_FromStringAndSize(NULL, length);
    if (result == NULL) {
        return PyErr_NoMemory();
    }

    data = PyBytes_AS_STRING(result);
    size = evbuffer_remove(self->buffer, data, length);

    if (size < 0) {
        PyErr_SetString(PyExc_TypeError, "could not remove data from buffer");
        result = NULL;
    } else if (size == 0) {
        result =PyBytes_FromString("");
    } else if (size != length) {
        _PyBytes_Resize(&result, size);
    }
    return result;
}

static PyObject*
lib_buffer_drain(LibBufferObject *self, PyObject *args)
{
    size_t size;

    if (!PyArg_ParseTuple(args, "|n", &size))
        return NULL;

    if (evbuffer_drain(self->buffer, size) < 0) {
        PyErr_SetString(PyExc_TypeError, "could not remove data from buffer");
        return NULL;
    }else{
        Py_RETURN_NONE;
    }
}


static PyObject *
lib_buffer_readln(LibBufferObject *self, PyObject *args)
{
    int flags=EVBUFFER_EOL_ANY;
    char *data;
    size_t size;
    PyObject *result;

    if (!PyArg_ParseTuple(args, "i|i", &size, &flags))
        Py_RETURN_NONE;

    data = evbuffer_readln(self->buffer, &size, flags);
    if (data == NULL) {
        result = PyBytes_FromString("");
    } else {
        result = PyBytes_FromStringAndSize(data, size);
        free(data);
    }
    return result;
}


static PySequenceMethods
lib_buffer_as_seq = {
    (lenfunc)get_buffer_length,  /*sq_length*/
    NULL,  /*sq_concat*/
    NULL,  /*sq_repeat*/
    NULL,  /*sq_item*/
    NULL,  /*sq_slice*/
    NULL,  /*sq_ass_item*/
    NULL,  /*sq_ass_slice*/
    NULL,  /*sq_contains*/
    NULL,  /*sq_inplace_concat*/
    NULL   /*sq_inplace_repeat*/
};


static PyMappingMethods
lib_buffer_as_mapping = {
	(lenfunc)get_buffer_length,  /*mp_length*/
	NULL,  /*mp_subscript*/
	NULL,  /*mp_ass_subscript*/
};



static PyMethodDef lib_buffer_methods[] = {
    {"length", (PyCFunction)get_buffer_length, METH_NOARGS,
     "get buffer's length"},

    {"add_to", (PyCFunction)lib_buffer_add_to, METH_VARARGS,
     "add buffer to another buffer"},

    {"lock", (PyCFunction)lib_buffer_lock, METH_NOARGS,
     "lock the buffer"},

    {"unlock", (PyCFunction)lib_buffer_unlock, METH_NOARGS,
     "unlock the buffer"},

    {"add", (PyCFunction)lib_buffer_add, METH_VARARGS,
     "add buffer"},

    {"remove", (PyCFunction)lib_buffer_remove, METH_VARARGS,
     "buffer remove \n:args: length"},

    {"drain", (PyCFunction)lib_buffer_drain, METH_VARARGS,
     "buffer drain \n:args: length"},

    {"readln", (PyCFunction)lib_buffer_readln, METH_VARARGS,
     "buffer read line"},
    {NULL},
};



static PyMemberDef lib_buffer_members[] = {
    {"base", T_OBJECT, offsetof(LibBufferObject, base), READONLY,
     "the base"},
    {NULL}
};


PyTypeObject LibBufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "event.Buffer",         /* tp_name */
    sizeof(LibBufferObject), /* tp_basicsize */
    0,                      /* tp_itemsize */
    (destructor)lib_buffer_dealloc, /* tp_dealloc */
    0,                    /* tp_print */
    0,                    /* tp_getattr */
    0,                    /* tp_setattr */
    0,                    /* tp_reserved */
    0,                    /* tp_repr */
    0,                    /* tp_as_number */
    &lib_buffer_as_seq,     /* tp_as_sequence */
    &lib_buffer_as_mapping,                    /* tp_as_mapping */
    0,                    /* tp_hash */
    0,                    /* tp_call */
    0,                    /* tp_str */
    0,                    /* tp_getattro */
    0,                    /* tp_setattro */
    0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "libevent buffer",  /* tp_doc */
    0,                    /* tp_traverse */
    0,                    /* tp_clear */
    0,                    /* tp_richcompare */
    0,                    /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    lib_buffer_methods,       /* tp_methods */
    lib_buffer_members,       /* tp_members */
    0,                    /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,                    /* tp_descr_get */
    0,                    /* tp_descr_set */
    0,                    /* tp_dictoffset */
    (initproc)lib_buffer_init,  /* tp_init */
    0,                    /* tp_alloc */
    lib_buffer_new,           /* tp_new */
};


