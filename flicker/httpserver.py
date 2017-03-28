from .tcpserver import TcpServer
from .utils import parse_http_message
from .httpbase import HttpConnection

class HttpServer(TcpServer):

    def handler(self, buffer, conn, addr, *args, **kwargs):
        http_data = parse_http_message(buffer)
        http_conn = HttpConnection(conn, addr, **http_data)


