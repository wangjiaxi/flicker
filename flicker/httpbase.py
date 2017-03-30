from urllib import parse
from flicker.base import STATUS_CODE

class HttpRequest(object):

    def __init__(self, method, path, protocol, headers, _cookie, body):
        self.method = method
        self.path = path
        self.protocol = protocol
        self.headers = headers
        self.body = body
        self._cookie = _cookie
        self.query = self.parse_query()
        self.args = tuple()      # save the arguments that passed by request url
        self.kwargs = dict()      # save the arguments that passed by request url

    def set_cookie(self, name, value):
        self._cookie[name] = value

    def get_cookie(self, name):
        return self._cookie[name]

    def set_headers(self, name, value):
        self.headers[name] = value

    def get_header(self, name):
        try:
            return self.headers[name]
        except KeyError:
            return None

    def remove_header(self, name):
        try:
            self.headers.pop(name)
        except KeyError:
            pass

    def clear_headers(self):
        self.headers = {}

    def parse_query(self):
        return parse.parse_qs(self.body)


class HttpsRequest(HttpRequest):
    pass


class HttpResponse(object):
    def __init__(self, sock, address, protocol):
        self.sock = sock
        self.address = address
        self.protocol = protocol
        self.status_code = None
        self._cookie = None
        self.headers = None
        self.status_code = None
        self.message = None
        self._closed = False

    def close(self):
        self._closed = True
        return self.sock.close()

    def write(self, chuck):
        self.sock.send(chuck)

    def send_response(self):
        rsp = (self.status_line() + self.header_line() + self.response_line()).encode()
        self.write(rsp)

    def status_line(self):
        """return the status line of response message"""
        return """%s %s\r\n""" % (self.protocol, STATUS_CODE.get(self.status_code))

    def header_line(self):
        """return the headers line of response message"""
        cookie_string = self._cookie.output() + "\r\n"
        header_string = '\r\n'.join(["%s: %s" % (k, v) for k, v in self.headers.items()]) + "\r\n"
        return header_string + cookie_string + "\r\n"

    def response_line(self):
        """return the response line of response message"""
        return self.message


class HttpConnection(object):
    def __init__(self, sock, address, method=None, path=None, protocol=None,
                 _cookie=None, headers=None, body=None):

        self.request = None
        self.response = None
        self.headers = headers
        self.sock = sock
        self.protocol = protocol
        self.method = method
        self.path = path
        self.body = body
        self.address = address
        self._cookie = _cookie

        self.init_request()
        self.init_response()

    def init_request(self):
        self.request = self.init_http_request()

    def init_http_request(self):
        return HttpRequest(self.method, self.path, self.protocol,
                           self.headers, self._cookie, self.body)

    def init_https_request(self):
        return HttpsRequest(self.method, self.path, self.protocol,
                            self.headers, self._cookie, self.body)

    def init_response(self):
        self.response = HttpResponse(self.sock, self.address
                                     , self.protocol)

    def send_message(self, status_code, message):
        self.response.status_code = status_code
        self.response.message = message
        self.response.headers = self.request.headers
        self.response._cookie = self.request._cookie
        return self.response.send_response()
