import socket
import libevent
import logging


class TcpServer(object):

    def __init__(self, host, port, application, socket_family=socket.AF_INET,
                 socket_type=socket.SOCK_STREAM, blocking=False,
                 listen_n=16, event_type=None, callback=None, max_buffer_size=None,
                 read_chunk_size=None):

        self.host = host
        self.port = port
        self.sock = socket.socket(socket_family, socket_type)
        self.base = libevent.Base()
        self.event = None
        self.listen_n = listen_n
        self.event_type = event_type
        self._callback = callback
        self.application = application
        self.max_buffer_size = max_buffer_size or 104857600
        # A chunk size that is too close to max_buffer_size can cause
        # spurious failures.
        self.read_chunk_size = min(read_chunk_size or 65536,
                                   self.max_buffer_size // 2)
        if not event_type:
            self.event_type = libevent.EV_READ|libevent.EV_PERSIST

        self.sock.setblocking(blocking)

    def start(self):
        self.sock.bind((self.host, self.port))
        self.sock.listen(self.listen_n)
        self.event = libevent.Event(self.base, self.sock.fileno(),
                                    self.event_type, self.do_accept, self.sock)
        self.event.add()
        self.base.dispatch()

    def read_from_sock(self, conn):
        return conn.recv(self.read_chunk_size)

    def do_accept(self, event, sock_fd, what, _sock):
        if what & libevent.EV_READ:
            conn, addr = _sock.accept()
            self.distribute(self.read_from_sock(conn), conn, addr)

    def distribute(self, buff, conn, addr, *args, **kwargs):
        pass
