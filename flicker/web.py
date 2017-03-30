import re
import sys
from functools import wraps

from flicker.exception import HTTPError


class Application(object):

    def __init__(self):
        self.handers = []

    def route(self, path):
        def decorator(klass):
            _compile = re.compile(path)
            self.add_handler((_compile, klass))
            return klass
        return decorator

    def add_handler(self, hander):
        self.handers.append(hander)

    def get_handler_class(self, request):
        for _compile, klass in self.handers:
            res = re.search(_compile, request.path)
            if res:
                # save the url's arguments
                request.args = res.groups()
                request.kwargs = res.groupdict()
                return klass
        return None
