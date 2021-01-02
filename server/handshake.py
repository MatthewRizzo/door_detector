# project includes
from server import Server


# standard includes
from queue import Queue
import signal
from threading import Thread

class Handshake():
    """Responsible for performing the handshake with the rpi and establishing Comms
    \nEnds up wrapping a lot of the Server classmethods
    """
    def __init__(self, args):
        """
        :param args: The result of parsing CLI
        """
        self._args = args


    def perform_handshake(self):
        # Setup the board by giving the port and ip on this machine (server) to send messages to
        setup_thread = Thread(target = Server.send_handshake,
                            args=(self._args.client_hostname,
                                self._args.client_handshake_port)
                            )

        # Wait for the client (board) to confirm recepit of setup packet
        confirm_thread = Thread(target = Server.confirm_handshake)

        # Start the threads
        setup_thread.start()
        confirm_thread.start()

        # Have the threads die as soon as possible
        setup_thread.join()
        confirm_thread.join()