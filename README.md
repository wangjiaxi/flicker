# Flicker
flicker is a Python web framework , based on libevent. using non-blocking network I/O
flicker is Pythonic framework, easily write a web service and code is easy understand

## Hello world
```

from flicker.base import HTTP_200_Code
from flicker.httphandler import HttpHandler
from flicker.web import Application
from flicker.httpserver import HttpServer



web = Application()


@web.route(r"/")
class TestHandler(HttpHandler):
    def get(self, *args, **kwargs):
        self.write(HTTP_200_Code, "<h1>Hello world</h1>")


server = HttpServer("localhost", 8000, web)
server.start()
```


# Flicker have the flies as

### tcpserver.py/httpserver.py
 * TcpServer define a basic tcp server that use libevent

 * HttpServer is subclass of tcp server, listen and deal http request
### httpbase.py


##### HttpRequest
  - headers
  - _cookie
  - path
  - protocol
  - query
  - body

##### HttpResponse
- sock
- protocol
- message
- headers
- _cookie
- address
- status_code

##### HttpConnect
- request
- address
- response
- protocol
- message
- headers
- _cookie

### httphandler.py


##### HttpHandler
- set_header
- get_header
- set_cookie
- get_cookie
- cookies
- get
- post
- ...


### web.py

Application


### libevent
extended the libevent module of c


# install
##### install libevent
`cd flicker/libevent & python setup.py install`
##### install flicker
`python setup.py  install`
 
### contact
<progwong@gmail.com>

