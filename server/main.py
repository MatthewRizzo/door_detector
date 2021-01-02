"""File to control all aspects of the server computer. They include:
1) waiting for a packet from the board
2) spawning a popup / notification that the door has been opened
"""
# standard includes
from queue import Queue
import signal
import sys
from threading import Thread

# Add subdirs to project path
sys.path.append("network/")

# project includes
# from handshake import Handshake
from network.handshake import Handshake
from network.server import Server
from notification import Notification
from cli_parser import Parser

def clear_queue(q: Queue):
    while not q.empty:
        q.get()

program_ended = False

# TODO: Refactor / move this into another class
def wait_for_board(server: Server, client_data: Queue) -> str:
    """Blocking function to wait for ping from the board i.e. that door has opened.
    \nreturn The msg sent from board on success, None otherwise"""
    socket_response_dict = {'data':None}
    # Only check queue if thread set - got a response from board
    if server.received_msg is True:
        if not client_data.empty():
            socket_response_dict = client_data.get(block=False)
            print(Server.translate_socket_dict(socket_response_dict))
            clear_queue(client_data)
        server.stop_thread()

    return socket_response_dict['data'] # None unless a msg received

def signal_handler(sig, frame):
    print("Caught ctrl+c: ")
    global program_ended
    program_ended = True
    Handshake.set_got_reponse()
    Server.set_got_msg()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    parser = Parser()
    args = parser.args

    # Set values obtained from parser (where applicable)
    Server.set_run_port(args.server_run_port)

    server = None

    # Will store data, addr after recvfrom
    client_data = Queue()

    notif = Notification()

    handshake = Handshake(args)
    handshake.perform_handshake()

    # Startup the server
    server = Server(client_data)
    server.start()

    # Constantly wait for a msg that the door has been opened from the board
    while True and program_ended is False:
        data = wait_for_board(server, client_data)
        if data is not None:
            # Alert user whenever it gets a msg
            notif.alert()

            # once the alert is clicked, reset the server run
            server = None
            server = Server(client_data)
            server.start()


    # Kill the server thread
    if server is not None:
        server.stop_thread()
        server.join()
        print("Done with join for server")
        server = None

    sys.exit(1)