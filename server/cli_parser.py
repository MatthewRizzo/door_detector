import argparse
import constants
import sys

# project imports
from web_app import app_constants

class Parser():
    """Class responsible for parsing command line args and exposing them to main class when requested."""
    def __init__(self):
        self._args = None
        self._parser = None

        self.create_args()
        self._parse()

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

        web_app_args = self.parser.add_argument_group(title="Web App Args",
                    description="Arguments related to the Web App")
        web_app_args.add_argument('-d', '--debug_app', default=False, action='store_true',
                                help="Use this flag to put the web app into debug mode (default is to have production mode)")
        web_app_args.add_argument('-wp', '--web_app_port', type=int, default=app_constants.WEB_APP_PORT)

        # args for utility
        self.parser.add_argument("-s", "--silent", default=False, action='store_true',
                                help="Use this flag to have the server run silent. The only print that occurs is the url the webpage will be hosted on")

    def _parse(self):
        self.args = self.parser.parse_args()

    @property
    def args(self):
        return self._args

    @args.setter
    def args(self, parsed_args):
        self._args = parsed_args