import time  # Time library
import socket
import tkinter as tk  # Tkinter GUI library
import threading
import os  # import function for finding absolute path to this python script
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

absolute_path = os.path.dirname(__file__)  # Absolute path to this python script
relative_path = "./"  # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
full_path = os.path.join(absolute_path, relative_path)  # Full path to sensor data file
sensor_file = 'sensor-scan.txt'  # Name of file you want to store sensor data from your sensor scan command
movement_file = 'movement.txt'

# Global variables to track position and orientation
position = [0.0, 0.0]  # x, y
heading = 0.0  # In degrees, 0 = facing right
path = [[0.0], [0.0]]  # path[0] = x list, path[1] = y list

def main():
    global window  # Made global so quit function (send_quit) can access
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    global update_gui
    
    window = tk.Tk()  # Create a Tk GUI Window
    window.title("CyBot Control Interface")
    
    # Quit command Button
    quit_command_Button = tk.Button(text="Press to Quit", command=send_quit)
    quit_command_Button.pack()  # Pack the button into the window for display

    # Cybot Scan command Button
    window.bind("<KeyPress>", key_down)
    window.bind("<KeyRelease>", key_up)

    # Create a Thread that will run a function associated with a user-defined "target" function.
    my_thread = threading.Thread(target=socket_thread, daemon=True)  # Create the thread
    my_thread.start()  # Start the thread

    embed_initial_plot()

    # Schedule periodic plot update
    window.after(100, update_plot_thread)  # Start the update plot process

    # Start event loop so the GUI can detect events such as button clicks, key presses, etc.
    window.mainloop()

def send_quit():
    global gui_send_message  # Command that the GUI has requested be sent to the Cybot
    global window  # Main GUI window
    
    gui_send_message = "quit\n"  # Update the message for the Client to send
    time.sleep(1)
    window.destroy()  # Exit GUI

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
    window.after(100, update_plot_thread)  # Schedule the next update in the GUI thread

def embed_initial_plot():
    global polar_fig, polar_ax
    global xy_fig, xy_ax

    polar_fig, polar_ax = plt.subplots(subplot_kw={'projection': 'polar'})
    xy_fig, xy_ax = plt.subplots()

    # Configure initial plot settings
    polar_ax.set_title("CyBot Sensor Scan (0-180 Degrees)")
    polar_ax.set_rmax(120)
    polar_ax.grid(True)
    
    xy_ax.set_xlim(-100, 100)
    xy_ax.set_ylim(-100, 100)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)
    
    # Show the plots in separate windows
    plt.figure(polar_fig.number)
    plt.show(block=False)
    plt.figure(xy_fig.number)
    plt.show(block=False)

def update_plot():
    global polar_fig, polar_ax
    global xy_fig, xy_ax
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
    distances = [float(dist) for dist in re.findall(distance_pattern, data)]
    distances = np.array(distances)
    angle_radians = (np.pi / 180) * degrees

    ob_ang = [int(ob_deg) for ob_deg in re.findall(ob_ang_pattern, data)]
    ob_r = [float(ob_dist) for ob_dist in re.findall(ob_r_pattern, data)]
    ob_width = [float(ob_diam) for ob_diam in re.findall(ob_r_width, data)]

    ob_ang = np.array(ob_ang)
    ob_rad = (np.pi / 180) * ob_ang
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

    # Update the XY plot
    xy_ax.clear()

    # Dynamically adjust plot bounds based on position and path
    margin = 20  # extra space around bot
    min_x = min(path[0] + [position[0]]) - margin
    max_x = max(path[0] + [position[0]]) + margin
    min_y = min(path[1] + [position[1]]) - margin
    max_y = max(path[1] + [position[1]]) + margin

    xy_ax.set_xlim(min_x, max_x)
    xy_ax.set_ylim(min_y, max_y)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)

    # Plot path
    xy_ax.plot(path[0], path[1], color='green', linestyle='-', linewidth=2, label='Path')

    # Plot current position
    xy_ax.plot(position[0], position[1], 'ro', label='CyBot')

    # Plot heading arrow
    arrow_length = 5
    arrow_dx = arrow_length * np.cos(np.radians(heading))
    arrow_dy = arrow_length * np.sin(np.radians(heading))
    xy_ax.arrow(position[0], position[1], arrow_dx, arrow_dy,
                head_width=2, head_length=3, fc='blue', ec='blue', label='Direction')

    xy_ax.legend()

    plt.figure(xy_fig.number)
    plt.draw()


def update_heading(delta_angle):
    global heading
    heading = (heading + delta_angle) % 360  # Keep in 0-359°

def update_position(distance):
    global position, heading, path
    rad = np.radians(heading)
    dx = distance * np.cos(rad)
    dy = distance * np.sin(rad)
    position[0] += dx
    position[1] += dy
    path[0].append(position[0])
    path[1].append(position[1])

def read_cybot(cybot):
    while True:
        try:
            rx_message = cybot.readline()
            decoded = rx_message.decode().strip()
            print(decoded)
            if decoded:  # Only print if there's actually something there
                if decoded[0] == 's':
                    file_object = open(full_path + sensor_file,'w')
                    file_object.write(rx_message.decode()[1:])
                    print(rx_message.decode()[1:])
                    
                    while (rx_message.decode().strip() != "END"):
                        rx_message = cybot.readline()
                        file_object.write(rx_message.decode())
                        print(rx_message.decode())

                    file_object.close()
                    update_gui = True
                elif decoded[0] in ('w', 'a', 'd'):
                    file_object = open(full_path + movement_file,'a')
                    print(rx_message.decode()[1:])
                    
                    while (rx_message.decode().strip() != "END"):
                        rx_message = cybot.readline()
                        decoded_line = rx_message.decode().strip()
                        print(decoded_line)

                        # Parse forward movement
                        forward_match = re.search(r"Drove Forward: (\d+\.\d+)", decoded_line)
                        left_turn_match = re.search(r"Turned Left (\d+\.\d+)", decoded_line)
                        right_turn_match = re.search(r"Turned Right (\d+\.\d+)", decoded_line)

                        if forward_match:
                            dist = float(forward_match.group(1))
                            update_position(dist)
                        elif left_turn_match:
                            angle = float(left_turn_match.group(1))
                            update_heading(angle)
                        elif right_turn_match:
                            angle = float(right_turn_match.group(1))
                            update_heading(-angle)

                    file_object.close()
                    update_gui = True
                else:
                    print(f"Received: {decoded}")
        except Exception as e:
            print(f"Error reading from cybot: {e}")
            break

def socket_thread():
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    global update_gui

    HOST = "127.0.0.1"  # Use this if using the MOCK Cybot
    PORT = 288  # The port used by the server
    cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cybot_socket.connect((HOST, PORT))

    cybot = cybot_socket.makefile("rbw", buffering=0)

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
    time.sleep(2)
    cybot.close()
    cybot_socket.close()

main()
