import time # Time library   
# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket
import tkinter as tk # Tkinter GUI library
# Thread library: https://www.geeksforgeeks.org/how-to-use-thread-in-tkinter-python/
import threading
import os  # import function for finding absolute path to this python script
#from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import matplotlib.pyplot as plt
import re
import queue

curr_keys = set()
handle_flag = True
first_key = None
key_is_pressed = False
key_is_released = False
update_gui = True
window = None
polar_fig = None
polar_ax = None
xy_fig = None
xy_ax = None

command_queue = queue.Queue()



absolute_path = os.path.dirname(__file__) # Absoult path to this python script
relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
sensor_file = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command
movement_file = 'movement.txt'

def main():
    
    global window  # Made global so quit function (send_quit) can access
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    global update_gui
    

    window = tk.Tk() # Create a Tk GUI Window
    window.title("CyBot Control Interface")
    # Quit command Button
    quit_command_Button = tk.Button(text ="Press to Quit", command = send_quit)
    quit_command_Button.pack()  # Pack the button into the window for display

    # Cybot Scan command Button

    window.bind("<KeyPress>", key_down)
    window.bind("<KeyRelease>", key_up)

    # Create a Thread that will run a fuction assocated with a user defined "target" function.
    # In this case, the target function is the Client socket code
    my_thread = threading.Thread(target=socket_thread, daemon=True) # Create the thread
    my_thread.start() # Start the thread

    embed_initial_plot()

    plot_thread = threading.Thread(target=update_plot_thread, daemon=True)
    plot_thread.start()

    # Start event loop so the GUI can detect events such as button clicks, key presses, etc.
    # Embed the plot in the Tkinter window using FigureCanvasTkAgg
    
    
    
    window.mainloop()
    
    
                
                
# Quit Button action.  Tells the client to send a Quit request to the Cybot, and exit the GUI
def send_quit():
        global gui_send_message # Command that the GUI has requested be sent to the Cybot
        global window  # Main GUI window
        
        gui_send_message = "quit\n"   # Update the message for the Client to send
        time.sleep(1)
        window.destroy() # Exit GUI




def key_down(event):
    global curr_keys
    global first_key
    global key_is_pressed
    if event.char not in curr_keys:
        curr_keys.add(event.char)
        if first_key is None and event.char in ['w', 'a', 's', 'd', 'b']:
            first_key = event.char
            key_is_pressed = True
            command_queue.put(event.char)
        

def key_up(event):
    global curr_keys
    global first_key
    global key_is_released
    global update_gui
    if event.char in curr_keys:
        curr_keys.remove(event.char)

        if event.char == first_key:
            first_key = None
            key_is_released = True
            update_gui = True
            command_queue.put('q')


def update_plot_thread():
    global update_gui
    if update_gui:
        update_gui = False
        update_plot()
    time.sleep(0.1)  # so it does't run constantly

def embed_initial_plot():
    global polar_fig, polar_ax
    global xy_fig, xy_ax

    polar_fig, polar_ax = plt.subplots(subplot_kw={'projection': 'polar'})
    xy_fig, xy_ax = plt.subplots()

    # Configure initial plot settings
    polar_ax.set_title("CyBot Sensor Scan (0-180 Degrees)")
    polar_ax.set_rmax(120)
    polar_ax.grid(True)
    
    xy_ax.set_xlim(-10, 10)
    xy_ax.set_ylim(-10, 10)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)
    
    # Show the plots in separate windows
    plt.figure(polar_fig.number)
    plt.show(block=False)
    plt.figure(xy_fig.number)
    plt.show(block=False)

