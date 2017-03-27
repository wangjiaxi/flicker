
typedef struct
_LibBufferObject{
    PyObject_HEAD;
    LibEventBaseObject *base;
    struct evbuffer *buffer;
}LibBufferObject;


extern PyTypeObject LibBufferType;
extern LibBufferObject *lib_buffer_create(struct evbuffer *buffer);
#define LibBufferCheck(ob) ((ob)->ob_type == &LibBufferType)

