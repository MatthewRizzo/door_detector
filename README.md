# door_detector

Project to detect opening of a door using Raspberry Pi, magnetic strip sensor, and wifi communication between the board and a computer.

## User Guide / Setup

**Note:** The install of the Server is primarily tested for Linux or WSL, not Windows.

### **Building / Running on the board (Client)**

1. Perform the following command (does a fresh setup / install for building):
`cd board/install`
`sudo ./setup.sh -a`

2. Navigate back to `board/` and run the following
`make`

3. Run the binary on the board by doing the following:
`./board/bin/main_driver`
The name of the top level executable might change, but it should be aparent which one to run.
This binary will manage:
    - The reading/processing of sensor data
    - Turning any LED's on/off
    - Spinning up the client thread. Responsible for pinging the server when the door opens.

### **Running on the Server (Computer / laptop)**

**Note: the board's client program must be up and runnning before starting the Server**

1. Navigate to the `server/` dir
2. To setup for running the server please run the following command
`sudo ./install/install.sh`

3. To start up the server program execute the following: `python main.py`

## **Portions of Code**

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
        - TODO: next branch / implementation will move from tkinter to web apps, so it can run agnostic to OS... basically moving to Linux or WSL

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

### Supplies

- MC-38 Magnetic Switch Door Sensor from Gikfun
- Raspberry Pi 3 running Raspian (debian based distribution)
- Computer - to inform user when door opens. Any OS
- Resistors
- Wires
- Breadboard (for circuit)