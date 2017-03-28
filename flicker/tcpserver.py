import socket
import libevent
import logging

MAX_RECV_SIZE = 1024


class TcpServer(object):

    def __init__(self, host, port, socket_family=socket.AF_INET,
                 socket_type=socket.SOCK_STREAM, blocking=False,
                 listen_n=16, event_type=None, callback=None):

        self.host = host
        self.port = port
        self.sock = socket.socket(socket_family, socket_type)
        self.base = libevent.Base()
        self.event = None
        self.listen_n = listen_n
        self.event_type = event_type
        self._callback = callback

        if not event_type:
            self.event_type = libevent.EV_READ|libevent.EV_PERSIST

        self.sock.setblocking(blocking)

    def start(self):
        self.sock.bind((self.host, self.port))
        self.sock.listen(self.listen_n)
        self.event = libevent.Event(self.base, self.sock.fileno(), self.event_type, self.do_accept, self.sock)
        self.event.add()
        self.base.dispatch()

    def do_accept(self, event, sock_fd, what, _sock):
        if what & libevent.EV_READ:
            conn, addr = _sock.accept()
            buffer = b''
            while True:
                buf = conn.recv(MAX_RECV_SIZE)
                if not len(buf):
                    break
                buffer += buf
            self.handler(buffer, conn, addr)

    def handler(self, buffer, conn, addr, *args, **kwargs):
        pass
