import socket

class UsageError(Exception): pass

class Netcat(object):
    def connect(self, host, port):
        if not isinstance(host, str):
            raise UsageError("Host must be ip address or resolvable url")
        try:
            addr = socket.getaddrinfo(host, port, proto=socket.IPPROTO_TCP)[0][4]
        except IndexError:
            raise UsageError("Unknown hostname or IP")
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(addr)

    def read(self, length=1024):
        return self.socket.recv(length)

    def write(self, data):
        self.socket.send(data.encode('utf-8'))

    def close(self):
        self.socket.close()
