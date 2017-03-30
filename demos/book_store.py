import sys
import os
sys.path.append(os.path.abspath(".."))

from flicker.base import HTTP_200_Code, HTTP_404_Code
from flicker.httphandler import HttpHandler
from flicker.web import Application
from flicker.httpserver import HttpServer


web = Application()

STORE_NAME = "Flicker"

BOOK_INFO = {
        '8978': {
            "name": "Go For It",
            "author": "ProgWong",
            "price": 19.88,
            "currency": "￥"
        },
        '8979': {
            "name": "Go For It",
            "author": "ProgWong",
            "price": 19.88,
            "currency": "￥"
        }

}


@web.route(r"^/$")
class HostHandler(HttpHandler):
    def get(self, *args, **kwargs):
        self.write(HTTP_200_Code, "<h1>Wellcome to %s Book Store</h1>" % STORE_NAME)


@web.route(r"^/book/(?P<id>\d+)/$")
class BookInfoHandler(HttpHandler):
    def get(self, *args, **kwargs):
        print(args, kwargs)
        book_id = self.request.kwargs.get("id")
        book_info = BOOK_INFO.get(book_id)
        if not book_info:
            return self.write(HTTP_404_Code, "The Book Not Found!")
        return self.write(HTTP_200_Code, "book name: %s\n book_author: %s\n book price %d\n" %
                          (book_info["name"], book_info["author"], book_info["price"]))

server = HttpServer("localhost", 8002, web)
server.start()