def update_plot():
    global polar_fig, polar_ax, polar_canvas
    global xy_fig, xy_ax, xy_canvas
    global update_gui
    
    # Read the data from the file
    degree_pattern = r"Degree: (\d+)"
    distance_pattern = r"ADC Distance: (\d+\.\d+)"
    ob_ang_pattern = r"Object at (\d+)"
    ob_r_pattern = r"has a distance of (\d+\.\d+)"
    ob_r_width = r"and width of (\d+\.\d+)"
    
    with open(full_path + sensor_file, "r") as file:
        data = file.read()

    degrees = [int(deg) for deg in re.findall(degree_pattern, data)]
    degrees = np.array(degrees)
    #degrees = moving_avg(degrees)
        
    distances = [float(dist) for dist in re.findall(distance_pattern, data)]
    distances = np.array(distances)
    angle_radians = (np.pi/180) * degrees

    ob_ang = [int(ob_deg) for ob_deg in re.findall(ob_ang_pattern, data)]
    ob_r = [float(ob_dist) for ob_dist in re.findall(ob_r_pattern, data)]
    ob_width = [float(ob_diam) for ob_diam in re.findall(ob_r_width, data)]

    ob_ang = np.array(ob_ang)
    ob_rad = (np.pi/180) * ob_ang
    ob_r = np.array(ob_r)
    ob_width = np.array(ob_width)

    
      

    # Clear the existing axes to prevent plotting over previous data
    polar_ax.clear()

    # Plot the new data
    polar_ax.plot(angle_radians, distances, color='r', linewidth=4.0)
    
    if len(ob_ang) > 0:
        polar_ax.scatter(ob_rad, ob_r, s=ob_width**2, c='blue')
    polar_ax.set_xlabel('Distance (cm)', fontsize=14.0)
    polar_ax.set_ylabel('Angle (degrees)', fontsize=14.0)
    polar_ax.xaxis.set_label_coords(0.5, 0.15)
    polar_ax.tick_params(axis='both', which='major', labelsize=14)
    polar_ax.set_rmax(120)
    polar_ax.set_rticks([10, 25, 50, 75, 100])
    polar_ax.set_rlabel_position(-22.5)
    polar_ax.set_thetamax(180)
    polar_ax.set_xticks(np.arange(0, np.pi + 0.1, np.pi / 4))
    polar_ax.grid(True)
    polar_ax.set_title("Mock-up Polar Plot of CyBot Sensor Scan from 0 to 180 Degrees", size=14, y=1.0, pad=-24)

    # Redraw the plot
    plt.figure(polar_fig.number)
    plt.draw()

    xy_ax.clear()
    xy_ax.set_xlim(-10,10)
    xy_ax.set_ylim(-10,10)
    xy_ax.plot([0, 1, 2], [0, 1, 0], color='green')
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    #xy_ax.legend()
    xy_ax.grid(True)

    plt.figure(xy_fig.number)
    plt.draw()


        # Schedule the next update (every 1000ms)
    window.after(10, update_plot)

def moving_avg(array):
    if len(array) < 5:
        return array
    result = np.convolve(array, np.ones(5)/5, mode='same')
    return result




def read_cybot(cybot):
    while True:
        try:
            
            rx_message = cybot.readline()
            decoded = rx_message.decode().strip()
            print(decoded)
            if decoded:  # Only print if there's actually something there
                if decoded[0] == 's':
                    file_object = open(full_path + sensor_file,'w') # Open the file: file_object is just a variable for the file "handler" returned by open()
                    
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
                    update_gui = True   
                elif decoded[0] in ('w', 'a', 'd'):
                    file_object = open(full_path + movement_file,'a') # Open the file: file_object is just a variable for the file "handler" returned by open()
                    
                    #file_object.write(rx_message.decode()[1:])
                    print(rx_message.decode()[1:])
                    
                    while (rx_message.decode().strip() != "END"): # Collect sensor data until "END" recieved
                        rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
                        if rx_message.decode().strip()[0] == 'q':
                            file_object.write(rx_message.decode()[1:])
                            print(rx_message.decode()[1:])
                            
                        else:
                            
                            print(rx_message.decode()) # Convert message from bytes to String (i.e., decode), then print

                    file_object.close() # Important to close file once you are done with it!!   
                    update_gui = True



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
    global update_gui
    
    global Last_command_Label # GUI label for displaying the last command sent to the Cybot
    global gui_send_message   # Command that the GUI has requested be sent to the Cybot

    
    # A little python magic to make it more convient for you to adjust where you want the data file to live
    # Link for more info: https://towardsthecloud.com/get-relative-path-python 
    absolute_path = os.path.dirname(__file__) # Absoult path to this python script
    relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
    full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
    sensor_file = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command
    #HOST = "192.168.1.1"  # Use this if using the actual Cybot
    HOST = "127.0.0.1" # Use this if using the MOCK Cybot
    PORT = 288        # The port used by the server
    cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
    cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)
                  
    cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile

    read_thread = threading.Thread(target=read_cybot, args=(cybot,), daemon=True)
    read_thread.start() 

    send_message = "Connection Initialized"
    print(send_message)






    last_command = None

    while send_message != 'quit\n':
        try:
            command = command_queue.get(timeout=0.1)

            if command != last_command or command == 'q':
                cybot.write(command.encode())
                last_command = command
                if command == 'quit\n':
                    break
            
            command_queue.task_done()
            
        except queue.Empty:
            
            pass    
            
        
        
      
        


    print("Client exiting, and closing file descriptor, and/or network socket\n")
    time.sleep(2) # Sleep for 2 seconds
    cybot.close() # Close file object associated with the socket or UART
    cybot_socket.close()  # Close the socket (NOTE: comment out if using UART interface, only use for network socket option)


main()