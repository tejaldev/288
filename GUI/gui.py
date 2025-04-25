import time # Time library   
# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket
import tkinter as tk # Tkinter GUI library
# Thread library: https://www.geeksforgeeks.org/how-to-use-thread-in-tkinter-python/
import threading
import os  # import function for finding absolute path to this python script

curr_keys = {}
handle_flag = True

def main():
    
    global window  # Made global so quit function (send_quit) can access
    global curr_keys

    window = tk.Tk() # Create a Tk GUI Window
    # Quit command Button
    quit_command_Button = tk.Button(text ="Press to Quit", command = send_quit)
    quit_command_Button.pack()  # Pack the button into the window for display

    # Cybot Scan command Button
    scan_command_Button = tk.Button(text ="Press to Scan", command = send_scan)
    scan_command_Button.pack() # Pack the button into the window for display

    window.bind("<KeyPress>", key_down)
    window.bind("<KeyRelease>", key_up)

    # Create a Thread that will run a fuction assocated with a user defined "target" function.
    # In this case, the target function is the Client socket code
    my_thread = threading.Thread(target=socket_thread) # Create the thread
    my_thread.start() # Start the thread

    # Start event loop so the GUI can detect events such as button clicks, key presses, etc.
    window.mainloop()

# Quit Button action.  Tells the client to send a Quit request to the Cybot, and exit the GUI
def send_quit():
        global gui_send_message # Command that the GUI has requested be sent to the Cybot
        global window  # Main GUI window
        
        gui_send_message = "quit\n"   # Update the message for the Client to send
        time.sleep(1)
        window.destroy() # Exit GUI


# Scan Button action.  Tells the client to send a scan request to the Cybot
def send_scan():
        global gui_send_message # Command that the GUI has requested sent to the Cybot
        
        gui_send_message = "M\n"   # Update the message for the Client to send

def key_down(event):
    global curr_keys
    if event.char not in curr_keys:
        curr_keys[event.char] = True
        print(curr_keys)
        

def key_up(event):
    global curr_keys
    if event.char in curr_keys:
        del curr_keys[event.char]
    print(curr_keys)

"""
def handle_a_press(cybot):
    global handle_flag
    if not handle_flag:
        return
    handle_flag = False
    print("Started a")
    rx_message = bytearray(1) # Initialize a byte array
    while (rx_message.decode() != "END\n"): # Collect sensor data until "END" recieved
        rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
        
        
        print(rx_message.decode()) # Convert message from bytes to String (i.e., decode), then print
        stripped = rx_message.strip()
        print(stripped)
        if stripped == "END":
            break
"""

def socket_thread():
    global handle_flag
    #Define Globals
    global curr_keys
    global Last_command_Label # GUI label for displaying the last command sent to the Cybot
    global gui_send_message   # Command that the GUI has requested be sent to the Cybot

    
    # A little python magic to make it more convient for you to adjust where you want the data file to live
    # Link for more info: https://towardsthecloud.com/get-relative-path-python 
    absolute_path = os.path.dirname(__file__) # Absoult path to this python script
    relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
    full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
    filename = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command
    HOST = "192.168.1.1"  # The server's hostname or IP address
    PORT = 288        # The port used by the server
    cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
    cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)
                  
    cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile

    send_message = "Connection Initialized"
    cybot.write(send_message.encode()) # Convert String to bytes (i.e., encode), and send data to the server
    print(send_message)
    

    
    while send_message != 'quit\n':
        global handle_flag
        if ('a' in curr_keys):
            rx_message = bytearray(1) # Initialize a byte array
            while (rx_message.decode() != "END\n"): # Collect sensor data until "END" recieved
                
                rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
        
        
                print(rx_message.decode()) # Convert message from bytes to String (i.e., decode), then print
                stripped = rx_message.strip()
                #print(stripped)
                #if stripped == "END":
                    #break
            #threading.Thread(target=handle_a_press, args=(cybot,)).start()
            print("test")
                
                
                
        


    print("Client exiting, and closing file descriptor, and/or network socket\n")
    time.sleep(2) # Sleep for 2 seconds
    cybot.close() # Close file object associated with the socket or UART
    cybot_socket.close()  # Close the socket (NOTE: comment out if using UART interface, only use for network socket option)


main()