# Built-in Includes
import json
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
    got_response = False
    """Class responsible for handling all communication's to/from client on the server machine.
    \nKeyword arguments:
    \nport -- The port number to wait for a connection on """
    _run_port =  50001
    def __init__(self, data_queue: Queue):
        # Threading related defines
        Thread.__init__(self)
        self.thread_status = threading.Event() # True when thread not running
        self.received_msg = False
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
        sock.settimeout(constants.SOCKET_TIMEOUT_SEC)
        sock.bind((udp_ip, self._run_port))
        print("Communcation Protocal Established\n------------------------\n\n")
        print(f"Server is waiting for ping from board/client on IP {udp_ip} and port {self._run_port}. hostname = {hostname}")

        # Wait for a connection
        while not self.thread_status.isSet() and self.received_msg is False:
            try:
                raw_data, raw_addr = sock.recvfrom(1024) # buffer size is 1024 bytes
                data = raw_data.decode()
                board_ip_addr, client_port = raw_addr

                data_dict = {'data': data.strip(), 'board_ip': board_ip_addr, 'client_port': client_port}
                self._data_queue.put(data_dict)

                self.received_msg = True
            except:
                # If no msg received, just start loop again
                pass
        sock.close()

    def stop_thread(self):
        """ Handles the end of the thread by setting and joining"""
        self.thread_status.set()
        self._stop_thread = True

    # Class methods to handle multicasting / multicasting to Pi
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
            # Forces getting the local IP behind the NAT
            ip_addr = subprocess.check_output("hostname -I", shell=True).decode().strip()
            return {'hostname' : name, 'ip' : ip_addr}

    @classmethod
    def send_handshake(cls, client_hostname, board_handshake_recv_port):
        """Function to ping board with a simple UDP msg.
        \nGives the board this computer's IP and the port it is waiting on.
        Keeps attempting to send until a response is received.
        """
        # Send laptop's IP and port to the board
        data_dict = Server.get_cur_hostname_IP()
        ip = data_dict['ip']
        hostname = data_dict['hostname']

        # give board hostname, ip, port
        data_json = {'host' : hostname, 'ip' : ip, 'port' : Server.get_run_port()}
        board_ip = socket.gethostbyname(client_hostname)
        msg = f"Sending msg to ip {board_ip} and port {board_handshake_recv_port}."
        msg += " Will keep trying until a reponse is received."
        print(msg)

        while cls.got_response is False:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(.2)
            try:
                sent = sock.sendto(json.dumps(data_json).encode('utf-8'), (board_ip, constants.BOARD_HANDSHAKE_RECV_PORT))
                # Don't care about responses
            except socket.timeout:
                sock.close()

    @classmethod
    def confirm_handshake(cls):
        """BLOCKING - CALL IN THREAD. During setup of communications, waits for response from the board.
        modifies cls.got_response when it does"""
        print(f"Starting confirm setup thread. Waits on port {constants.CONFIRMATION_WAIT_PORT}")
        while cls.got_response is False:
            recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
            recv_sock.bind((Server.get_cur_hostname_IP()['ip'], constants.CONFIRMATION_WAIT_PORT))
            recv_sock.settimeout(constants.SOCKET_TIMEOUT_SEC)
            try:
                data, address = recv_sock.recvfrom(1024, 0)
                print(f"Received confirmation {data.decode()}")
                cls.set_got_reponse(True)
            except socket.timeout:
                recv_sock.close()
            recv_sock.close()


    @classmethod
    def get_run_port(cls):
        return cls._run_port

    @classmethod
    def set_run_port(cls, new_port):
        cls._run_port = new_port

    @classmethod
    def translate_socket_dict(cls, data: dict) -> str:
        """
        Takes in a dictionary from the thread worker, and creates an explanatory string"""
        msg = []
        for key, val in data.items():
            msg.append(f"{key}: {val}")
        return '\n'.join(msg)

    @classmethod
    def set_got_reponse(cls, new_got_response: bool = True):
        """Sets the class variable governing if send_handshake has received a response or not.
        When set to True, the send_handshake and wait for confirmation loops end"""
        cls.got_response = new_got_response
