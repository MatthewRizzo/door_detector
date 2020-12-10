# door_detector

Project to detect opening of a door using Raspberry Pi, magnetic strip sensor, and wifi communication between the board and a computer.

## Portions of Code

1. Raspberry Pi Board GPIO Code (C++) - client side
    - Detects the door opening, using magentic strip sensor on door frame
    - Must be run on a Rapsberry Pi running a Linux like distro (i.e. Raspian)

2. Communication client(C++)/server(python)
    - server = another computer (probably my laptop)
    - Client = Raspberry Pi controlling the door sensor
    - when the RPi detect the door opening (using the sensor):
        - spawn the client as a thread and send a basic message to the server notifying it door closed/opened
        - client will handle receipt of message

3. Laptop Code - server side (python)
    - When program is running, waits for reciept of a packet from the client.
    - On reciept, triggers a basic popup or notification on laptop that door has been opened
    - **Currently, must be run on Windows for graphical aspect of tkinter**

**Note:** All client-side code will be written in a completely seperate directory than server-side.
This is because the client-side code will need to be compiled and run without interaction from server-side.
Folder structure
- door_detector/
  - board/
    - src/
  - server/
    - python files

## Experimental Setup

- Sensor plugged into GPIO's of board
  - See info_files for circuit diagram
- Sensor attached to door / door frame to detect opening
- Other laptop is across the room, not connected by wire to board