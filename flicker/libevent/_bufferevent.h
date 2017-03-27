#include<Python.h>
#include <structmember.h>
#include <sys/time.h>

#include <event2/event.h>
#include <event2/util.h>


extern PyTypeObject LibBufferEventType;

#define LibBufferEventCheck(ob) ((ob)->ob_type == &LibBufferEventType)
