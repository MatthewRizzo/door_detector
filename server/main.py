"""File to control all aspects of the server computer. They include:
1) waiting for a packet from the board
2) spawning a popup / notification that the door has been opened
"""
# standard includes
import atexit
from queue import Queue
import signal
import sys
from threading import Thread
import time

# Add subdirs to project path
sys.path.append("network/")
sys.path.append("web_app/")

# project includes
from web_app.app_manager import AppManager
from network.handshake import Handshake
from network.server import Server
from cli_parser import Parser

program_ended = False


def signal_handler(sig, frame):
    """Wraps all exit handling related to networked classes/threads
    """
    print("Caught ctrl+c: ")
    global program_ended
    program_ended = True
    Handshake.set_got_reponse()
    Server.set_got_msg()
    AppManager.kill_app()


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    parser = Parser()
    args = parser.args

    # Set values obtained from parser (where applicable)
    Server.set_run_port(args.server_run_port)

    server = None

    # Will store data, addr after recvfrom
    client_data = Queue()

    handshake = Handshake(args, args.silent)
    handshake.perform_handshake()

    # Startup the server
    server = Server(client_data, args.silent)
    server.start()

    # Takes a long time so do this ASAP
    app = AppManager(args.debug_app, args.web_app_port, args.silent, server, client_data)

    # Will handle the getting of data from the board and opening the webpage when needed
    app.start_app()