"""File to control all aspects of the server computer. They include:
1) waiting for a packet from the board
2) spawning a popup / notification that the door has been opened
"""
from queue import Queue
from server import Server
from notification import Notification

udp_port = 52160

def clear_queue(q: Queue):
    while not q.empty:
        q.get()

def wait_for_board(client_data: Queue) -> str:
    """Blocking function to wait for ping from the board i.e. that door has opened.
    \nreturn The msg sent from board on success, None otherwise"""
    # Startup the server
    server = Server(client_data, udp_port)
    server.start()

    while True:
        # Only check queue if thread set - got a response from board
        if server.thread_status.is_set():
            socket_response_dict = client_data.get()
            print(Server.translate_socket_dict(socket_response_dict))
            clear_queue(client_data)

            server.join()
            server = None
            return socket_response_dict['data']


if __name__ == "__main__":
    # Will store data, addr after recvfrom
    client_data = Queue()

    notif = Notification()

    # Send multicast so board knows what server's IP and port are
    Server.inform_board()

    # Constantly wait for a msg that the door has been opened from the board
    while True:
        data = wait_for_board(client_data)

        # Alert user whenever it gets a msg
        notif.alert()


# To test, run main and the following command in a terminal:
# nc -u 192.168.1.220 52160
# nc -u MATT-ROG-LAPTOP 52160 #TODO: get this to work with new hostname

#TODO: Get exit (xtrl+z or ctrl+c) handling to be more graceful for the thread