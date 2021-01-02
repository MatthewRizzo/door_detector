import json
from queue import Queue
import subprocess
import socket
import struct
import sys
import threading
from threading import Thread
import time

# Project Includes
import constants
from network import network_utils
from network.network_utils import NetworkUtils

class Handshake(Thread):
    """Class responsible for static functions needed to
        establish communications with the client / board
    """
    got_handshake_response = False
    def __init__(self, args):
        """
        :param args: The result of parsing CLI
        """
        self._args = args

    def perform_handshake(self):
        # Setup the board by giving the port and ip on this machine (server) to send messages to
        setup_thread = Thread(target = Handshake.send_handshake,
                            args=(self._args.client_hostname,
                                self._args.client_handshake_port,
                                self._args.server_run_port)
                            )

        # Wait for the client (board) to confirm recepit of setup packet
        confirm_thread = Thread(target = Handshake.confirm_handshake)

        # Start the threads
        setup_thread.start()
        confirm_thread.start()

        # Have the threads die as soon as possible
        setup_thread.join()
        confirm_thread.join()

    @classmethod
    def send_handshake(cls, client_hostname, board_handshake_recv_port, server_run_port):
        """Function to ping board with a simple UDP msg.
        \nGives the board this computer's IP and the port it is waiting on.
        Keeps attempting to send until a response is received.
        """
        # Send laptop's IP and port to the board
        data_dict = NetworkUtils.get_cur_hostname_IP()
        ip = data_dict['ip']
        hostname = data_dict['hostname']

        # give board hostname, ip, port
        data_json = {'host' : hostname, 'ip' : ip, 'port' : server_run_port}
        board_ip = socket.gethostbyname(client_hostname)
        msg = f"Sending msg to ip {board_ip} and port {board_handshake_recv_port}."
        msg += " Will keep trying until a reponse is received."
        print(msg)

        while cls.got_handshake_response is False:
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
        modifies cls.got_handshake_response when it does"""
        print(f"Starting confirm setup thread. Waits on port {constants.CONFIRMATION_WAIT_PORT}")
        while cls.got_handshake_response is False:
            recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
            recv_sock.bind((NetworkUtils.get_cur_hostname_IP()['ip'], constants.CONFIRMATION_WAIT_PORT))
            recv_sock.settimeout(constants.SOCKET_TIMEOUT_SEC)
            try:
                data, address = recv_sock.recvfrom(1024, 0)
                print(f"Received confirmation {data.decode()}")
                cls.set_got_reponse(True)
            except socket.timeout:
                recv_sock.close()
            recv_sock.close()

    @classmethod
    def set_got_reponse(cls, new_got_handshake_response: bool = True):
        """Sets the class variable governing if send_handshake has received a response or not.
        When set to True, the send_handshake and wait for confirmation loops end"""
        cls.got_handshake_response = new_got_handshake_response