import time # Time library   
# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket
import tkinter as tk # Tkinter GUI library
# Thread library: https://www.geeksforgeeks.org/how-to-use-thread-in-tkinter-python/
import threading
import os  # import function for finding absolute path to this python script
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import matplotlib.pyplot as plt
import re
import queue

curr_keys = set()
handle_flag = True
first_key = None
key_is_pressed = False
key_is_released = False



absolute_path = os.path.dirname(__file__) # Absoult path to this python script
relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
filename = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command

def main():
    
    global window  # Made global so quit function (send_quit) can access
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    

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
    # Embed the plot in the Tkinter window using FigureCanvasTkAgg
    
    embed_initial_plot()
    
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
    global first_key
    global key_is_pressed
    if event.char not in curr_keys:
        curr_keys.add(event.char)
        if first_key is None and event.char in ['w', 'a', 's', 'd']:
            first_key = event.char
            key_is_pressed = True
        

def key_up(event):
    global curr_keys
    global first_key
    global key_is_released
    if event.char in curr_keys:
        curr_keys.remove(event.char)

        if event.char == first_key:
            first_key = None
            key_is_released = True

def embed_initial_plot():
    global fig
    global ax
    global polar
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    polar = FigureCanvasTkAgg(fig, master=window)
    polar.draw()
    polar.get_tk_widget().pack()

    # Schedule the first plot update
    window.after(100, update_plot)

def update_plot():
    global fig
    global ax
    global polar
    global key_is_released
    if key_is_released:
        
        # Read the data from the file
        degree_pattern = r"Degree: (\d+)"
        distance_pattern = r"ADC Distance: (\d+\.\d+)"
    
        with open(full_path + filename, "r") as file:
            data = file.read()

        degrees = [int(deg) for deg in re.findall(degree_pattern, data)]
        degrees = np.array(degrees)
        distances = [float(dist) for dist in re.findall(distance_pattern, data)]
        distances = np.array(distances)
        angle_radians = (np.pi/180) * degrees

        # Clear the existing axes to prevent plotting over previous data
        ax.clear()

        # Plot the new data
        ax.plot(angle_radians, distances, color='r', linewidth=4.0)
        ax.set_xlabel('Distance (cm)', fontsize=14.0)
        ax.set_ylabel('Angle (degrees)', fontsize=14.0)
        ax.xaxis.set_label_coords(0.5, 0.15)
        ax.tick_params(axis='both', which='major', labelsize=14)
        ax.set_rmax(2.5)
        ax.set_rticks([50, 100, 150, 200, 250])
        ax.set_rlabel_position(-22.5)
        ax.set_thetamax(180)
        ax.set_xticks(np.arange(0, np.pi + 0.1, np.pi / 4))
        ax.grid(True)
        ax.set_title("Mock-up Polar Plot of CyBot Sensor Scan from 0 to 180 Degrees", size=14, y=1.0, pad=-24)

        # Redraw the plot
        polar.draw()

        # Schedule the next update (every 1000ms)
    window.after(100, update_plot)

def read_cybot(cybot):
    while True:
        try:
            
            rx_message = cybot.readline()
            decoded = rx_message.decode().strip()
            if decoded:  # Only print if there's actually something there
                if decoded[0] == 's':
                    file_object = open(full_path + filename,'w') # Open the file: file_object is just a variable for the file "handler" returned by open()
                    
                    file_object.write(rx_message.decode()[1:])
                    print(rx_message.decode()[1:])
                    
                    while (rx_message.decode().strip() != "END"): # Collect sensor data until "END" recieved
                        rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
                        if rx_message.decode().strip()[0] == 'q':
                            file_object.write(rx_message.decode()[1:])
                            print(rx_message.decode()[1:])
                        else:
                            file_object.write(rx_message.decode())  # Write a line of sensor data to the file
                            print(rx_message.decode()) # Convert message from bytes to String (i.e., decode), then print

                    file_object.close() # Important to close file once you are done with it!!                
                else:
                    print(f"Received: {decoded}")
        except Exception as e:
            print(f"Error reading from cybot: {e}")
            break
    
def socket_thread():
    
    #Define Globals
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    
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

    read_thread = threading.Thread(target=read_cybot, args=(cybot,))
    read_thread.start() 

    send_message = "Connection Initialized"
    print(send_message)






    curr_cmd = 0
    prev_cmd = 0
    while send_message != 'quit\n':
        
        if key_is_pressed:
            key_is_pressed = False
            curr_cmd = first_key
        
        if key_is_released:
            key_is_released = False
            curr_cmd = 'q'
            
            
            
            
            
        
        
        if curr_cmd != 0:
            send_str = curr_cmd
            cybot.write(send_str.encode())
            
        
        if curr_cmd != 0:
            prev_cmd = curr_cmd
        curr_cmd = 0
        


    print("Client exiting, and closing file descriptor, and/or network socket\n")
    time.sleep(2) # Sleep for 2 seconds
    cybot.close() # Close file object associated with the socket or UART
    cybot_socket.close()  # Close the socket (NOTE: comment out if using UART interface, only use for network socket option)


main()