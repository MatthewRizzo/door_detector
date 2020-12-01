"""File to control all aspects of the server computer. They include:
1) waiting for a packet from the board
2) spawning a popup / notification that the door has been opened
"""
from queue import Queue
from server import Server

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
            addr = client_data.get()
            data = client_data.get()
            clear_queue(client_data)

            print(f"Received data from board {data}")
            return data

if __name__ == "__main__":
    # Will store data, addr after recvfrom
    client_data = Queue()

    # Send broadcast so board knows what server's IP and port are
    Server.inform_board()

    # Constantly wait for a msg that the door has been opened from the board
    while True:
        data = wait_for_board(client_data)

        # TODO: make popup here

