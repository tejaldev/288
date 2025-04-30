import time
import socket
import tkinter as tk
import threading
import os
import numpy as np
import matplotlib.pyplot as plt
import re
import queue
from collections import deque
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

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
polar_canvas = None
xy_canvas = None

command_queue = queue.Queue()

absolute_path = os.path.dirname(__file__)
relative_path = "./"
full_path = os.path.join(absolute_path, relative_path)
sensor_file = 'sensor-scan.txt'
movement_file = 'movement.txt'

position = [0.0, 0.0]
heading = 0.0
path = [[0.0], [0.0]]

terminal_output = None
message_log = deque(maxlen=3)

def main():
    global window, terminal_output
    window = tk.Tk()
    window.title("CyBot Control Interface")
    window.geometry("1200x700")

    control_frame = tk.Frame(window)
    control_frame.pack(side=tk.TOP, fill=tk.X)

    quit_command_Button = tk.Button(control_frame, text="Press to Quit", command=send_quit)
    quit_command_Button.pack(pady=5)

    # Terminal Display
    terminal_frame = tk.Frame(window)
    terminal_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)

    terminal_label = tk.Label(terminal_frame, text="CyBot Terminal Output (Last 3 Messages):")
    terminal_label.pack(anchor='w')

    terminal_output = tk.Text(terminal_frame, height=5, width=100, state='disabled', bg='black', fg='lime')
    terminal_output.pack(fill=tk.X)

    window.bind("<KeyPress>", key_down)
    window.bind("<KeyRelease>", key_up)

    embed_initial_plot()

    my_thread = threading.Thread(target=socket_thread, daemon=True)
    my_thread.start()

    window.after(100, update_plot_thread)
    window.mainloop()

def send_quit():
    global gui_send_message
    global window
    gui_send_message = "quit\n"
    time.sleep(1)
    window.destroy()

def key_down(event):
    global curr_keys, first_key, key_is_pressed
    if event.char not in curr_keys:
        curr_keys.add(event.char)
        if first_key is None and event.char in ['w', 'a', 's', 'd', 'b']:
            first_key = event.char
            key_is_pressed = True
            command_queue.put(event.char)

def key_up(event):
    global curr_keys, first_key, key_is_released, update_gui
    if event.char in curr_keys:
        curr_keys.remove(event.char)
        if event.char == first_key:
            first_key = None
            key_is_released = True
            update_gui = True
            command_queue.put('q')

def embed_initial_plot():
    global polar_fig, polar_ax, xy_fig, xy_ax
    global polar_canvas, xy_canvas

    plot_frame = tk.Frame(window)
    plot_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    polar_fig, polar_ax = plt.subplots(subplot_kw={'projection': 'polar'})
    xy_fig, xy_ax = plt.subplots()

    polar_ax.set_title("CyBot Sensor Scan (0-180 Degrees)")
    polar_ax.set_rmax(120)
    polar_ax.grid(True)

    xy_ax.set_xlim(-100, 100)
    xy_ax.set_ylim(-100, 100)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)

    polar_canvas = FigureCanvasTkAgg(polar_fig, master=plot_frame)
    polar_canvas.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
    polar_canvas.draw()

    xy_canvas = FigureCanvasTkAgg(xy_fig, master=plot_frame)
    xy_canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
    xy_canvas.draw()

def update_plot_thread():
    global update_gui
    if update_gui:
        update_gui = False
        update_plot()
    window.after(100, update_plot_thread)

