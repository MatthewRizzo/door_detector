import atexit
from queue import Queue
import signal
import sys
from threading import Thread
import time

# Add subdirs to project path
sys.path.append("network/")
sys.path.append("web_app/")


class MainLoop(Thread):
    _program_ended = False
    def __init__(self):
        super(MainLoop, self).__init__()
        MainLoop._program_ended = False

    def start_main_loop(self,
        is_blocked_func,
        wait_for_board_func,
        client_data_queue,
        open_page_func,
        _send_block_to_frontend_func
    ):
        # Constantly wait for a msg that the door has been opened from the board
        while True and self._program_ended is False:
            if is_blocked_func() is True:
                print("main loop thinks it's blocked")
                continue
            data = wait_for_board_func(client_data_queue)
            if data is not None:
                # Alert user whenever it gets a msg by starting the app
                # app will shut itself down once alert is clicked

                open_page_func()
        sys.exit(1)

    @classmethod
    def kill_main_loop(cls):
        cls._program_ended = True