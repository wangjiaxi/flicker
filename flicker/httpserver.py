from flicker.tcpserver import TcpServer
from flicker.utils import parse_http_message
from flicker.httpbase import HttpConnection
from flicker.base import HTTP_404_Code, STATUS_CODE


class HttpServer(TcpServer):

    def distribute(self, buff, conn, addr, *args, **kwargs):
        http_data = parse_http_message(buff)
        http_conn = HttpConnection(conn, addr, **http_data)
        handler_class = self.get_handler_class(http_conn.request)
        if not handler_class:
            # url path not exist
            return http_conn.send_message(
                    HTTP_404_Code, STATUS_CODE.get(HTTP_404_Code))

        return handler_class(http_conn).distribute()

    def get_handler_class(self, path):
        return self.application.get_handler_class(path)
