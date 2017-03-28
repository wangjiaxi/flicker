

class HttpRequest(object):

    def __init__(self, method, path, protocol, headers,
                 body):
        self.method = method
        self.path = path
        self.protocol = protocol
        self.headers = headers
        self.body = body


class HttpResponse(object):
    def __init__(self, sock, address):
        self.sock = sock
        self.address = address

    def close(self):
        return self.sock.close()

    def write(self):
        pass

    def write_lines(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


class HttpConnection(object):
    def __init__(self, sock, address, method=None, path=None, protocol=None,
                 _cookies=None, headers=None, body=None):
        self.request = None
        self.response = None
