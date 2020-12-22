"""File to control all aspects of the server computer. They include:
1) waiting for a packet from the board
2) spawning a popup / notification that the door has been opened
"""
# standard includes
from queue import Queue
import signal
import sys
from threading import Thread

# project includes
from server import Server
from notification import Notification

udp_port = 52160

def clear_queue(q: Queue):
    while not q.empty:
        q.get()

keep_running = True


def wait_for_board(server: Server, client_data: Queue) -> str:
    """Blocking function to wait for ping from the board i.e. that door has opened.
    \nreturn The msg sent from board on success, None otherwise"""
    socket_response_dict = {'data':None}
    # Only check queue if thread set - got a response from board
    if server.received_msg is True:
        socket_response_dict = client_data.get(block=False)
        print(Server.translate_socket_dict(socket_response_dict))
        clear_queue(client_data)
        server.join()

    return socket_response_dict['data'] # None unless a msg received

def signal_handler(sig, frame):
    print("Caught ctrl+c: ")
    global keep_running
    keep_running = False
    Server.set_got_reponse()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    # Will store data, addr after recvfrom
    client_data = Queue()

    notif = Notification()



    # Setup the board by giving the port and ip on this machine (server) to send messages to
    setup_thread = Thread(target = Server.setup_board)

    # Wait for the client (board) to confirm recepit of setup packet
    # confirm_thread = Thread(target = Server.confirm_board_setup)

    setup_thread.start()
    # confirm_thread.start()

    setup_thread.join()
    # confirm_thread.join()

    #TODO: wait for response from setup_board and save info

    server = Server(client_data, udp_port)
    server.start() # Startup the server

    # Constantly wait for a msg that the door has been opened from the board
    while True and keep_running is True:
        data = wait_for_board(server, client_data)

        if data is not None:
            # Alert user whenever it gets a msg
            notif.alert()


    # Kill the server thread
    server.stop_thread()
    server.join()
    print("Done with join for server")
    server = None

    sys.exit(1)

# To test, run main and the following command in a terminal:
# nc -u 192.168.1.220 52160
# nc -u MATT-ROG-LAPTOP 52160 #TODO: get this to work with new hostname
