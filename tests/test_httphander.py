import unittest
import re
from http import cookies
from flicker.web import Application
from flicker.httphandler import HttpHandler
from flicker.httpbase import HttpRequest
from flicker.base import HTTP_200_Code


class HttpHandlerTestCase(unittest.TestCase):
    def setUp(self):
        self.request = HttpRequest("GET", "/", "HTTP/1.0",
                                   {}, cookies.SimpleCookie(), "")
        self.path = r"/"

        @self.web.route(self.path)
        class TestHandler(HttpHandler):
            def get(self, *args, **kwargs):
                self.write(HTTP_200_Code, "<h1>hello world</h1>")

    def test_request_cookie(self):
        self.request.set_cookie("keyword", "78asdf9qy-f&89&F")
        self.assertEqual(self.request._cookies,
                         "Set-Cookie: keyword=78asdf9qy-f&89&F", "set cookies error")

    def test_request_headers(self):
        self.request.set_headers("Accept", "json")
        self.assertIn("Accept", self.request.headers.keys(), "add header error")
        self.request.remove_headers("Accept", "json")
        self.assertNotIn("Accept", self.request.headers.keys(), "delete header error")
