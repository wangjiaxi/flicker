# flicker
python web server, based on libevent

easy create a http server
```
# hello world
from flicker.base import HTTP_200_Code
from flicker.httphandler import HttpHandler
from flicker.web import Application
from flicker.httpserver import HttpServer




web = Application()


@web.route("/")
class TestHandler(HttpHandler):
    def get(self, *args, **kwargs):
        print("test get method \n")
        self.write(HTTP_200_Code, "<h1>hello world</h1>")


server = HttpServer("localhost", 8000, web)
server.start()
```