def update_plot():
    global polar_ax, polar_canvas, xy_ax, xy_canvas

    degree_pattern = r"Degree: (\d+)"
    distance_pattern = r"ADC Distance: (\d+\.\d+)"
    ob_ang_pattern = r"Object at (\d+)"
    ob_r_pattern = r"has a distance of (\d+\.\d+)"
    ob_r_width = r"and width of (\d+\.\d+)"

    with open(full_path + sensor_file, "r") as file:
        data = file.read()

    degrees = np.array([int(deg) for deg in re.findall(degree_pattern, data)])
    distances = np.array([float(dist) for dist in re.findall(distance_pattern, data)])
    angle_radians = np.radians(degrees)

    ob_ang = np.array([int(ob_deg) for ob_deg in re.findall(ob_ang_pattern, data)])
    ob_rad = np.radians(ob_ang)
    ob_r = np.array([float(ob_dist) for ob_dist in re.findall(ob_r_pattern, data)])
    ob_width = np.array([float(ob_diam) for ob_diam in re.findall(ob_r_width, data)])

    polar_ax.clear()
    polar_ax.plot(angle_radians, distances, color='r', linewidth=4.0)
    if len(ob_ang) > 0:
        polar_ax.scatter(ob_rad, ob_r, s=ob_width**2, c='blue')
    polar_ax.set_rmax(120)
    polar_ax.set_rticks([10, 25, 50, 75, 100])
    polar_ax.set_rlabel_position(-22.5)
    polar_ax.set_thetamax(180)
    polar_ax.grid(True)
    polar_ax.set_title("Sensor Scan", size=14)

    polar_canvas.draw()

    xy_ax.clear()
    margin = 20
    min_x = min(path[0] + [position[0]]) - margin
    max_x = max(path[0] + [position[0]]) + margin
    min_y = min(path[1] + [position[1]]) - margin
    max_y = max(path[1] + [position[1]]) + margin

    xy_ax.set_xlim(min_x, max_x)
    xy_ax.set_ylim(min_y, max_y)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)

    xy_ax.plot(path[0], path[1], color='green', linestyle='-', linewidth=2, label='Path')
    xy_ax.plot(position[0], position[1], 'ro', label='CyBot')

    arrow_length = 5
    dx = arrow_length * np.cos(np.radians(heading))
    dy = arrow_length * np.sin(np.radians(heading))
    xy_ax.arrow(position[0], position[1], dx, dy, head_width=2, head_length=3, fc='blue', ec='blue', label='Direction')

    xy_ax.legend()
    xy_canvas.draw()

def update_heading(delta_angle):
    global heading
    heading = (heading + delta_angle) % 360

def update_position(distance):
    global position, heading, path
    rad = np.radians(heading)
    dx = distance * np.cos(rad)
    dy = distance * np.sin(rad)
    position[0] += dx
    position[1] += dy
    path[0].append(position[0])
    path[1].append(position[1])

def update_terminal(new_message):
    global terminal_output, message_log
    message_log.append(new_message)
    terminal_output.configure(state='normal')
    terminal_output.delete(1.0, tk.END)
    for msg in message_log:
        terminal_output.insert(tk.END, msg + '\n')
    terminal_output.configure(state='disabled')

def read_cybot(cybot):
    global update_gui
    while True:
        try:
            rx_message = cybot.readline()
            decoded = rx_message.decode().strip()
            if decoded:
                update_terminal(decoded)

                if decoded[0] == 's':
                    file_object = open(full_path + sensor_file,'w')
                    file_object.write(rx_message.decode()[1:])
                    while decoded != "END":
                        rx_message = cybot.readline()
                        decoded = rx_message.decode().strip()
                        update_terminal(decoded)
                        file_object.write(rx_message.decode())
                    file_object.close()
                    update_gui = True
                elif decoded[0] in ('w', 'a', 'd'):
                    file_object = open(full_path + movement_file,'a')
                    while decoded != "END":
                        rx_message = cybot.readline()
                        decoded_line = rx_message.decode().strip()
                        update_terminal(decoded_line)
                        forward = re.search(r"Drove Forward: (\d+\.\d+)", decoded_line)
                        left = re.search(r"Turned Left (\d+\.\d+)", decoded_line)
                        right = re.search(r"Turned Right (\d+\.\d+)", decoded_line)
                        if forward:
                            update_position(float(forward.group(1)))
                        elif left:
                            update_heading(float(left.group(1)))
                        elif right:
                            update_heading(-float(right.group(1)))
                        decoded = decoded_line
                    file_object.close()
                    update_gui = True
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
    PORT = 288          # The port used by the server
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
