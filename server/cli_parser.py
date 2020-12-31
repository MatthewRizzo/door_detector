import argparse

import constants

class Parser():
    """Class responsible for parsing command line args and exposing them to main class when requested."""
    def __init__(self):
        self._args = None
        self._parser = None

        self.create_args()
        self.parse()

    def create_args(self):
        """Util function to create the args to parse for"""
        self.parser = argparse.ArgumentParser(description='Door Detector Project Server (not connected to sensor)')
        client_args = self.parser.add_argument_group(title="Client Handshake Args",
                description="Arguments related to communicating with the RPI during the handshake phase")
        client_args.add_argument('-cp', '--client_handshake_port', help="Use to set the handshake port on the client (rpi)",
                                type=int, default=constants.BOARD_HANDSHAKE_RECV_PORT)
        client_args.add_argument('-n','--client_hostname', type=str, default=constants.BOARD_HOSTNAME,
                                help="Use to select the hostname of the client (connected to the sensor)")
        self.parser.add_argument("-p", "--server_run_port", type=int, default=constants.DEFAULT_RUN_PORT,
                                help="Use this to set the port the client (board) will send notifications to")

    def parse(self):
        self.args = self.parser.parse_args()

    @property
    def args(self):
        return self._args

    @args.setter
    def args(self, parsed_args):
        self._args = parsed_args