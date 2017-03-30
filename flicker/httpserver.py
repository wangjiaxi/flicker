from flicker.tcpserver import TcpServer
from flicker.utils import parse_http_message
from flicker.httpbase import HttpConnection


class HttpServer(TcpServer):

    def distribute(self, buff, conn, addr, *args, **kwargs):
        http_data = parse_http_message(buff)
        http_conn = HttpConnection(conn, addr, **http_data)
        handler_class = self.get_handler(http_data.get("path"))
        handler_class(http_conn).distribute()

    def get_handler(self, path):
        return self.application.get_handler(path)
