import unittest
import re
from flicker.web import Application
from flicker.httphandler import HttpHandler
from flicker.base import HTTP_200_Code


class AppliCationTestCase(unittest.TestCase):
    def setUp(self):
        self.web = Application()
        self.path = r"/"

        @self.web.route(self.path)
        class TestHandler(HttpHandler):
            def get(self, *args, **kwargs):
                self.write(HTTP_200_Code, "<h1>hello world</h1>")

    def test_application_add_handler(self):

        handlers = self.web.handers
        self.assertEqual(1, len(handlers), "wrong size handlers")
        self.assertIn(re.compile(self.path), [handler[0] for handler in handlers])

    def test_get_handler_class(self):
        request = type("object", (), {"path": "/"})
        klass = self.web.get_handler_class(request)
        self.assertIsNotNone(klass, "handler class is None")

