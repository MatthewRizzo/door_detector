MULTICAST_RANGE = ["224.0.0.0", "230.255.255.255"] # Not to use, but is a reference
MULTICAST_GROUP_IP = "224.1.1.1" # NEED to make a constant to be mutual with C++ code
MULTICAST_PORT =  55054
MULTICAST_INTERFACE = "203.106.93.94"


ALERT_URL = "alert"

# Constants related to client (board)
BOARD_HANDSHAKE_RECV_PORT = 52535
BOARD_HOSTNAME = "matt-rpi3b"

# Constants related to waiting for confirmation of setup from board
CONFIRMATION_WAIT_PORT = 55554

SOCKET_TIMEOUT_SEC = 5