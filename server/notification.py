# Independent includes
import time
import webbrowser
from tkinter import Tk, messagebox, Label

# Project Includes
import constants
from server import Server

class Notification():
    def __init__(self):
        pass

    def alert(self):
        # root manages other notifications
        root = Tk()
        root.geometry("0x0")
        root.withdraw() # Hide root - it is empty anyway

        # Displays the info to the user
        messagebox.showinfo("Door Status", "The door has been opened")
