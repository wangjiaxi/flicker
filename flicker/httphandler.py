
class HttpHandler(object):
    def __init__(self, request, response, connction):
        self.request = request
        self.response = response
        self.connction = connction

    def get(self, request, *args, **kwargs):
        pass

    def post(self, request, *args, **kwargs):
        pass

    def delete(self, request, *args, **kwargs):
        pass

    def put(self, request, *args, **kwargs):
        pass

    def head(self, request, *args, **kwargs):
        pass

    def option(self, request, *args, **kwargs):
        pass

    def patch(self, request, *args, **kwargs):
        pass
