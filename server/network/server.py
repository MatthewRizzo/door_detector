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
from network.network_utils import NetworkUtils

class Server(Thread):
    """Class responsible for handling all communication's to/from client on the server machine.
    """
    received_msg = False
    _run_port =  50001
    def __init__(self, data_queue: Queue, is_silent=False):
        """
        :param is_silent Defaults to False. If set to true, all print's will be suppressed
        """
        # Threading related defines
        Thread.__init__(self)
        self.thread_status = threading.Event() # True when thread not running
        Server.received_msg = False
        self._is_silent = is_silent

        self._data_queue = data_queue

    def wait_for_board(self, client_data: Queue) -> str:
        """Blocking function to wait for ping from the board i.e. that door has opened.
        \nreturn The msg sent from board on success, None otherwise"""
        socket_response_dict = {'data':None}
        # Only check queue if thread set - got a response from board
        if Server.received_msg is True:
            if not client_data.empty():
                socket_response_dict = client_data.get(block=False)
                if not self._is_silent:
                    print(Server.translate_socket_dict(socket_response_dict))
                self.clear_queue(client_data)
            self.stop_thread()

        return socket_response_dict['data'] # None unless a msg received


    def run(self):
        """Handle the start of the thread and its waiting for communication
        \nOverload run function of Thread for this thread class. Gets called by start()."""
        # Setup the socket server
        data_dict = NetworkUtils.get_cur_hostname_IP()
        udp_ip = data_dict['ip']
        hostname = data_dict['hostname']
        sock = socket.socket(socket.AF_INET,  # Use the internet
                            socket.SOCK_DGRAM) # UDP
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.settimeout(constants.SOCKET_TIMEOUT_SEC)
        sock.bind((udp_ip, self._run_port))
        if not self._is_silent:
            print(f"Server is waiting for ping from board/client on IP {udp_ip} and port {self._run_port}. hostname = {hostname}")

        # Wait for a ping from board
        while not self.thread_status.is_set() and Server.received_msg is False:
            try:
                raw_data, raw_addr = sock.recvfrom(1024) # buffer size is 1024 bytes
                data = raw_data.decode()
                board_ip_addr, client_port = raw_addr

                data_dict = {'data': data.strip(), 'board_ip': board_ip_addr, 'client_port': client_port}
                self._data_queue.put(data_dict)

                Server.received_msg = True
            except:
                # If no msg received, just start loop again
                pass
        sock.close()

    def stop_thread(self):
        """ Handles the end of the thread by setting and joining"""
        self.thread_status.set()


    def clear_queue(self, q: Queue):
        """Util Function
        """
        while not q.empty:
            q.get()
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
        return '\n'.join(msg) + '\n'

    @classmethod
    def set_got_msg(cls, new_got_msg: bool = True):
        """Sets the class variable governing if send_handshake has received a response or not.
        When set to True, the send_handshake and wait for confirmation loops end"""
        cls.received_msg = new_got_msg


