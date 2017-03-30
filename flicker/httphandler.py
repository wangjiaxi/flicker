from flicker.exception import HTTPError


class HttpHandler(object):
    def __init__(self, connection):
        self.request = connection.request
        self.response = connection.response
        self.connection = connection
        self.query = self.request.query

    def distribute(self, *args, **kwargs):
        print(self.request.args)
        print(self.request.method.lower())
        return getattr(self, self.request.method.lower())(*args, **kwargs)

    def get_method(self):
        pass

    def set_headers(self, key, value):
        return self.request.set_headers(self, key, value)

    def clear_headers(self):
        return self.request.clear_headers()

    def get_header(self, name):
        return self.request.get_header(name)

    def write(self, status_code, message):
        return self.connection.send_message(status_code, message)

    def get(self, *args, **kwargs):
        raise HTTPError(405)

    def post(self, *args, **kwargs):
        raise HTTPError(405)

    def delete(self, *args, **kwargs):
        raise HTTPError(405)

    def put(self, *args, **kwargs):
        raise HTTPError(405)

    def head(self, *args, **kwargs):
        raise HTTPError(405)

    def option(self, *args, **kwargs):
        raise HTTPError(405)

    def patch(self, *args, **kwargs):
        raise HTTPError(405)
