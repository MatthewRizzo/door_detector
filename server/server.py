# Built-in Includes
import platform
from queue import Queue
import re
import subprocess
import socket
import struct
import threading
from threading import Thread
import time

# Project Includes
import constants

class Server(Thread):
    """Class responsible for handling all communication's to/from client on the server machine.
    \nKeyword arguments:
    \nport -- The port number to wait for a connection on """
    _udp_port =  50001
    def __init__(self, data_queue: Queue, udp_port: int = 50001):
        # Threading related defines
        Thread.__init__(self)
        self.thread_status = threading.Event() # True when thread not running
        Server.set_port(udp_port)

        self._data_queue = data_queue

    def run(self):
        """Handle the start of the thread and its waiting for communication
        \nOverload run function of Thread for this thread class. Gets called by start()."""
        # Setup the socket server
        data_dict = Server.get_cur_hostname_IP()
        udp_ip = data_dict['ip']
        hostname = data_dict['hostname']
        sock = socket.socket(socket.AF_INET,  # Use the internet
                            socket.SOCK_DGRAM) # UDP
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        # sock.bind((udp_ip, self._udp_port))
        sock.bind((hostname, self._udp_port))
        print(f"Server is waiting for connection from client on IP {udp_ip} and port {self._udp_port}. hostname = {hostname}")

        # Wait for a connection
        while not self.thread_status.isSet():
            raw_data, raw_addr = sock.recvfrom(1024) # buffer size is 1024 bytes
            data = raw_data.decode()
            client_ip_addr, client_port = raw_addr

            data_dict = {'data': data.strip(), 'client_ip': client_ip_addr, 'client_port': client_port}
            self._data_queue.put(data_dict)

            self.thread_status.set()


    # Class methods to handle broadcasting / multicasting to Pi
    @classmethod
    def  _platform_check(cls, is_windows: bool = True) -> bool:
        """Utility function that checks what the platform is.
        \n is_windows - If true (defualt), assumes platform is windows and checks if true. If false, checks if is linux
        """
        expected_platform = "Windows" if is_windows else "Linux"
        return expected_platform == platform.system()

    @classmethod
    def get_cur_hostname_IP(cls)->dict:
        """Return the IP address of your machine as seen by the router
        \n{'hostname' : hostname, 'ip' : ip_addr}"""
        if(cls._platform_check(True)):
            hostname = socket.gethostname()
            ip_addr = socket.gethostbyname(hostname)
            return {'hostname' : hostname, 'ip' : ip_addr}
        else:
            # ip_reg_ex= r'inet addr:([^.]+.[^.]+.[^.]+.[^\s]+)'
            # ip_found = subprocess.check_output("ifconfig").decode()
            # matches = re.findall(ip_reg_ex, ip_found)

            # # There might be many matches. Only one cared about starts with 192.168 (router perspective)
            # # https://qr.ae/pNs807
            # reduce_matches = lambda match: match.startswith("192.168.")
            # ip_addr =  list(filter(reduce_matches, matches))[0]
            # return ip_addr
            name = socket.gethostname()
            ip_addr = socket.gethostbyname(name)
            return {'hostname' : name, 'ip' : ip_addr}

    @classmethod
    def inform_board(cls):
        """Function to ping board with a broadcast.
        \nGives the board this computer's IP and the port it is waiting on.
        Ignores responses.
        """
        # Send laptop's IP and port to the board
        data_dict = Server.get_cur_hostname_IP()
        ip = data_dict['ip']
        hostname = data_dict['hostname']
        multicast_group = ("224.1.1.1", 10000)

        # give board hostname, ip, port
        msg = f"{hostname}, {ip}, {Server.get_port()}"
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(.2)

        # Ensure only LAN gets broadcast - set time-to-live to 1, cannot go past router
        ttl = struct.pack('b', 1)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

        try:
            print("Sending broadcast to LAN")
            sent = sock.sendto(msg.encode(), multicast_group)
            # Don't care about responses
        finally:
            sock.close()

    @classmethod
    def get_port(cls):
        return cls._udp_port

    @classmethod
    def set_port(cls, new_port):
        cls._udp_port = new_port

    @classmethod
    def translate_socket_dict(cls, data: dict) -> str:
        """
        Takes in a dictionary from the thread worker, and creates an explanatory string"""
        msg = []
        for key, val in data.items():
            msg.append(f"{key}: {val}")
        return '\n'.join(msg)